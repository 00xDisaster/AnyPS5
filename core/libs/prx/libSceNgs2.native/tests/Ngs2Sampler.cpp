#include "Ngs2Test.hpp"

#include <array>
#include <cmath>
#include <vector>

static constexpr std::uint32_t RenderGrain = 256;
static constexpr std::array<std::uint32_t, 8> Checkpoints{8, 12, 14, 20, 24, 30, 32, 40};

struct Case {
    std::uint32_t rate;
    float pitch;
    std::int32_t cents;
    bool events;
    bool bypass;
    std::array<std::array<float, 6>, 8> expected;
};

static const Case Cases[]{
    {48000, 1.0f, 0, false, true, {}},
    {44100, 1.3f, 700, false, false, {{
        {0.137265846f, -0.0252674483f, 0.0799425617f, 0.0359304883f, -0.00174468313f, -0.0710403621f},
        {-0.0386396796f, 0.113722473f, -0.0735076442f, 0.0788545236f, 0.115713701f, -0.0370367542f},
        {-0.0208529234f, 0.131442904f, -0.0557390116f, 0.0966642424f, 0.133591712f, -0.0192373525f},
        {0.0472084023f, -0.108676627f, 0.0123404339f, -0.158158675f, -0.103753135f, 0.0490724295f},
        {0.0901324302f, -0.0657368749f, 0.055264473f, -0.100620545f, -0.0606985465f, 0.0921270251f},
        {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    }}},
    {24000, 0.7f, -700, false, false, {{
        {-0.127651423f, -0.075771533f, -0.0938414633f, -0.0419081151f, 0.0508089289f, 0.102808177f},
        {-0.0176326279f, 0.0342472568f, 0.017153997f, 0.0691795796f, -0.144594505f, -0.092595242f},
        {0.0412285328f, 0.0931084156f, 0.0762526914f, 0.128278285f, -0.0875838622f, -0.0355783924f},
        {-0.0992715359f, -0.0473916531f, -0.06464196f, -0.0126163764f, 0.0828370824f, 0.134836316f},
        {0.014179945f, 0.0660598278f, 0.0491285622f, 0.101154149f, -0.114638358f, -0.062632896f},
        {-0.125253201f, -0.0733733177f, -0.0906971246f, -0.0386715382f, 0.0567951128f, 0.108794361f},
        {-0.0694803074f, -0.0176004246f, -0.0356095247f, 0.0163238123f, 0.111254223f, -0.145683259f},
        {-0.16338414f, -0.100421906f, -0.117817312f, -0.0657917261f, 0.0296836458f, 0.0816829056f},
    }}},
    {48000, 1.3f, 700, true, false, {{
        {-0.0924464986f, -0.0188726112f, -0.0760293677f, 0.0488269478f, 0.156057909f, 0.0191847961f},
        {0.0912231579f, -0.0198750645f, 0.0805369988f, -0.0305612143f, 0.000996570569f, -0.000805972959f},
        {0.0f, -0.0f, 0.035238862f, -0.0581547283f, 0.0200667791f, -0.0917441919f},
        {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
        {0.130914479f, 0.0156071745f, 0.117588684f, 0.00487004546f, 0.0995834172f, -0.0121165738f},
        {0.0912231579f, -0.0198750645f, 0.0805369988f, -0.0305612143f, 0.0637805164f, -0.0515822694f},
        {0.0705910772f, 0.0424018614f, 0.108651653f, -0.0748682022f, -0.028412085f, 0.0848536044f},
    }}},
};

static void TestSampler(const Case& test) {
    const auto firstBuffer = usedBuffers;
    const auto system = CreateSystem();
    Require(sceNgs2SystemSetGrainSamples(system, RenderGrain) == SCE_NGS2_OK);
    const auto master = Mastering(system, 2);
    Ngs2CustomSamplerRackOption option{};
    auto& custom = option.custom_rack_option;
    auto& common = custom.rack_option;
    common.size = sizeof(option);
    common.max_grain_samples = 512;
    common.max_voices = 1;
    common.max_matrices = 1;
    common.max_ports = 1;
    custom.num_modules = 1;
    custom.num_buffers = 1;
    custom.state_size = 48;
    custom.module[0].module_id = SCE_NGS2_CUSTOM_MODULE_ID_PITCH_SHIFT;
    option.max_channel_works = 2;
    option.max_waveform_blocks = 4;
    Ngs2ContextBufferInfo query{};
    Require(sceNgs2RackQueryBufferSize(SCE_NGS2_RACK_ID_CUSTOM_SAMPLER, &common, &query) == SCE_NGS2_OK);
    const auto storage = Buffer(query);
    std::uintptr_t rack = 0;
    Require(sceNgs2RackCreate(system, SCE_NGS2_RACK_ID_CUSTOM_SAMPLER, &common, &storage, &rack) == SCE_NGS2_OK);
    const auto voice = Voice(rack);
    Control(voice, SCE_NGS2_CUSTOM_SAMPLER_VOICE_PARAM_SETUP,
            Ngs2SamplerVoiceSetupParam{{}, {SCE_NGS2_WAVEFORM_TYPE_PCM_I16L, 2, test.rate, 0, 0, 0}});
    if (!test.bypass) Control(voice, SCE_NGS2_CUSTOM_VOICE_PARAM_PITCH_SHIFT, Ngs2CustomVoicePitchShiftParam{{}, test.cents, 0, 0, 0});
    Control(voice, SCE_NGS2_CUSTOM_SAMPLER_VOICE_PARAM_PITCH, Ngs2SamplerVoicePitchParam{{}, test.pitch});
    Patch(voice, master);
    std::vector<std::int16_t> pcm(8192 * 2);
    for (std::uint32_t i = 0; i < pcm.size(); ++i) pcm[i] = static_cast<std::int16_t>((static_cast<int>((i / 2 + i % 2 * 17) % 101) - 50) * 100);
    const Ngs2WaveformBlock block{0, pcm.size() * 2, 0, 0, 8192, 0, 0x55};
    const auto submit = [&](std::uint32_t flags) {
        Control(voice, SCE_NGS2_CUSTOM_SAMPLER_VOICE_PARAM_ADD_WAVEFORM_BLOCKS,
                Ngs2SamplerVoiceWaveformBlocksParam{{}, pcm.data(), flags, 1, &block});
    };
    submit(0);
    Event(voice, SCE_NGS2_VOICE_EVENT_PLAY);
    std::array<float, RenderGrain * 2> samples{};
    const Ngs2RenderBufferInfo output{samples.data(), sizeof(samples), SCE_NGS2_WAVEFORM_TYPE_PCM_F32L, 2};
    for (std::uint32_t grain = 0; grain <= Checkpoints.back(); ++grain) {
        if (test.events) {
            if (grain == 8) submit(SCE_NGS2_WAVEFORM_BLOCKS_FLAG_RESET);
            if (grain == 12) Event(voice, SCE_NGS2_VOICE_EVENT_PAUSE);
            if (grain == 14) Event(voice, SCE_NGS2_VOICE_EVENT_RESUME);
            if (grain == 20) {
                Event(voice, SCE_NGS2_VOICE_EVENT_STOP_IMM);
                Event(voice, SCE_NGS2_VOICE_EVENT_PLAY);
            }
            if (grain == 30) Control(voice, SCE_NGS2_CUSTOM_SAMPLER_VOICE_PARAM_PITCH, Ngs2SamplerVoicePitchParam{{}, 0.5f});
        }
        Require(sceNgs2SystemRender(system, &output, 1) == SCE_NGS2_OK);
        for (const float sample : samples) Require(std::isfinite(sample));
        if (test.bypass) {
            for (std::uint32_t i = 0; i < samples.size(); ++i) {
                const auto frame = grain * RenderGrain + i / 2;
                const float expected = frame >= 3 && frame < 8195 ? pcm[(frame - 3) * 2 + i % 2] / 32768.0f : 0.0f;
                Require(samples[i] == expected);
            }
            continue;
        }
        for (std::size_t i = 0; i < Checkpoints.size(); ++i) {
            if (grain != Checkpoints[i]) continue;
            constexpr std::array<std::uint32_t, 6> indices{0, 1, 100, 101, 510, 511};
            for (std::size_t j = 0; j < indices.size(); ++j) Require(std::abs(samples[indices[j]] - test.expected[i][j]) < 1e-6f);
        }
    }
    Require(sceNgs2SystemDestroy(system, nullptr) == SCE_NGS2_OK);
    usedBuffers = firstBuffer;
}

int main() {
    for (const auto& test : Cases) TestSampler(test);
    return 0;
}
