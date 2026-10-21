#include "SceTypes.hpp"
#include "prx/libSceCommonDialog/CommonDialog.hpp"

#include <cstdint>
#include <cstdlib>
#include <cstring>

extern "C" {
int APS5_VABI sceNpCommerceDialogInitialize();
int APS5_VABI sceNpCommerceDialogOpen(const NpCommerceDialogParam* param);
int APS5_VABI sceNpCommerceDialogOpen2(const NpCommerceDialogParam2* param);
int APS5_VABI sceNpCommerceDialogUpdateStatus(void);
int APS5_VABI sceNpCommerceDialogGetStatus(void);
int APS5_VABI sceNpCommerceDialogGetResult(void* result);
int APS5_VABI sceNpCommerceDialogClose(void);
int APS5_VABI sceNpCommerceDialogTerminate();
}

namespace {

void Require(bool value) { if (!value) std::abort(); }

constexpr int statusNone = 0;
constexpr int statusInitialized = 1;
constexpr int statusFinished = 3;
constexpr std::int32_t resultUserCanceled = 1;

void RequireCanceledResult() {
    std::uint8_t result[0x20];
    std::memset(result, 0x33, sizeof(result));
    Require(sceNpCommerceDialogGetResult(result) == 0);
    std::int32_t value = 0;
    std::memcpy(&value, result, sizeof(value));
    Require(value == resultUserCanceled);
    Require(result[4] == 0);
    Require(result[5] == 0x33);
}

}

int main() {
    NpCommerceDialogParam2 param2{};
    param2.base.size = sizeof(param2.base);
    param2.base.magic = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&param2)) - 1060003575u;
    param2.size = sizeof(param2);
    param2.userId = 1;
    Require(sceNpCommerceDialogGetStatus() == statusNone);
    Require(sceNpCommerceDialogOpen(nullptr) == static_cast<int>(0x80B8000Du));
    Require(sceNpCommerceDialogOpen2(nullptr) == static_cast<int>(0x80B8000Du));
    Require(sceNpCommerceDialogOpen2(&param2) == static_cast<int>(0x80B80003u));
    param2.base.magic = 0;
    Require(sceNpCommerceDialogOpen2(&param2) == static_cast<int>(0x80B8000Au));
    param2.base.magic = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&param2)) - 1060003575u;
    param2.base.reserved[35] = 1;
    Require(sceNpCommerceDialogOpen2(&param2) == static_cast<int>(0x80B8000Au));
    param2.base.reserved[35] = 0;
    Require(sceNpCommerceDialogTerminate() == static_cast<int>(0x80B80003u));
    Require(sceNpCommerceDialogClose() == static_cast<int>(0x80B80003u));
    Require(sceNpCommerceDialogGetResult(nullptr) == static_cast<int>(0x80B8000Du));
    std::uint8_t result[48]{};
    Require(sceNpCommerceDialogGetResult(result) == static_cast<int>(0x80B80003u));
    Require(sceNpCommerceDialogInitialize() == 0);
    Require(sceNpCommerceDialogClose() == static_cast<int>(0x80B8000Bu));
    Require(sceNpCommerceDialogGetResult(result) == static_cast<int>(0x80B80005u));
    Require(sceNpCommerceDialogGetResult(nullptr) == static_cast<int>(0x80B8000Du));
    Require(sceNpCommerceDialogGetStatus() == statusInitialized);

    Require(sceNpCommerceDialogOpen2(&param2) == 0);
    Require(sceNpCommerceDialogUpdateStatus() == statusFinished);
    Require(sceNpCommerceDialogGetStatus() == statusFinished);
    RequireCanceledResult();

    Require(sceNpCommerceDialogClose() == static_cast<int>(0x80B8000Bu));
    Require(sceNpCommerceDialogGetStatus() == statusFinished);
    Require(sceNpCommerceDialogTerminate() == 0);
    Require(sceNpCommerceDialogGetStatus() == statusNone);

    NpCommerceDialogParam param{};
    param.base.size = sizeof(param.base);
    param.base.magic = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&param)) - 1060003575u;
    param.size = sizeof(param);
    param.userId = 1;
    Require(sceNpCommerceDialogInitialize() == 0);
    param.base.size = 0;
    Require(sceNpCommerceDialogOpen(&param) == static_cast<int>(0x80B8000Au));
    param.base.size = sizeof(param.base);
    Require(sceNpCommerceDialogOpen(&param) == 0);
    Require(sceNpCommerceDialogOpen2(&param2) == 0);
    Require(sceNpCommerceDialogUpdateStatus() == statusFinished);
    RequireCanceledResult();
    Require(sceNpCommerceDialogTerminate() == 0);
    Require(sceNpCommerceDialogGetStatus() == statusNone);
    return 0;
}
