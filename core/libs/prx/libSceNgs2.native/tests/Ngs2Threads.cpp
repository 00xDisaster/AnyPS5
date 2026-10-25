#include "Ngs2JobTest.hpp"

#include <array>
#include <atomic>
#include <condition_variable>
#include <latch>
#include <mutex>

struct ThreadControl {
    std::mutex mutex;
    std::condition_variable condition;
    bool entered = false;
    bool release = false;
    bool park = true;
    bool failFirst = false;
    std::uintptr_t voice = 0;
    std::atomic<bool> active = false;
    std::atomic<unsigned> calls = 0;
    std::atomic<unsigned> cleanups = 0;
    std::array<float, 4> before{};
};

static int ThreadValue(std::uintptr_t voice, float value) {
    const Ngs2CustomVoiceUserFx2Param param{{sizeof(param), 0, SCE_NGS2_CUSTOM_VOICE_PARAM_USER_FX2}, &value, sizeof(value)};
    return sceNgs2VoiceControl(voice, &param.header);
}

static int APS5_VABI ThreadSetup(Ngs2UserFx2SetupContext* context) {
    *static_cast<float*>(context->param) = 1.0f;
    return SCE_NGS2_OK;
}

static int APS5_VABI ThreadCleanup(Ngs2UserFx2CleanupContext* context) {
    auto& state = *reinterpret_cast<ThreadControl*>(context->user_data);
    Require(!state.active.load());
    std::uint32_t flags = 99;
    Require(sceNgs2VoiceGetStateFlags(state.voice, &flags) == SCE_NGS2_ERROR_INVALID_VOICE_HANDLE && flags == 99);
    ++state.cleanups;
    return SCE_NGS2_OK;
}

static int APS5_VABI ThreadHandler(Ngs2UserFx2ControlContext* context) {
    auto& state = *reinterpret_cast<ThreadControl*>(context->user_data);
    Require(!state.active.exchange(true));
    const auto index = state.calls.fetch_add(1);
    Require(index < state.before.size());
    state.before[index] = *static_cast<float*>(context->param);
    const float value = *static_cast<const float*>(context->data);
    if (state.park && value == 1.0f) {
        std::unique_lock lock(state.mutex);
        state.entered = true;
        state.condition.notify_all();
        state.condition.wait(lock, [&] { return state.release; });
    }
    *static_cast<float*>(context->param) = value * 2.0f;
    state.active = false;
    return state.failFirst && value == 1.0f ? -55 : SCE_NGS2_OK;
}

static int APS5_VABI ThreadProcess(Ngs2UserFx2ProcessContext* context) {
    const float value = *static_cast<const float*>(context->param) * 0.125f;
    for (unsigned channel = 0; channel < context->num_output_channels; ++channel)
        std::fill_n(context->channel_data[channel], context->num_grain_samples, value);
    return SCE_NGS2_OK;
}

static std::uintptr_t ThreadVoice(std::uintptr_t system, ThreadControl& state, std::uintptr_t& rack) {
    const Ngs2CustomUserFx2ModuleOption module{{sizeof(module)}, ThreadSetup, ThreadCleanup, ThreadHandler, ThreadProcess,
        0, sizeof(float), 0, reinterpret_cast<std::uintptr_t>(&state)};
    Ngs2CustomSubmixerRackOption option{};
    auto& custom = option.custom_rack_option;
    auto& common = custom.rack_option;
    common.size = sizeof(option);
    common.max_grain_samples = 512;
    common.max_voices = 1;
    common.max_matrices = 1;
    common.max_ports = 1;
    custom.state_size = 16;
    custom.num_buffers = 1;
    custom.num_modules = 1;
    custom.module[0].module_id = SCE_NGS2_CUSTOM_MODULE_ID_USER_FX2;
    custom.module[0].option = &module.custom_module_option;
    option.max_channels = 2;
    option.max_inputs = 1;
    Ngs2ContextBufferInfo query{};
    Require(sceNgs2RackQueryBufferSize(SCE_NGS2_RACK_ID_CUSTOM_SUBMIXER, &common, &query) == SCE_NGS2_OK);
    const auto buffer = Buffer(query);
    Require(sceNgs2RackCreate(system, SCE_NGS2_RACK_ID_CUSTOM_SUBMIXER, &common, &buffer, &rack) == SCE_NGS2_OK);
    const auto voice = Voice(rack);
    state.voice = voice;
    Control(voice, SCE_NGS2_CUSTOM_SUBMIXER_VOICE_PARAM_SETUP, Ngs2CustomSubmixerVoiceSetupParam{{}, 2, 2, 0});
    return voice;
}

static void WaitEntered(ThreadControl& state) {
    std::unique_lock lock(state.mutex);
    state.condition.wait(lock, [&] { return state.entered; });
}

static void ReleaseControl(ThreadControl& state) {
    std::lock_guard lock(state.mutex);
    state.release = true;
    state.condition.notify_all();
}

static std::uintptr_t ThreadSystem() {
    Ngs2SystemOption option{};
    option.size = sizeof(option);
    option.max_grain_samples = 512;
    option.num_grain_samples = 64;
    option.sample_rate = 48000;
    option.max_voice_channels = 8;
    Ngs2ContextBufferInfo query{};
    Require(sceNgs2SystemQueryBufferSize(&option, &query) == SCE_NGS2_OK);
    const auto buffer = Buffer(query);
    std::uintptr_t system = 0;
    Require(sceNgs2SystemCreate(&option, &buffer, &system) == SCE_NGS2_OK);
    return system;
}

