#pragma once

#include "SceTypes.hpp"

struct ApplicationInfo {
    std::byte data[152];
};

extern "C" {

int APS5_VABI sceApplicationGetAppInfoByAppId(std::uint32_t applicationId, ApplicationInfo* info);

}
