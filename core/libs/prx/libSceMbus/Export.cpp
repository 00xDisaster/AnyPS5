#include <cstdint>
#include <cstddef>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"

#include <string>

struct DeviceServiceThreadParam {
    std::uint64_t affinityMask;
    std::int32_t priority;
    std::uint32_t reserved;
};

struct DeviceServiceInitParam {
    std::uint32_t size;
    std::uint32_t reserved;
    const DeviceServiceThreadParam* threadParam;
};

struct DeviceServiceUsageFilter {
    std::uint16_t usagePage;
    std::uint16_t minimumUsage;
    std::uint16_t maximumUsage;
};

static_assert(sizeof(DeviceServiceThreadParam) == 16);
static_assert(offsetof(DeviceServiceThreadParam, priority) == 8);
static_assert(sizeof(DeviceServiceInitParam) == 16);
static_assert(offsetof(DeviceServiceInitParam, threadParam) == 8);
static_assert(sizeof(DeviceServiceUsageFilter) == 6);

extern "C" {

std::int32_t APS5_VABI sceDeviceServiceInitialize(std::int32_t mode, const DeviceServiceInitParam* param) {
    (void)mode;
    if (param != nullptr) {
        if (param->size != sizeof(DeviceServiceInitParam) || param->reserved != 0) return static_cast<std::int32_t>(0x809B0080u);
        if (param->threadParam != nullptr) {
            if (param->threadParam->reserved != 0) return static_cast<std::int32_t>(0x809B0080u);
            throw std::runtime_error("sceDeviceServiceInitialize: custom worker scheduling is not implemented");
        }
    }
    return 0;
}

std::int32_t APS5_VABI sceDeviceServiceTerminate() {
    return 0;
}

std::int32_t APS5_VABI sceDeviceServiceGetEventState(std::int32_t clear) {
    (void)clear;
    return 0;
}

std::int32_t APS5_VABI sceDeviceServiceQueryDeviceInfo_(std::int32_t deviceClass, const DeviceServiceUsageFilter* filter, std::int32_t arg2, void* infos, std::int32_t maxInfos, std::int32_t* count, std::int32_t* reserved, std::uint32_t infoSize) {
    (void)deviceClass;
    (void)filter;
    (void)arg2;
    (void)infos;
    (void)reserved;
    (void)infoSize;
    if (count == nullptr || maxInfos < 0) throw std::invalid_argument(std::string(__func__) + ": invalid argument");
    *count = 0;
    return 0;
}

std::int32_t APS5_VABI sceDeviceServiceGetGeneration(void) {
    return 0;
}

}
