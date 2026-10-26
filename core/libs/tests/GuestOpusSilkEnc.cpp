#include "prx/libSceOpusSilkEnc/include/Api.hpp"
#include "prx/libc/include/general/VabiMacros.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <source_location>
#include <stdexcept>
#include <opus.h>

namespace {

void Require(bool value, const std::source_location& location = std::source_location::current()) {
    if (!value) {
        std::fprintf(stderr, "Require failed at %s:%d\n", location.file_name(), static_cast<int>(location.line()));
        std::abort();
    }
}

template <typename TFunction>
void RequireThrows(TFunction function) {
    try { function(); } catch (const std::runtime_error&) { return; }
    std::abort();
}

template <std::size_t Size>
OpusSilkEncoder* ToState(std::array<std::uint8_t, Size>& block) {
    return reinterpret_cast<OpusSilkEncoder*>(block.data());
}

void FillSine(std::int16_t* samples, int frameSize, int channels, int rate, int position) {
    for (int index = 0; index < frameSize; ++index) {
        const int value = static_cast<int>(3000 * std::sin(2 * 3.14159265358979323846 * 2000 * (position + index) / rate));
        for (int channel = 0; channel < channels; ++channel) samples[index * channels + channel] = static_cast<std::int16_t>(value);
    }
}

void RequireTone(const std::int16_t* pcm, int samples, int channels, int rate) {
    for (int channel = 0; channel < channels; ++channel) {
        int peak = 0, crossings = 0;
        for (int sample = 0; sample < samples; ++sample) {
            const int value = pcm[sample * channels + channel];
            peak = std::max(peak, std::abs(value));
            if (sample > 0) {
                const int previous = pcm[(sample - 1) * channels + channel];
                if ((previous <= 0 && value > 0) || (previous >= 0 && value < 0)) ++crossings;
            }
        }
        Require(peak > 30 && peak < 32000);
        const int expected = 2 * samples * 2000 / rate;
        Require(crossings >= expected - expected / 3 && crossings <= expected + expected / 3);
    }
}

} // namespace

