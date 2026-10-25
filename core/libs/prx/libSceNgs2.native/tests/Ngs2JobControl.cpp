#include "Ngs2Test.hpp"

#include <array>
#include <cmath>
#include <stdexcept>
#include <thread>
#include <vector>

static unsigned completedCases = 0;

struct Manager {
    struct Batch { Ngs2JobHandler handler; void* context; std::uint32_t count; };
    std::vector<Batch> batches;
    unsigned order;
};

static int APS5_VABI Query(const Ngs2JobSchedulerOption*, std::uint32_t slot, Ngs2ContextBufferInfo* buffer) {
    Require(slot == 0);
    *buffer = {};
    buffer->host_buffer_size = 64;
    return SCE_NGS2_OK;
}
static int APS5_VABI Initialize(const Ngs2JobSchedulerOption* option, std::uint32_t slot, Ngs2ContextBufferInfo*, std::uintptr_t* handle) {
    Require(slot == 0);
    *handle = option->user_data;
    return SCE_NGS2_OK;
}
static int APS5_VABI Submit(std::uintptr_t handle, Ngs2JobHandler handler, void* context, std::uint32_t count) {
    reinterpret_cast<Manager*>(handle)->batches.push_back({handler, context, count});
    return SCE_NGS2_OK;
}
static int APS5_VABI Complete(std::uintptr_t handle) {
    auto& manager = *reinterpret_cast<Manager*>(handle);
    for (const auto& batch : manager.batches) {
        std::vector<std::thread> workers;
        if (manager.order == 2) {
            for (std::uint32_t i = 0; i < batch.count; ++i) workers.emplace_back([&, i] { batch.handler(batch.context, i); });
        } else workers.emplace_back([&] {
            for (std::uint32_t i = 0; i < batch.count; ++i) batch.handler(batch.context, manager.order == 0 ? i : batch.count - i - 1);
        });
        for (auto& worker : workers) worker.join();
    }
    manager.batches.clear();
    return SCE_NGS2_OK;
}
static int APS5_VABI Destroy(std::uintptr_t handle) {
    Require(reinterpret_cast<Manager*>(handle)->batches.empty());
    return SCE_NGS2_OK;
}

struct ControlState {
    std::uintptr_t voice;
    std::uint32_t event;
    bool commands;
    bool sent = false;
    bool rejectedBatch = false;
    unsigned delay = 0;
    bool unusualCommand = false;
};
static int APS5_VABI StopPeer(Ngs2UserFxProcessContext* context) {
    auto& state = *reinterpret_cast<ControlState*>(context->user_data0);
    if (!state.sent && state.delay != 0) --state.delay;
    else if (!state.sent) {
        state.sent = true;
        std::uint32_t before = 0, after = 0;
        Require(sceNgs2VoiceGetStateFlags(state.voice, &before) == SCE_NGS2_OK);
        if (state.rejectedBatch) {
            std::array<Ngs2VoiceCommand, 2> commands{};
            for (auto& command : commands) { command.id = 2; command.type = 4; command.value.u = state.event; }
            commands[1].value.u = 0x80;
            bool rejected = false;
            try { rejected = sceNgs2VoiceRunCommands(state.voice, commands.data(), commands.size()) != SCE_NGS2_OK; }
            catch (const std::runtime_error&) { rejected = true; }
            Require(rejected);
        } else if (state.commands) {
            Ngs2VoiceCommand command{};
            command.id = state.unusualCommand ? 0xff000002u : 2u;
            command.type = state.unusualCommand ? 0xff : 4;
            command.flags = state.unusualCommand ? 0xfe : 0;
            command.count = state.unusualCommand ? 0xffff : 0;
            command.value.u = state.event;
            Require(sceNgs2VoiceRunCommands(state.voice, &command, 1) == SCE_NGS2_OK);
        } else Event(state.voice, state.event);
        Require(sceNgs2VoiceGetStateFlags(state.voice, &after) == SCE_NGS2_OK && after == before);
    }
    for (std::uint32_t i = 0; i < context->num_grain_samples; ++i) context->channel_data[0][i] = 0.0f;
    return SCE_NGS2_OK;
}

