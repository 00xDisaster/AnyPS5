#include <cstdint>
#include <cstddef>
#include <cstring>
#include "SceTypes.hpp"
#include "prx/libSceCommonDialog/CommonDialog.hpp"
#include "prx/libc/include/General.hpp"

namespace {

constexpr int COMMON_DIALOG_STATUS_NONE = 0;
constexpr int COMMON_DIALOG_STATUS_INITIALIZED = 1;
constexpr int COMMON_DIALOG_STATUS_RUNNING = 2;
constexpr int COMMON_DIALOG_STATUS_FINISHED = 3;
constexpr int COMMON_DIALOG_RESULT_USER_CANCELED = 1;
constexpr int COMMON_DIALOG_ERROR_NOT_INITIALIZED = static_cast<int>(0x80B80003u);
constexpr int COMMON_DIALOG_ERROR_NOT_FINISHED = static_cast<int>(0x80B80005u);
constexpr int COMMON_DIALOG_ERROR_ARG_NULL = static_cast<int>(0x80B8000Du);

constexpr int COMMON_DIALOG_ERROR_INVALID_ARGUMENT = static_cast<int>(0x80B8000Au);
constexpr int COMMON_DIALOG_ERROR_NOT_RUNNING = static_cast<int>(0x80B8000Bu);

int g_status = COMMON_DIALOG_STATUS_NONE;

}

extern "C" {

// The store dialog has nothing to show: every open finishes at once as cancelled by the user.
int APS5_VABI sceNpCommerceDialogInitialize() {
 if (g_status != COMMON_DIALOG_STATUS_NONE) return COMMON_DIALOG_ERROR_ALREADY_INITIALIZED;
 g_status = COMMON_DIALOG_STATUS_INITIALIZED;
 return 0;
}

int APS5_VABI sceNpCommerceDialogOpen(const NpCommerceDialogParam* param) {
 if (param == nullptr) return COMMON_DIALOG_ERROR_ARG_NULL;
 if (!ValidateCommonDialogBaseParam(param->base)) return COMMON_DIALOG_ERROR_INVALID_ARGUMENT;
 if (g_status == COMMON_DIALOG_STATUS_NONE) return COMMON_DIALOG_ERROR_NOT_INITIALIZED;
 g_status = COMMON_DIALOG_STATUS_FINISHED;
 return 0;
}

int APS5_VABI sceNpCommerceDialogUpdateStatus(void) {
 return g_status;
}

int APS5_VABI sceNpCommerceDialogGetStatus(void) {
 return g_status;
}

int APS5_VABI sceNpCommerceDialogGetResult(void* result) {
 if (result == nullptr) return COMMON_DIALOG_ERROR_ARG_NULL;
 if (g_status == COMMON_DIALOG_STATUS_NONE) return COMMON_DIALOG_ERROR_NOT_INITIALIZED;
 if (g_status != COMMON_DIALOG_STATUS_FINISHED) return COMMON_DIALOG_ERROR_NOT_FINISHED;
 // SceNpCommerceDialogResult: int32 result, bool authorized, then reserved bytes.
 std::int32_t value = COMMON_DIALOG_RESULT_USER_CANCELED;
 std::memcpy(result, &value, sizeof(value));
 std::uint8_t authorized = 0;
 std::memcpy(static_cast<std::uint8_t*>(result) + sizeof(value), &authorized, sizeof(authorized));
 return 0;
}

int APS5_VABI sceNpCommerceDialogClose(void) {
 if (g_status == COMMON_DIALOG_STATUS_NONE) return COMMON_DIALOG_ERROR_NOT_INITIALIZED;
 if (g_status != COMMON_DIALOG_STATUS_RUNNING) return COMMON_DIALOG_ERROR_NOT_RUNNING;
 g_status = COMMON_DIALOG_STATUS_FINISHED;
 return 0;
}

int APS5_VABI sceNpCommerceDialogTerminate() {
 if (g_status == COMMON_DIALOG_STATUS_NONE) return COMMON_DIALOG_ERROR_NOT_INITIALIZED;
 g_status = COMMON_DIALOG_STATUS_NONE;
 return 0;
}

int APS5_VABI sceNpCommerceHidePsStoreIcon(void) {
 return 0;
}

int APS5_VABI sceNpCommerceShowPsStoreIcon(int pos) {
 (void)pos;
 return 0;
}

int APS5_VABI sceNpCommerceSetPsStoreIconLayout(int layout) {
 (void)layout;
 return 0;
}

int APS5_VABI sceNpCommerceDialogOpen2(const NpCommerceDialogParam2* param) {
 if (param == nullptr) return COMMON_DIALOG_ERROR_ARG_NULL;
 if (!ValidateCommonDialogBaseParam(param->base)) return COMMON_DIALOG_ERROR_INVALID_ARGUMENT;
 if (g_status == COMMON_DIALOG_STATUS_NONE) return COMMON_DIALOG_ERROR_NOT_INITIALIZED;
 g_status = COMMON_DIALOG_STATUS_FINISHED;
 return 0;
}

int APS5_VABI sceNpCommerceDialogInitializeInternal() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

}
