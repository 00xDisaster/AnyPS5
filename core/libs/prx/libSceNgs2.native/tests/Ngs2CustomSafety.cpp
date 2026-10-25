#include "prx/libSceNgs2.native/tests/Ngs2Test.hpp"

#include <array>
#include <algorithm>
#include <stdexcept>
#include <string_view>

static uintptr_t target = 0;

static int APS5_VABI ChangeSetup(Ngs2UserFxProcessContext* context) {
    std::fill_n(context->channel_data[0], context->num_grain_samples, 1.0f);
    Control(target, SCE_NGS2_CUSTOM_SUBMIXER_VOICE_PARAM_SETUP, Ngs2CustomSubmixerVoiceSetupParam{{}, 8, 8, 0, 0});
    return 0;
}

static uintptr_t Custom(uintptr_t system, std::uint32_t count, bool chain) {
    Ngs2CustomSubmixerRackOption option{};
    auto& custom = option.custom_rack_option;
    custom.rack_option.size = sizeof(option);
    custom.rack_option.max_grain_samples = 512;
    custom.rack_option.max_voices = 1;
    custom.rack_option.max_ports = 1;
    custom.num_buffers = count;
    custom.num_modules = chain ? 2 : 0;
    custom.module[0].module_id = SCE_NGS2_CUSTOM_MODULE_ID_USER_FX;
    custom.module[1].module_id = SCE_NGS2_CUSTOM_MODULE_ID_GAIN;
    option.max_channels = 8;
    option.max_inputs = count;
    Ngs2ContextBufferInfo query{};
    Require(sceNgs2RackQueryBufferSize(SCE_NGS2_RACK_ID_CUSTOM_SUBMIXER, &custom.rack_option, &query) == 0);
    const auto storage = Buffer(query);
    uintptr_t rack = 0;
    Require(sceNgs2RackCreate(system, SCE_NGS2_RACK_ID_CUSTOM_SUBMIXER, &custom.rack_option, &storage, &rack) == 0);
    const auto voice = Voice(rack);
    Control(voice, SCE_NGS2_CUSTOM_SUBMIXER_VOICE_PARAM_SETUP, Ngs2CustomSubmixerVoiceSetupParam{{}, 2, 2, 0, 0});
    Event(voice, SCE_NGS2_VOICE_EVENT_PLAY);
    return voice;
}

static bool Rejected(uintptr_t system) {
    std::array<float, Grain * 2> out{};
    const Ngs2RenderBufferInfo info{out.data(), sizeof(out), SCE_NGS2_WAVEFORM_TYPE_PCM_F32L, 2};
    try {
        Require(sceNgs2SystemRender(system, &info, 1) == 0);
    } catch (const std::runtime_error& error) {
        std::printf("safely refused: %s\n", error.what());
        return true;
    }
    return false;
}

static void TestSelfCallback() {
    const auto system = CreateSystem();
    target = Custom(system, 1, true);
    Control(target, SCE_NGS2_CUSTOM_VOICE_PARAM_USER_FX, Ngs2CustomVoiceUserFxParam{{}, ChangeSetup, 0, 0, 0});
    Control(target, SCE_NGS2_CUSTOM_VOICE_PARAM_GAIN | 1, Ngs2CustomVoiceGainParam{});
    Require(Rejected(system));
    Require(sceNgs2SystemDestroy(system, nullptr) == 0);
}

static void TestUpstreamCallback() {
    const auto system = CreateSystem();
    target = Custom(system, 2, false);
    const auto source = Voice(CreateRack(system, SCE_NGS2_RACK_ID_SUBMIXER));
    Control(source, SCE_NGS2_SUBMIXER_VOICE_PARAM_SETUP, Ngs2SubmixerVoiceSetupParam{{}, 2, 0});
    Control(source, SCE_NGS2_SUBMIXER_VOICE_PARAM_USER_FX, Ngs2SubmixerVoiceUserFxParam{{}, ChangeSetup, 0, 0, 0});
    Control(source, SCE_NGS2_VOICE_PARAM_PATCH, Ngs2VoicePatchParam{{}, 0, 1, target});
    Event(source, SCE_NGS2_VOICE_EVENT_PLAY);
    Require(Rejected(system));
    Require(sceNgs2SystemDestroy(system, nullptr) == 0);
}

static void TestSetupLifecycle() {
    const auto system = CreateSystem();
    target = Custom(system, 1, true);
    Control(target, SCE_NGS2_CUSTOM_VOICE_PARAM_GAIN | 1, Ngs2CustomVoiceGainParam{});
    Require(!Rejected(system));
    Control(target, SCE_NGS2_CUSTOM_SUBMIXER_VOICE_PARAM_SETUP, Ngs2CustomSubmixerVoiceSetupParam{{}, 2, 2, 0, 0});
    Require(Rejected(system));
    Require(sceNgs2SystemDestroy(system, nullptr) == 0);
}

int main(int argc, char** argv) {
    Require(argc == 2);
    const std::string_view test = argv[1];
    if (test == "self_callback") TestSelfCallback();
    else if (test == "upstream_callback") TestUpstreamCallback();
    else if (test == "setup_lifecycle") TestSetupLifecycle();
    else return 1;
    return 0;
}
