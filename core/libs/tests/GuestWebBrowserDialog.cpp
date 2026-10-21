#include "prx/libc/include/general/VabiMacros.hpp"
#include "prx/libSceCommonDialog/CommonDialog.hpp"
#include <cstdint>
#include <cstdlib>

extern "C" {
int APS5_VABI sceWebBrowserDialogInitialize(void);
int APS5_VABI sceWebBrowserDialogTerminate(void);
int APS5_VABI sceWebBrowserDialogOpen(const void* param);
int APS5_VABI sceWebBrowserDialogOpenForPredeterminedContent(const void* param, const WebBrowserDialogAllowList* allowList);
int APS5_VABI sceWebBrowserDialogClose(void);
int APS5_VABI sceWebBrowserDialogGetStatus(void);
int APS5_VABI sceWebBrowserDialogUpdateStatus(void);
int APS5_VABI sceWebBrowserDialogGetResult(void* result);
}

namespace {

constexpr int COMMON_DIALOG_STATUS_NONE = 0;
constexpr int COMMON_DIALOG_STATUS_INITIALIZED = 1;
constexpr int COMMON_DIALOG_STATUS_FINISHED = 3;
constexpr int COMMON_DIALOG_RESULT_USER_CANCELED = 1;
constexpr int COMMON_DIALOG_ERROR_NOT_INITIALIZED = static_cast<int>(0x80B80003u);
constexpr int COMMON_DIALOG_ERROR_NOT_FINISHED = static_cast<int>(0x80B80005u);
constexpr int COMMON_DIALOG_ERROR_ARG_NULL = static_cast<int>(0x80B8000Du);

void Require(bool value) { if (!value) std::abort(); }

}

int main() {
    std::uint8_t param[64] = {};
    WebBrowserDialogAllowList allowList{};
    allowList.size = sizeof(allowList);
    allowList.addresses[0] = "test.invalid";
    std::int32_t result = 0x5a5a5a5a;

    Require(sceWebBrowserDialogTerminate() == COMMON_DIALOG_ERROR_NOT_INITIALIZED);
    Require(sceWebBrowserDialogGetStatus() == COMMON_DIALOG_STATUS_NONE);
    Require(sceWebBrowserDialogClose() == COMMON_DIALOG_ERROR_NOT_INITIALIZED);
    Require(sceWebBrowserDialogOpen(param) == COMMON_DIALOG_ERROR_NOT_INITIALIZED);
    Require(sceWebBrowserDialogGetResult(&result) == COMMON_DIALOG_ERROR_NOT_INITIALIZED);
    Require(result == 0x5a5a5a5a);

    Require(sceWebBrowserDialogInitialize() == 0);
    Require(sceWebBrowserDialogInitialize() == COMMON_DIALOG_ERROR_ALREADY_INITIALIZED);
    Require(sceWebBrowserDialogGetStatus() == COMMON_DIALOG_STATUS_INITIALIZED);
    Require(sceWebBrowserDialogUpdateStatus() == COMMON_DIALOG_STATUS_INITIALIZED);
    Require(sceWebBrowserDialogGetResult(&result) == COMMON_DIALOG_ERROR_NOT_FINISHED);
    Require(result == 0x5a5a5a5a);

    Require(sceWebBrowserDialogTerminate() == 0);
    Require(sceWebBrowserDialogTerminate() == COMMON_DIALOG_ERROR_NOT_INITIALIZED);
    Require(sceWebBrowserDialogGetStatus() == COMMON_DIALOG_STATUS_NONE);

    Require(sceWebBrowserDialogInitialize() == 0);
    Require(sceWebBrowserDialogOpen(nullptr) == COMMON_DIALOG_ERROR_ARG_NULL);
    Require(sceWebBrowserDialogOpen(param) == 0);
    Require(sceWebBrowserDialogGetStatus() == COMMON_DIALOG_STATUS_FINISHED);
    Require(sceWebBrowserDialogUpdateStatus() == COMMON_DIALOG_STATUS_FINISHED);

    Require(sceWebBrowserDialogGetResult(nullptr) == COMMON_DIALOG_ERROR_ARG_NULL);
    Require(sceWebBrowserDialogGetResult(&result) == 0);
    Require(result == COMMON_DIALOG_RESULT_USER_CANCELED);

    Require(sceWebBrowserDialogClose() == 0);
    Require(sceWebBrowserDialogTerminate() == 0);
    Require(sceWebBrowserDialogTerminate() == COMMON_DIALOG_ERROR_NOT_INITIALIZED);
    Require(sceWebBrowserDialogGetStatus() == COMMON_DIALOG_STATUS_NONE);

    Require(sceWebBrowserDialogOpenForPredeterminedContent(param, &allowList) == COMMON_DIALOG_ERROR_NOT_INITIALIZED);
    Require(sceWebBrowserDialogInitialize() == 0);
    Require(sceWebBrowserDialogOpenForPredeterminedContent(nullptr, &allowList) == COMMON_DIALOG_ERROR_ARG_NULL);
    Require(sceWebBrowserDialogOpenForPredeterminedContent(param, nullptr) == COMMON_DIALOG_ERROR_ARG_NULL);
    allowList.addresses[0] = nullptr;
    Require(sceWebBrowserDialogOpenForPredeterminedContent(param, &allowList) == static_cast<int>(0x80B8000Au));
    allowList.addresses[19] = "test.invalid";
    allowList.reserved[255] = 1;
    Require(sceWebBrowserDialogOpenForPredeterminedContent(param, &allowList) == static_cast<int>(0x80B8000Au));
    allowList.reserved[255] = 0;
    allowList.addresses[19] = "";
    Require(sceWebBrowserDialogOpenForPredeterminedContent(param, &allowList) == static_cast<int>(0x80B8000Au));
    char longAddress[257];
    std::fill(std::begin(longAddress), std::end(longAddress), 'a');
    longAddress[256] = '\0';
    allowList.addresses[19] = longAddress;
    Require(sceWebBrowserDialogOpenForPredeterminedContent(param, &allowList) == static_cast<int>(0x80B8000Au));
    longAddress[255] = '\0';
    Require(sceWebBrowserDialogOpenForPredeterminedContent(param, &allowList) == 0);
    Require(sceWebBrowserDialogGetStatus() == COMMON_DIALOG_STATUS_FINISHED);
    Require(sceWebBrowserDialogUpdateStatus() == COMMON_DIALOG_STATUS_FINISHED);

    result = 0x5a5a5a5a;
    Require(sceWebBrowserDialogGetResult(&result) == 0);
    Require(result == COMMON_DIALOG_RESULT_USER_CANCELED);

    Require(sceWebBrowserDialogClose() == 0);
    Require(sceWebBrowserDialogTerminate() == 0);
    Require(sceWebBrowserDialogGetStatus() == COMMON_DIALOG_STATUS_NONE);

    Require(sceWebBrowserDialogInitialize() == 0);
    Require(sceWebBrowserDialogGetStatus() == COMMON_DIALOG_STATUS_INITIALIZED);
    Require(sceWebBrowserDialogClose() == 0);
    Require(sceWebBrowserDialogGetStatus() == COMMON_DIALOG_STATUS_FINISHED);
    result = 0x5a5a5a5a;
    Require(sceWebBrowserDialogGetResult(&result) == 0);
    Require(result == COMMON_DIALOG_RESULT_USER_CANCELED);
    Require(sceWebBrowserDialogTerminate() == 0);
    Require(sceWebBrowserDialogGetStatus() == COMMON_DIALOG_STATUS_NONE);

    return 0;
}
