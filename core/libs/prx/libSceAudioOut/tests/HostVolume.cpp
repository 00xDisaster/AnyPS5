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

void stepsEndMute() {
    HostVolume state;
    state.VolumeDown();
    state.ToggleMute();
    require(state.VolumeUp(), "a step up while muted changes the state");
    require(!state.Muted() && state.Volume() == 100, "a step up while muted unmutes and moves the volume");
    state.ToggleMute();
    require(state.VolumeDown(), "a step down while muted changes the state");
    require(!state.Muted() && state.Volume() == 95, "a step down while muted unmutes and moves the volume");
}

void stepsAtLimitsStillEndMute() {
    HostVolume top;
    top.ToggleMute();
    require(top.VolumeUp(), "a step up at 100 while muted reports the unmute");
    require(!top.Muted() && top.Volume() == 100, "a step up at 100 while muted only unmutes");
    require(!top.VolumeUp(), "a second step up at 100 reports no change");
    HostVolume bottom;
    for (int step = 0; step < 20; ++step) bottom.VolumeDown();
    bottom.ToggleMute();
    require(bottom.VolumeDown(), "a step down at 0 while muted reports the unmute");
    require(!bottom.Muted() && bottom.Volume() == 0, "a step down at 0 while muted only unmutes");
}

void setVolumeDirectly() {
    HostVolume state;
    require(state.SetVolume(35), "a new volume reports a change");
    require(state.Volume() == 35, "the volume takes the value set");
    require(!state.SetVolume(35), "the same volume reports no change");
    require(state.SetVolume(0) && state.Volume() == 0, "0 is accepted");
    require(state.SetVolume(100) && state.Volume() == 100, "100 is accepted");
    state.ToggleMute();
    state.SetVolume(40);
    require(state.Muted(), "setting the volume leaves the mute alone");
}

void setVolumeRefusesOutOfRange() {
    HostVolume state;
    state.SetVolume(60);
    for (const int value : {-1, 101, 1000, -100}) {
        bool refused = false;
        try {
            state.SetVolume(value);
        } catch (const std::invalid_argument&) {
            refused = true;
        }
        require(refused, "a volume outside 0 to 100 is refused");
        require(state.Volume() == 60, "a refused volume leaves the state as it was");
    }
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
    stepsEndMute();
    stepsAtLimitsStillEndMute();
    setVolumeDirectly();
    setVolumeRefusesOutOfRange();
    std::puts("host volume tests passed");
    return 0;
} catch (const std::exception& error) {
    std::fprintf(stderr, "%s\n", error.what());
    return 1;
}
