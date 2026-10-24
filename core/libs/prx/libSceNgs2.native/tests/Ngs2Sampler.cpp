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

struct Stream {
    std::uintptr_t voice;
    std::vector<std::int16_t> pcm;
    std::uint32_t mode;
    std::uint32_t callbacks = 0;
};

static int SubmitStream(Stream& stream, std::uint32_t block, std::uint32_t flags) {
    const auto offset = block * (stream.mode == 2 ? 256u : 128u);
    const Ngs2WaveformBlock info{0, 256, 0, 0, stream.mode == 1 || stream.mode == 2 || stream.mode == 5 ? 320u : 64u, 0, stream.mode == 1 ? 0u : block};
    const Ngs2SamplerVoiceWaveformBlocksParam param{{sizeof(param), 0, SCE_NGS2_CUSTOM_SAMPLER_VOICE_PARAM_ADD_WAVEFORM_BLOCKS},
                                                   stream.pcm.data() + offset, flags, 1, &info};
    return sceNgs2VoiceControl(stream.voice, &param.header);
}

static void APS5_VABI RefillStream(const Ngs2VoiceCallbackInfo* info) {
    auto& stream = *reinterpret_cast<Stream*>(info->callback_data);
    Require(info->flag == SCE_NGS2_VOICE_CALLBACK_FLAG_BLOCK_END);
    Require(info->block_size == (stream.mode == 1 ? 1280u : 256u));
    ++stream.callbacks;
    if (stream.mode < 3 || stream.mode == 5 || stream.callbacks >= 5) return;
    const auto flags = (stream.mode == 4 ? SCE_NGS2_WAVEFORM_BLOCKS_FLAG_RESET : 0) |
                       (stream.callbacks < 4 ? SCE_NGS2_WAVEFORM_BLOCKS_FLAG_CONTINUE : 0);
    Require(SubmitStream(stream, stream.callbacks, flags) == SCE_NGS2_OK);
}

static void TestStreaming(std::uint32_t mode, std::uint32_t capacity = 2) {
    const auto firstBuffer = usedBuffers;
    const auto system = CreateSystem();
    Require(sceNgs2SystemSetGrainSamples(system, 64) == SCE_NGS2_OK);
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
    option.max_waveform_blocks = capacity;
    Ngs2ContextBufferInfo query{};
    Require(sceNgs2RackQueryBufferSize(SCE_NGS2_RACK_ID_CUSTOM_SAMPLER, &common, &query) == SCE_NGS2_OK);
    const auto storage = Buffer(query);
    std::uintptr_t rack = 0;
    Require(sceNgs2RackCreate(system, SCE_NGS2_RACK_ID_CUSTOM_SAMPLER, &common, &storage, &rack) == SCE_NGS2_OK);
    Stream stream{Voice(rack), std::vector<std::int16_t>(1280), mode};
    for (std::uint32_t i = 0; i < stream.pcm.size(); ++i) stream.pcm[i] = static_cast<std::int16_t>((i / 128 + 1) * 1000);
    Control(stream.voice, SCE_NGS2_CUSTOM_SAMPLER_VOICE_PARAM_SETUP,
            Ngs2SamplerVoiceSetupParam{{}, {SCE_NGS2_WAVEFORM_TYPE_PCM_I16L, 2, 48000, 0, 0, 0}});
    Patch(stream.voice, master);
    Require(SubmitStream(stream, 0, SCE_NGS2_WAVEFORM_BLOCKS_FLAG_CONTINUE) == SCE_NGS2_OK);
    if (mode == 0) {
        const std::array<Ngs2WaveformBlock, 3> blocks{{{0, 256, 0, 0, 64, 0, 0}, {256, 256, 0, 0, 64, 0, 0}, {512, 256, 0, 0, 64, 0, 0}}};
        const Ngs2SamplerVoiceWaveformBlocksParam invalid{{sizeof(invalid), 0, SCE_NGS2_CUSTOM_SAMPLER_VOICE_PARAM_ADD_WAVEFORM_BLOCKS},
                                                       stream.pcm.data(), SCE_NGS2_WAVEFORM_BLOCKS_FLAG_CONTINUE, blocks.size(), blocks.data()};
        Require(sceNgs2VoiceControl(stream.voice, &invalid.header) == static_cast<int>(0x804A8054u));
    }
    Control(stream.voice, SCE_NGS2_VOICE_PARAM_CALLBACK,
            Ngs2VoiceCallbackParam{{}, RefillStream, reinterpret_cast<std::uintptr_t>(&stream), SCE_NGS2_VOICE_CALLBACK_FLAG_BLOCK_END, 0});
    Event(stream.voice, SCE_NGS2_VOICE_EVENT_PLAY);
    std::array<float, 128> samples{};
    const Ngs2RenderBufferInfo output{samples.data(), sizeof(samples), SCE_NGS2_WAVEFORM_TYPE_PCM_F32L, 2};
    for (std::uint32_t grain = 0; grain < 15; ++grain) {
        if ((mode < 3 || mode == 5) && grain >= 1 && grain <= 4) {
            const auto flags = (mode == 1 || mode == 2 || mode == 5 ? SCE_NGS2_WAVEFORM_BLOCKS_FLAG_APPEND : 0) |
                               (grain < 4 ? SCE_NGS2_WAVEFORM_BLOCKS_FLAG_CONTINUE : 0);
            Require(SubmitStream(stream, grain, flags) == SCE_NGS2_OK);
        }
        if (grain == 8) {
            Event(stream.voice, SCE_NGS2_VOICE_EVENT_STOP_IMM);
            Event(stream.voice, SCE_NGS2_VOICE_EVENT_PLAY);
        }
        Require(sceNgs2SystemRender(system, &output, 1) == SCE_NGS2_OK);
        for (std::uint32_t i = 0; i < samples.size(); ++i) {
            const auto frame = (grain < 8 ? grain : grain - 8) * 64 + i / 2;
            float expected = 0.0f;
            if (grain < 8 || mode == 1) {
                if (frame >= 3 && frame < 323) expected = static_cast<float>(((frame - 3) / 64 * (mode == 2 ? 2 : 1) + 1) * 1000) / 32768.0f;
            } else if (mode != 2 && mode != 5 && frame >= 3 && frame < (capacity == 3 ? 131u : 67u)) {
                expected = static_cast<float>(capacity == 3 ? ((frame - 3) / 64 + 4) * 1000 : 5000) / 32768.0f;
            }
            Require(samples[i] == expected);
        }
        auto expectedCallbacks = mode == 1 ? (grain < 4 ? 0u : grain < 12 ? 1u : 2u)
            : mode == 2 || mode == 5 ? (grain == 0 ? 0u : grain < 4 ? grain : grain < 8 ? 5u : 6u)
            : grain < 5 ? grain + 1 : grain < 8 ? 5u : 6u;
        if (capacity == 3 && grain >= 9) expectedCallbacks = 7;
        Require(stream.callbacks == expectedCallbacks);
    }
    Require(SubmitStream(stream, 0, 0) == static_cast<int>(0x804A8054u));
    option.max_waveform_blocks = 0;
    Require(sceNgs2RackQueryBufferSize(SCE_NGS2_RACK_ID_CUSTOM_SAMPLER, &common, &query) == static_cast<int>(0x804A8053u));
    Require(sceNgs2SystemDestroy(system, nullptr) == SCE_NGS2_OK);
    usedBuffers = firstBuffer;
}

int main() {
    for (const auto& test : Cases) TestSampler(test);
    for (std::uint32_t mode = 0; mode < 6; ++mode) TestStreaming(mode);
    TestStreaming(0, 3);
    return 0;
}