static void TestSerialization(bool failure) {
    const auto firstBuffer = usedBuffers;
    const auto system = ThreadSystem();
    ThreadControl state, peerState;
    peerState.park = false;
    std::uintptr_t rack = 0, peerRack = 0;
    const auto voice = ThreadVoice(system, state, rack);
    const auto peer = ThreadVoice(system, peerState, peerRack);
    state.failFirst = failure;
    std::thread first([&] { Require(ThreadValue(voice, 1.0f) == (failure ? -55 : SCE_NGS2_OK)); });
    WaitEntered(state);
    std::latch started(2);
    std::atomic<bool> queried = false;
    std::thread second([&] {
        started.count_down();
        Require(ThreadValue(voice, 2.0f) == SCE_NGS2_OK);
    });
    std::thread query([&] {
        started.count_down();
        std::uint32_t flags = 99;
        Require(sceNgs2VoiceGetStateFlags(voice, &flags) == SCE_NGS2_OK && flags == 0);
        queried = true;
    });
    started.wait();
    Require(ThreadValue(peer, 0.75f) == SCE_NGS2_OK);
    Require(state.active.load() && state.calls == 1 && !queried.load());
    ReleaseControl(state);
    first.join();
    second.join();
    query.join();
    Require(queried && state.calls == 2 && state.before[0] == 1.0f && state.before[1] == 2.0f);
    Require(ThreadValue(voice, 3.0f) == SCE_NGS2_OK && state.before[2] == 4.0f);
    Require(sceNgs2RackDestroy(rack, nullptr) == SCE_NGS2_OK);
    Require(sceNgs2RackDestroy(peerRack, nullptr) == SCE_NGS2_OK);
    Require(sceNgs2SystemDestroy(system, nullptr) == SCE_NGS2_OK);
    Require(state.cleanups == 1 && peerState.cleanups == 1);
    usedBuffers = firstBuffer;
}

static void TestDestruction(bool destroySystem) {
    const auto firstBuffer = usedBuffers;
    const auto system = ThreadSystem();
    ThreadControl state;
    std::uintptr_t rack = 0;
    const auto voice = ThreadVoice(system, state, rack);
    std::thread control([&] { Require(ThreadValue(voice, 1.0f) == SCE_NGS2_OK); });
    WaitEntered(state);
    std::latch started(1);
    std::thread destroy([&] {
        started.count_down();
        const int result = destroySystem ? sceNgs2SystemDestroy(system, nullptr) : sceNgs2RackDestroy(rack, nullptr);
        Require(result == SCE_NGS2_OK);
    });
    started.wait();
    Require(state.cleanups == 0);
    ReleaseControl(state);
    control.join();
    destroy.join();
    Require(!state.active.load());
    if (!destroySystem) Require(state.cleanups == 1);
    if (!destroySystem) Require(sceNgs2SystemDestroy(system, nullptr) == SCE_NGS2_OK);
    usedBuffers = firstBuffer;
}

static void TestRenderWait(bool crossSystem) {
    const auto firstBuffer = usedBuffers;
    const auto system = ThreadSystem();
    const auto master = Mastering(system, 2);
    ThreadControl state, peerState;
    peerState.park = false;
    std::uintptr_t rack = 0, peerRack = 0;
    const auto voice = ThreadVoice(system, state, rack);
    const auto peerSystem = crossSystem ? ThreadSystem() : system;
    const auto peer = ThreadVoice(peerSystem, peerState, peerRack);
    Require(ThreadValue(voice, 0.5f) == SCE_NGS2_OK);
    Patch(voice, master);
    Event(voice, SCE_NGS2_VOICE_EVENT_PLAY);
    std::thread control([&] { Require(ThreadValue(voice, 1.0f) == SCE_NGS2_OK); });
    WaitEntered(state);
    std::array<float, 128> audio{};
    std::latch started(1);
    std::thread render([&] {
        started.count_down();
        const Ngs2RenderBufferInfo output{audio.data(), sizeof(audio), SCE_NGS2_WAVEFORM_TYPE_PCM_F32L, 2};
        Require(sceNgs2SystemRender(system, &output, 1) == SCE_NGS2_OK);
    });
    started.wait();
    Require(ThreadValue(peer, 0.75f) == SCE_NGS2_OK);
    ReleaseControl(state);
    control.join();
    render.join();
    for (float value : audio) Require(value == 0.25f);
    Require(sceNgs2RackDestroy(rack, nullptr) == SCE_NGS2_OK);
    Require(sceNgs2RackDestroy(peerRack, nullptr) == SCE_NGS2_OK);
    Require(sceNgs2SystemDestroy(system, nullptr) == SCE_NGS2_OK);
    if (crossSystem) Require(sceNgs2SystemDestroy(peerSystem, nullptr) == SCE_NGS2_OK);
    Require(state.cleanups == 1 && peerState.cleanups == 1);
    usedBuffers = firstBuffer;
}

int main() {
    TestSerialization(false);
    TestSerialization(true);
    TestDestruction(false);
    TestDestruction(true);
    TestRenderWait(false);
    TestRenderWait(true);
    std::puts("NGS2 cross-thread ownership: 6 scenarios passed");
    return 0;
}
