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

}

int main() try {
    defaultState();
    stepsDown();
    stepsStopAtZero();
    stepsUp();
    stepsStopAtMaximum();
    std::puts("host volume tests passed");
    return 0;
} catch (const std::exception& error) {
    std::fprintf(stderr, "%s\n", error.what());
    return 1;
}
