#include "prx/libSceVideoOut/include/HostVolumeInput.hpp"

#include <cstdio>
#include <exception>
#include <stdexcept>

namespace {

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

void noRequestChangesNothing() {
    HostVolumeConfigure_nid_no_patch(nullptr, nullptr);
    HostVolume after;
    require(!ApplyHostVolumeRequests(Pad::InputRequests{}, &after), "an empty request set asks for nothing");
    require(HostVolumeGain_nid_no_patch() == HostVolume::UnityGain, "an empty request set leaves unity");
    require(after.Volume() == 100 && !after.Muted(), "the out parameter is untouched without a request");
}

void volumeRequestsMoveTheVolume() {
    HostVolumeConfigure_nid_no_patch(nullptr, nullptr);
    HostVolume after;
    Pad::InputRequests down;
    down.volumeDown = true;
    require(ApplyHostVolumeRequests(down, &after), "a volume down request is handled");
    require(after.Volume() == 95 && HostVolumeGain_nid_no_patch() == after.Gain(), "volume down lowers the shared volume by one step");
    Pad::InputRequests up;
    up.volumeUp = true;
    require(ApplyHostVolumeRequests(up, &after), "a volume up request is handled");
    require(after.Volume() == 100 && HostVolumeGain_nid_no_patch() == HostVolume::UnityGain, "volume up returns to exactly unity");
    require(ApplyHostVolumeRequests(up, &after), "a volume up request at the top is still handled");
    require(after.Volume() == 100, "volume up at the top stays at 100");
}

void muteRequestToggles() {
    HostVolumeConfigure_nid_no_patch(nullptr, nullptr);
    HostVolume after;
    Pad::InputRequests mute;
    mute.toggleMute = true;
    require(ApplyHostVolumeRequests(mute, &after) && after.Muted() && HostVolumeGain_nid_no_patch() == 0, "the first mute request silences the output");
    require(ApplyHostVolumeRequests(mute, &after) && !after.Muted() && HostVolumeGain_nid_no_patch() == HostVolume::UnityGain, "the second mute request restores the output");
}

void requestsRunInOrder() {
    HostVolumeConfigure_nid_no_patch(nullptr, nullptr);
    HostVolume after;
    Pad::InputRequests all;
    all.volumeUp = true;
    all.volumeDown = true;
    all.toggleMute = true;
    require(ApplyHostVolumeRequests(all, &after), "a combined request is handled");
    require(after.Volume() == 95 && after.Muted(), "up, then down, then mute leaves 95 muted");
}

void otherRequestsAreIgnored() {
    HostVolumeConfigure_nid_no_patch(nullptr, nullptr);
    HostVolume after;
    Pad::InputRequests other;
    other.toggleFullscreen = true;
    other.togglePerformanceOverlay = true;
    other.screenshot = true;
    other.toggleMouse = true;
    other.releaseMouse = true;
    require(!ApplyHostVolumeRequests(other, &after), "the other host requests are not volume requests");
    require(HostVolumeGain_nid_no_patch() == HostVolume::UnityGain, "the other host requests leave the volume alone");
}

}

int main() try {
    noRequestChangesNothing();
    volumeRequestsMoveTheVolume();
    muteRequestToggles();
    requestsRunInOrder();
    otherRequestsAreIgnored();
    HostVolumeConfigure_nid_no_patch(nullptr, nullptr);
    std::puts("host volume input tests passed");
    return 0;
} catch (const std::exception& error) {
    std::fprintf(stderr, "%s\n", error.what());
    return 1;
}
