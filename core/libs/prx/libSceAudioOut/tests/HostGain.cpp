#include "prx/libSceAudioOut/include/HostGain.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <vector>

namespace {

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

std::vector<std::int16_t> constantS16(std::size_t frames, unsigned channels, std::int16_t value) {
    return std::vector<std::int16_t>(frames * channels, value);
}

void s16BypassAtUnity() {
    for (const unsigned channels : {1u, 2u, 8u}) {
        std::vector<std::int16_t> samples(256 * channels);
        for (std::size_t index = 0; index < samples.size(); ++index) samples[index] = static_cast<std::int16_t>(static_cast<int>(index * 7919u % 65536u) - 32768);
        const auto before = samples;
        HostGainRamp ramp;
        require(ramp.Bypass(HostGainRamp::Unity), "a ramp at unity with a unity target bypasses");
        ramp.Apply(samples.data(), 256, channels, HostGainRamp::Unity);
        require(std::memcmp(before.data(), samples.data(), samples.size() * sizeof(std::int16_t)) == 0, "unity leaves 16-bit samples bit-identical");
        require(ramp.Current() == HostGainRamp::Unity, "bypass keeps the gain at unity");
    }
}

void s16FirstSampleOfRamp() {
    HostGainRamp ramp;
    require(ramp.Step() == 136, "a 480 frame ramp steps 136 per frame");
    auto samples = constantS16(1, 1, 10000);
    ramp.Apply(samples.data(), 1, 1, 32768);
    require(ramp.Current() == 65536 - 136, "the first frame moves one step");
    require(samples[0] == 9979, "the first frame is scaled by the first step");
}

void s16RampReachesTargetExactly() {
    HostGainRamp ramp;
    auto samples = constantS16(2000, 1, 10000);
    ramp.Apply(samples.data(), 2000, 1, 32768);
    require(ramp.Current() == 32768, "the ramp lands exactly on the target");
    std::size_t firstAtTarget = 0;
    while (firstAtTarget < samples.size() && samples[firstAtTarget] != 5000) ++firstAtTarget;
    require(firstAtTarget == 240, "the ramp takes the 241 frames its step needs");
    for (std::size_t index = firstAtTarget; index < samples.size(); ++index) require(samples[index] == 5000, "samples stay at the target after the ramp");
}

void s16RampHasNoDiscontinuity() {
    HostGainRamp ramp;
    auto samples = constantS16(2000, 1, 32000);
    ramp.Apply(samples.data(), 2000, 1, 0);
    int largest = 0;
    int previous = 32000;
    for (const auto sample : samples) {
        require(sample <= previous, "a ramp down never rises");
        largest = std::max(largest, previous - sample);
        previous = sample;
    }
    require(largest <= 67, "no step of a full-scale ramp exceeds its per-frame slope");
    require(samples.back() == 0, "the ramp ends in silence");
}

void s16RampContinuesAcrossBuffers() {
    auto whole = constantS16(3000, 2, 20000);
    HostGainRamp wholeRamp;
    wholeRamp.Apply(whole.data(), 3000, 2, 16384);
    auto split = constantS16(3000, 2, 20000);
    HostGainRamp splitRamp;
    std::size_t done = 0;
    for (const std::size_t chunk : {1u, 7u, 100u, 333u, 1u, 900u, 1658u}) {
        splitRamp.Apply(split.data() + done * 2, chunk, 2, 16384);
        done += chunk;
    }
    require(done == 3000, "the chunks cover the buffer");
    require(std::memcmp(whole.data(), split.data(), whole.size() * sizeof(std::int16_t)) == 0, "a ramp split across buffers equals the same ramp in one buffer");
    require(wholeRamp.Current() == splitRamp.Current(), "both ramps end at the same gain");
}

void s16SaturatesAboveUnity() {
    HostGainRamp ramp(1);
    auto samples = constantS16(4, 1, 30000);
    samples[1] = -30000;
    samples[2] = 8000;
    ramp.Apply(samples.data(), 4, 1, 4 * HostGainRamp::Unity);
    require(samples[0] == 32767, "a positive overshoot saturates at 32767");
    require(samples[1] == -32768, "a negative overshoot saturates at -32768");
    require(samples[2] == 32000, "a sample inside the range is scaled");
}

void s16ChannelsShareTheFrameGain() {
    for (const unsigned channels : {1u, 2u, 8u}) {
        HostGainRamp ramp;
        auto samples = constantS16(600, channels, 12345);
        ramp.Apply(samples.data(), 600, channels, 0);
        for (std::size_t frame = 0; frame < 600; ++frame) {
            for (unsigned channel = 1; channel < channels; ++channel) require(samples[frame * channels + channel] == samples[frame * channels], "every channel of a frame gets the same gain");
        }
        require(samples[599 * channels] == 0, "the last frame is silent");
    }
}

void s16SilentTargetSilencesAfterRamp() {
    HostGainRamp ramp(1);
    auto samples = constantS16(8, 2, -32768);
    ramp.Apply(samples.data(), 8, 2, 0);
    for (const auto sample : samples) require(sample == 0, "a zero gain writes zeros");
    require(!ramp.Bypass(0), "a silent target is never bypassed");
    auto again = constantS16(8, 2, 1000);
    ramp.Apply(again.data(), 8, 2, 0);
    for (const auto sample : again) require(sample == 0, "a settled silent gain stays silent");
}

void s16ReturnToUnityBypassesAgain() {
    HostGainRamp ramp;
    auto samples = constantS16(2000, 2, 9000);
    ramp.Apply(samples.data(), 2000, 2, 0);
    auto back = constantS16(2000, 2, 9000);
    ramp.Apply(back.data(), 2000, 2, HostGainRamp::Unity);
    require(ramp.Current() == HostGainRamp::Unity, "the ramp returns to exactly unity");
    require(ramp.Bypass(HostGainRamp::Unity), "once back at unity the stream bypasses again");
    auto steady = constantS16(64, 2, 9000);
    const auto before = steady;
    ramp.Apply(steady.data(), 64, 2, HostGainRamp::Unity);
    require(steady == before, "samples after the return are bit-identical");
}

}

int main() try {
    s16BypassAtUnity();
    s16FirstSampleOfRamp();
    s16RampReachesTargetExactly();
    s16RampHasNoDiscontinuity();
    s16RampContinuesAcrossBuffers();
    s16SaturatesAboveUnity();
    s16ChannelsShareTheFrameGain();
    s16SilentTargetSilencesAfterRamp();
    s16ReturnToUnityBypassesAgain();
    std::puts("host gain tests passed");
    return 0;
} catch (const std::exception& error) {
    std::fprintf(stderr, "%s\n", error.what());
    return 1;
}
