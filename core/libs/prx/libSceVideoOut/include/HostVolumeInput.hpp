#ifndef CORE_LIBS_PRX_LIBSCEVIDEOOUT_INCLUDE_HOSTVOLUMEINPUT_HPP
#define CORE_LIBS_PRX_LIBSCEVIDEOOUT_INCLUDE_HOSTVOLUMEINPUT_HPP

#include "prx/libSceAudioOut/include/HostVolumeControl.hpp"
#include "prx/libScePad/include/KeyboardMouse.hpp"

inline bool ApplyHostVolumeRequests(const Pad::InputRequests& requests, HostVolume* after) {
    bool requested = false;
    if (requests.volumeUp) {
        HostVolumeApply_nid_no_patch(HostVolumeCommand::VolumeUp, after);
        requested = true;
    }
    if (requests.volumeDown) {
        HostVolumeApply_nid_no_patch(HostVolumeCommand::VolumeDown, after);
        requested = true;
    }
    if (requests.toggleMute) {
        HostVolumeApply_nid_no_patch(HostVolumeCommand::ToggleMute, after);
        requested = true;
    }
    return requested;
}

#endif