static std::uintptr_t Sampler(std::uintptr_t system, bool custom) {
    if (!custom) return Voice(CreateRack(system, SCE_NGS2_RACK_ID_SAMPLER));
    Ngs2CustomSamplerRackOption option{};
    auto& rack = option.custom_rack_option;
    auto& common = rack.rack_option;
    common.size = sizeof(option);
    common.max_grain_samples = 512;
    common.max_voices = 1;
    common.max_matrices = 1;
    common.max_ports = 1;
    rack.num_modules = 1;
    rack.num_buffers = 1;
    rack.state_size = 48;
    rack.module[0].module_id = SCE_NGS2_CUSTOM_MODULE_ID_PITCH_SHIFT;
    option.max_channel_works = 1;
    option.max_waveform_blocks = 4;
    Ngs2ContextBufferInfo query{};
    Require(sceNgs2RackQueryBufferSize(SCE_NGS2_RACK_ID_CUSTOM_SAMPLER, &common, &query) == SCE_NGS2_OK);
    const auto buffer = Buffer(query);
    std::uintptr_t handle = 0;
    Require(sceNgs2RackCreate(system, SCE_NGS2_RACK_ID_CUSTOM_SAMPLER, &common, &buffer, &handle) == SCE_NGS2_OK);
    return Voice(handle);
}

