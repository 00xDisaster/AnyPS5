#ifndef CORE_LIBS_PRX_LIBSCEAUDIOOUT_INCLUDE_HOSTVOLUMECONTROL_HPP
#define CORE_LIBS_PRX_LIBSCEAUDIOOUT_INCLUDE_HOSTVOLUMECONTROL_HPP

#include <cstdint>

#include "prx/libSceAudioOut/include/HostVolume.hpp"

enum class HostVolumeCommand {
    VolumeUp,
    VolumeDown,
    ToggleMute
};

bool HostVolumeApply_nid_no_patch(HostVolumeCommand command, HostVolume* after);
bool HostVolumeFocus_nid_no_patch(bool focused, HostVolume* after);
void HostVolumeConfigure_nid_no_patch(const char* initialVolume, const char* muteOnFocusLoss);
void HostVolumeConfigureFromEnvironment_nid_no_patch();
std::uint32_t HostVolumeGain_nid_no_patch();

#endif