int main() {
    constexpr std::uint32_t errorBase = 0x81770000u;
    constexpr int callerApplication = OPUS_APPLICATION_VOIP;
    constexpr int codecApplication = OPUS_APPLICATION_RESTRICTED_SILK;
    Require(sceOpusSilkEncInitialize(nullptr) == static_cast<int>(errorBase + 0x18));
    Require(sceOpusSilkEncDestroy(nullptr) == static_cast<int>(errorBase + 0x01));
    std::uint32_t context = 0;
    Require(sceOpusSilkEncInitialize(&context) == 0);
    RequireThrows([&] { sceOpusSilkEncInitialize(&context); });
    RequireThrows([&] { sceOpusSilkEncTerminate(nullptr); });

    std::array<std::uint8_t, 65536> invalidState{};
    std::array<std::int16_t, 1920> samples{};
    samples.fill(12345);
    std::array<float, 1920> floatSamples{};
    std::uint8_t packet[1024];
    for (auto& byte : packet) byte = 0x5A;
    Require(sceOpusSilkEncCreate(nullptr, 48000, 2, callerApplication) == static_cast<int>(errorBase + 0x01));
    Require(sceOpusSilkEncCreate(ToState(invalidState), 48000, 0, callerApplication) == static_cast<int>(errorBase + 0x0C));
    Require(sceOpusSilkEncCreate(ToState(invalidState), 0, 0, callerApplication) == static_cast<int>(errorBase + 0x0F));
    Require(sceOpusSilkEncCreate(ToState(invalidState), 22050, 2, callerApplication) == static_cast<int>(errorBase + 0x0F));
    Require(sceOpusSilkEncCreate(ToState(invalidState), 48000, 2, OPUS_APPLICATION_RESTRICTED_SILK) == static_cast<int>(errorBase + 0x10));
    Require(sceOpusSilkEncCreateEx(nullptr, ToState(invalidState), 0, 0, callerApplication) == static_cast<int>(errorBase + 0x01));
    Require(sceOpusSilkEncCreateEx(&context, ToState(invalidState), 0, 0, callerApplication) == static_cast<int>(errorBase + 0x0F));
    Require(sceOpusSilkEncCreateEx(&context, ToState(invalidState), 48000, 0, callerApplication) == static_cast<int>(errorBase + 0x0C));
    Require(sceOpusSilkEncCreateEx(&context, ToState(invalidState), 48000, 2, OPUS_APPLICATION_RESTRICTED_SILK) == static_cast<int>(errorBase + 0x10));
    Require(sceOpusSilkEncEncode(nullptr, nullptr, -1, nullptr, -1) == static_cast<int>(errorBase));
    Require(sceOpusSilkEncEncode(ToState(invalidState), nullptr, 960, nullptr, -1) == static_cast<int>(errorBase + 6));
    Require(sceOpusSilkEncEncode(ToState(invalidState), samples.data(), -1, packet, -1) == static_cast<int>(errorBase + 7));
    Require(sceOpusSilkEncEncode(ToState(invalidState), samples.data(), 960, nullptr, -1) == static_cast<int>(errorBase + 8));
    Require(sceOpusSilkEncEncode(ToState(invalidState), samples.data(), 960, packet, -1) == static_cast<int>(errorBase + 9));
    Require(sceOpusSilkEncEncode(ToState(invalidState), samples.data(), 960, packet, 8) == static_cast<int>(errorBase + 1));
    Require(packet[0] == 0x5A && packet[1] == 0x5A);
    Require(sceOpusSilkEncEncodeFloat(nullptr, nullptr, -1, nullptr, -1) == static_cast<int>(errorBase));
    Require(sceOpusSilkEncEncodeFloat(ToState(invalidState), nullptr, 960, packet, 8) == static_cast<int>(errorBase + 6));
    Require(sceOpusSilkEncEncodeFloat(ToState(invalidState), floatSamples.data(), -1, packet, 8) == static_cast<int>(errorBase + 7));

    Require(sceOpusSilkEncGetSize(1) == 752 && sceOpusSilkEncGetSize(2) == 752);
    Require(sceOpusSilkEncGetSize(3) == static_cast<int>(errorBase + 0x0C));

    for (int rate : {8000, 12000, 16000, 24000, 48000}) {
        for (int channels : {1, 2}) {
            std::array<std::uint8_t, 65536> state{};
            Require(sceOpusSilkEncCreateEx(&context, ToState(state), rate, channels, callerApplication) == 0);
            int opusError = OPUS_OK;
            auto* reference = opus_encoder_create(rate, channels, codecApplication, &opusError);
            Require(reference && opusError == OPUS_OK);
            const int frameSize = rate / 50;
            for (int frame = 0; frame < 4; ++frame) {
                FillSine(samples.data(), frameSize, channels, rate, frame * frameSize);
                const int bytes = sceOpusSilkEncEncode(ToState(state), samples.data(), frameSize, packet, sizeof(packet));
                Require(bytes > 2 && static_cast<int>(packet[0]) >> 3 < 12);
                std::array<std::uint8_t, 1024> expected{};
                const int expectedBytes = opus_encode(reference, samples.data(), frameSize, expected.data(), sizeof(expected));
                Require(expectedBytes == bytes && std::equal(packet, packet + bytes, expected.begin()));
            }
            int decodeError = OPUS_OK;
            auto* decoder = opus_decoder_create(rate, channels, &decodeError);
            Require(decoder && decodeError == OPUS_OK);
            std::array<std::int16_t, 1922> pcm{};
            for (int frame = 0; frame < 4; ++frame) {
                FillSine(samples.data(), frameSize, channels, rate, frame * frameSize);
                const int bytes = sceOpusSilkEncEncode(ToState(state), samples.data(), frameSize, packet, sizeof(packet));
                pcm.fill(12345);
                Require(opus_decode(decoder, packet, bytes, pcm.data() + 1, frameSize, 0) == frameSize);
                Require(pcm.front() == 12345 && pcm.back() == 12345);
                RequireTone(pcm.data() + 1, frameSize, channels, rate);
            }
            opus_decoder_destroy(decoder);
            opus_encoder_destroy(reference);
            Require(sceOpusSilkEncDestroy(ToState(state)) == 0);
        }
    }
    Require(sceOpusSilkEncDestroy(ToState(invalidState)) == static_cast<int>(errorBase + 0x15));

    std::array<std::uint8_t, 65536> floatState{};
    Require(sceOpusSilkEncCreateEx(&context, ToState(floatState), 48000, 1, callerApplication) == 0);
    for (int index = 0; index < 960; ++index)
        floatSamples[static_cast<std::size_t>(index)] = static_cast<float>(0.2 * std::sin(2 * 3.14159265358979323846 * 2000 * index / 48000.0));
    const int floatResult = sceOpusSilkEncEncodeFloat(ToState(floatState), floatSamples.data(), 960, packet, static_cast<int>(sizeof(packet)));
    Require(floatResult > 2 && static_cast<int>(packet[0]) >> 3 < 12);
    int floatOpusError = OPUS_OK;
    auto* floatReference = opus_encoder_create(48000, 1, codecApplication, &floatOpusError);
    Require(floatReference && floatOpusError == OPUS_OK);
    std::array<std::uint8_t, 1024> floatExpected{};
    const int floatExpectedBytes = opus_encode_float(floatReference, floatSamples.data(), 960, floatExpected.data(), sizeof(floatExpected));
    Require(floatExpectedBytes == floatResult && std::equal(packet, packet + floatResult, floatExpected.begin()));
    opus_encoder_destroy(floatReference);
    Require(sceOpusSilkEncDestroy(ToState(floatState)) == 0);

    std::array<std::uint8_t, 65536> liveState{};
    Require(sceOpusSilkEncCreateEx(&context, ToState(liveState), 48000, 2, callerApplication) == 0);
    Require(sceOpusSilkEncDestroy(ToState(liveState)) == 0);
    Require(sceOpusSilkEncTerminate(&context) == 0);
    RequireThrows([&] { sceOpusSilkEncTerminate(&context); });
    RequireThrows([&] { sceOpusSilkEncCreateEx(&context, ToState(liveState), 48000, 2, callerApplication); });
    Require(sceOpusSilkEncCreate(ToState(liveState), 48000, 2, callerApplication) == 0);
    Require(sceOpusSilkEncDestroy(ToState(liveState)) == 0);
}