static void TestStop(bool custom, bool commands, std::uint32_t event, unsigned order, bool active = false, bool replay = false, bool rejectedBatch = false, bool unusualCommand = false) {
    const auto firstBuffer = usedBuffers;
    const unsigned grainFrames = active ? 256 : 64;
    Manager manager{{}, order};
    Ngs2JobSchedulerOption scheduler{};
    scheduler.size = sizeof(scheduler);
    scheduler.user_data = reinterpret_cast<std::uintptr_t>(&manager);
    scheduler.query_handler = Query;
    scheduler.init_handler = Initialize;
    scheduler.destroy_handler = Destroy;
    scheduler.submit_handler = Submit;
    scheduler.flush_handler = Complete;
    scheduler.join_handler = Complete;
    scheduler.num_workers = 2;
    scheduler.max_jobs = 16;
    Ngs2SystemOption option{};
    option.size = sizeof(option);
    option.max_grain_samples = 512;
    option.num_grain_samples = grainFrames;
    option.sample_rate = 48000;
    option.max_voice_channels = 8;
    option.job_scheduler_options[0] = reinterpret_cast<std::uintptr_t>(&scheduler);
    Ngs2ContextBufferInfo query{};
    Require(sceNgs2SystemQueryBufferSize(&option, &query) == SCE_NGS2_OK);
    const auto buffer = Buffer(query);
    std::uintptr_t system = 0;
    Require(sceNgs2SystemCreate(&option, &buffer, &system) == SCE_NGS2_OK);
    const auto master = Mastering(system, 1);
    const auto sender = Voice(CreateRack(system, SCE_NGS2_RACK_ID_SUBMIXER));
    Control(sender, SCE_NGS2_SUBMIXER_VOICE_PARAM_SETUP, Ngs2SubmixerVoiceSetupParam{{}, 1, 0});
    const auto voice = Sampler(system, custom);
    const auto setup = custom ? SCE_NGS2_CUSTOM_SAMPLER_VOICE_PARAM_SETUP : SCE_NGS2_SAMPLER_VOICE_PARAM_SETUP;
    const auto blocks = custom ? SCE_NGS2_CUSTOM_SAMPLER_VOICE_PARAM_ADD_WAVEFORM_BLOCKS : SCE_NGS2_SAMPLER_VOICE_PARAM_ADD_WAVEFORM_BLOCKS;
    Control(voice, setup, Ngs2SamplerVoiceSetupParam{{}, {SCE_NGS2_WAVEFORM_TYPE_PCM_I16L, 1, 48000, 0, 0, 0}});
    if (active) Control(voice, SCE_NGS2_CUSTOM_VOICE_PARAM_PITCH_SHIFT, Ngs2CustomVoicePitchShiftParam{{}, 1200, 0, 0, 0});
    std::array<std::int16_t, 4096> pcm{};
    pcm.fill(1000);
    const Ngs2WaveformBlock block{0, sizeof(pcm), 0, 0, pcm.size(), 0, 0};
    Control(voice, blocks, Ngs2SamplerVoiceWaveformBlocksParam{{}, pcm.data(), 0, 1, &block});
    const unsigned stopGrain = active ? 10 : 0;
    ControlState state{voice, event, commands, false, rejectedBatch, stopGrain, unusualCommand};
    Control(sender, SCE_NGS2_SUBMIXER_VOICE_PARAM_USER_FX,
            Ngs2SubmixerVoiceUserFxParam{{}, StopPeer, reinterpret_cast<std::uintptr_t>(&state), 0, 0});
    Patch(voice, master);
    Patch(sender, master);
    if (unusualCommand) {
        Ngs2VoiceCommand play{};
        play.id = 0xff000002u; play.type = 0xff; play.flags = 0xfe; play.count = 0xffff;
        play.value.u = SCE_NGS2_VOICE_EVENT_PLAY;
        Require(sceNgs2VoiceRunCommands(voice, &play, 1) == SCE_NGS2_OK);
    } else Event(voice, SCE_NGS2_VOICE_EVENT_PLAY);
    Event(sender, SCE_NGS2_VOICE_EVENT_PLAY);
    std::array<float, 256> samples{};
    const Ngs2RenderBufferInfo output{samples.data(), sizeof(samples), SCE_NGS2_WAVEFORM_TYPE_PCM_F32L, 1};
    for (unsigned grain = 0; grain < stopGrain + 4; ++grain) {
        samples.fill(-1234.0f);
        Require(sceNgs2SystemRender(system, &output, 1) == SCE_NGS2_OK && manager.batches.empty());
        std::uint32_t flags = 0;
        Require(sceNgs2VoiceGetStateFlags(voice, &flags) == SCE_NGS2_OK);
        Require(flags == (grain <= stopGrain || replay || rejectedBatch ? 3u : 0u));
        if (grain >= stopGrain) for (unsigned i = 0; i < grainFrames; ++i) {
            const bool restarting = grain == 0 || (replay && grain == 1);
            const float level = active ? 0.03068515658378601f : 1000.0f / 32768.0f;
            const float expected = replay || rejectedBatch ? (restarting && i < 3 ? 0.0f : level) :
                grain == stopGrain ? (!active && i < 3 ? 0.0f : level) :
                grain == stopGrain + 1 && i < 64 ? level * (1.0f - static_cast<float>(i) / 64.0f) : 0.0f;
            if (std::fabs(samples[i] - expected) > 1.0e-7f) std::fprintf(stderr,
                "custom=%d commands=%d event=%u order=%u active=%d replay=%d rejected=%d grain=%u sample=%u actual=%g expected=%g\n",
                custom, commands, event, order, active, replay, rejectedBatch, grain, i, samples[i], expected);
            Require(std::fabs(samples[i] - expected) <= 1.0e-7f);
        }
        for (unsigned i = grainFrames; i < samples.size(); ++i) Require(samples[i] == -1234.0f);
        if (replay && grain == 0) Event(voice, SCE_NGS2_VOICE_EVENT_PLAY);
    }
    Require(state.sent && sceNgs2SystemDestroy(system, nullptr) == SCE_NGS2_OK);
    usedBuffers = firstBuffer;
    ++completedCases;
}

int main() {
    for (bool custom : {false, true}) for (bool commands : {false, true})
        for (auto event : {SCE_NGS2_VOICE_EVENT_STOP, SCE_NGS2_VOICE_EVENT_STOP_IMM})
            for (unsigned order = 0; order < 3; ++order) TestStop(custom, commands, event, order);
    for (auto event : {SCE_NGS2_VOICE_EVENT_STOP, SCE_NGS2_VOICE_EVENT_STOP_IMM})
        for (unsigned order = 0; order < 3; ++order) TestStop(true, true, event, order, true);
    TestStop(false, false, SCE_NGS2_VOICE_EVENT_STOP, 0, false, true);
    TestStop(true, true, SCE_NGS2_VOICE_EVENT_STOP_IMM, 2, false, true);
    TestStop(false, true, SCE_NGS2_VOICE_EVENT_STOP, 2, false, false, true);
    TestStop(true, true, SCE_NGS2_VOICE_EVENT_STOP_IMM, 1, false, false, true);
    TestStop(false, true, SCE_NGS2_VOICE_EVENT_STOP, 1, false, false, false, true);
    std::printf("NGS2 worker controls: %u scenarios passed\n", completedCases);
    return 0;
}
