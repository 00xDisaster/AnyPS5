#ifndef CORE_LIBS_PRX_LIBSCEAUDIOOUT_INCLUDE_HOSTVOLUMESETTINGS_HPP
#define CORE_LIBS_PRX_LIBSCEAUDIOOUT_INCLUDE_HOSTVOLUMESETTINGS_HPP

#include <charconv>
#include <stdexcept>
#include <string>
#include <string_view>

#include "prx/libSceAudioOut/include/HostVolume.hpp"

inline bool ParseHostVolumeFlag(std::string_view name, std::string_view value) {
    if (value == "0") return false;
    if (value == "1") return true;
    throw std::runtime_error(std::string(name) + " must be 0 or 1, not '" + std::string(value) + "'");
}

inline int ParseHostVolumeLevel(std::string_view name, std::string_view value) {
    int parsed = 0;
    const auto end = value.data() + value.size();
    const auto result = std::from_chars(value.data(), end, parsed);
    if (value.empty() || result.ec != std::errc{} || result.ptr != end || parsed < 0 || parsed > HostVolume::MaxVolume) {
        throw std::runtime_error(std::string(name) + " must be a whole number from 0 to " + std::to_string(HostVolume::MaxVolume) + ", not '" + std::string(value) + "'");
    }
    return parsed;
}

#endif
