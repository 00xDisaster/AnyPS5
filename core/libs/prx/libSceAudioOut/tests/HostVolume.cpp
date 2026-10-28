#include "prx/libSceAudioOut/include/HostVolume.hpp"

#include <cstdio>
#include <exception>
#include <stdexcept>

namespace {

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

void defaultState() {
    HostVolume state;
    require(state.Volume() == 100, "the volume starts at 100");
    require(!state.Muted(), "the host volume starts unmuted");
}

void stepsDown() {
    HostVolume state;
    for (int expected = 95; expected >= 0; expected -= 5) {
        require(state.VolumeDown(), "a step down from above 0 changes the volume");
        require(state.Volume() == expected, "each step down removes 5");
    }
    require(state.Volume() == 0, "twenty steps reach 0");
}

void stepsStopAtZero() {
    HostVolume state;
    for (int step = 0; step < 20; ++step) state.VolumeDown();
    require(!state.VolumeDown(), "a step down at 0 reports no change");
    require(state.Volume() == 0, "the volume never goes below 0");
}

void stepsUp() {
    HostVolume state;
    for (int step = 0; step < 20; ++step) state.VolumeDown();
    for (int expected = 5; expected <= 100; expected += 5) {
        require(state.VolumeUp(), "a step up from below 100 changes the volume");
        require(state.Volume() == expected, "each step up adds 5");
    }
}

void stepsStopAtMaximum() {
    HostVolume state;
    require(!state.VolumeUp(), "a step up at 100 reports no change");
    require(state.Volume() == 100, "the volume never goes above 100");
}

void muteKeepsVolume() {
    HostVolume state;
    state.VolumeDown();
    state.VolumeDown();
    require(state.Volume() == 90, "two steps down give 90");
    require(state.ToggleMute(), "muting changes the state");
    require(state.Muted() && state.Volume() == 90, "muting leaves the volume alone");
    require(state.ToggleMute(), "unmuting changes the state");
    require(!state.Muted() && state.Volume() == 90, "unmuting restores the volume that was set");
}

void muteAtZeroAndMaximum() {
    HostVolume state;
    state.ToggleMute();
    require(state.Volume() == 100, "muting at 100 keeps 100");
    state.ToggleMute();
    for (int step = 0; step < 20; ++step) state.VolumeDown();
    state.ToggleMute();
    require(state.Muted() && state.Volume() == 0, "muting at 0 keeps 0");
    state.ToggleMute();
    require(!state.Muted() && state.Volume() == 0, "unmuting at 0 leaves 0");
}

}

int main() try {
    defaultState();
    stepsDown();
    stepsStopAtZero();
    stepsUp();
    stepsStopAtMaximum();
    muteKeepsVolume();
    muteAtZeroAndMaximum();
    std::puts("host volume tests passed");
    return 0;
} catch (const std::exception& error) {
    std::fprintf(stderr, "%s\n", error.what());
    return 1;
}
