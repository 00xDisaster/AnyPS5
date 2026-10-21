#include <atomic>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"

namespace {
std::atomic<int> g_status{0};
std::atomic<bool> g_open{false};

constexpr int COMMON_DIALOG_STATUS_NONE = 0;
constexpr int COMMON_DIALOG_STATUS_RUNNING = 2;
constexpr int COMMON_DIALOG_STATUS_FINISHED = 3;
constexpr int COMMON_DIALOG_RESULT_USER_CANCELED = 1;
constexpr int COMMON_DIALOG_ERROR_NOT_INITIALIZED = static_cast<int>(0x80B80003u);
constexpr int COMMON_DIALOG_ERROR_ALREADY_INITIALIZED = static_cast<int>(0x80B80004u);
constexpr int COMMON_DIALOG_ERROR_NOT_FINISHED = static_cast<int>(0x80B80005u);
constexpr int COMMON_DIALOG_ERROR_INVALID_STATE = static_cast<int>(0x80B80006u);
constexpr int COMMON_DIALOG_ERROR_BUSY = static_cast<int>(0x80B80007u);
constexpr int COMMON_DIALOG_ERROR_PARAM_INVALID = static_cast<int>(0x80B8000Au);
constexpr int COMMON_DIALOG_ERROR_ARG_NULL = static_cast<int>(0x80B8000Du);

struct ResetCookieParam {
    std::uint64_t size;
    std::uint8_t reserved[256];
};

struct SetCookieParam {
    std::uint64_t size;
    const char* url;
    const char* cookie;
    std::uint8_t reserved[256];
};

static_assert(sizeof(ResetCookieParam) == 0x108 && sizeof(SetCookieParam) == 0x118);

bool ReservedClear(const std::uint8_t (&reserved)[256]) {
    for (const auto byte : reserved)
        if (byte != 0) return false;
    return true;
}

bool LengthWithin(const char* text, std::size_t limit) {
    if (!text) return false;
    const auto length = strnlen(text, limit);
    return length != 0 && length < limit;
}
}

extern "C" {

int APS5_VABI sceWebBrowserDialogInitialize(void) {
    int expected = COMMON_DIALOG_STATUS_NONE;
    if (!g_status.compare_exchange_strong(expected, 1)) return COMMON_DIALOG_ERROR_ALREADY_INITIALIZED;
    return 0;
}

int APS5_VABI sceWebBrowserDialogTerminate(void) {
    int expected = 1;
    if (g_status.compare_exchange_strong(expected, COMMON_DIALOG_STATUS_NONE)) { g_open = false; return 0; }
    expected = COMMON_DIALOG_STATUS_FINISHED;
    if (g_status.compare_exchange_strong(expected, COMMON_DIALOG_STATUS_NONE)) { g_open = false; return 0; }
    if (g_status.load() == COMMON_DIALOG_STATUS_NONE) return COMMON_DIALOG_ERROR_NOT_INITIALIZED;
    return COMMON_DIALOG_ERROR_BUSY;
}

int APS5_VABI sceWebBrowserDialogClose(void) {
 if (g_status.load() == COMMON_DIALOG_STATUS_NONE) return COMMON_DIALOG_ERROR_NOT_INITIALIZED;
 g_status = COMMON_DIALOG_STATUS_FINISHED;
 g_open = false;
 return 0;
}

int APS5_VABI sceWebBrowserDialogGetResult(void* result) {
 const int status = g_status.load();
 if (status == COMMON_DIALOG_STATUS_NONE) return COMMON_DIALOG_ERROR_NOT_INITIALIZED;
 if (result == nullptr) return COMMON_DIALOG_ERROR_ARG_NULL;
 if (status != COMMON_DIALOG_STATUS_FINISHED) return COMMON_DIALOG_ERROR_NOT_FINISHED;
 *static_cast<std::int32_t*>(result) = COMMON_DIALOG_RESULT_USER_CANCELED;
 return 0;
}

int APS5_VABI sceWebBrowserDialogGetStatus(void) {
    return g_status.load();
}

int APS5_VABI sceWebBrowserDialogOpen(const void* param) {
 const int status = g_status.load();
 if (status == COMMON_DIALOG_STATUS_NONE) return COMMON_DIALOG_ERROR_NOT_INITIALIZED;
 if (status == COMMON_DIALOG_STATUS_RUNNING) return COMMON_DIALOG_ERROR_BUSY;
 if (param == nullptr) return COMMON_DIALOG_ERROR_ARG_NULL;
 g_status = COMMON_DIALOG_STATUS_FINISHED;
 g_open = true;
 return 0;
}

int APS5_VABI sceWebBrowserDialogUpdateStatus(void) {
    const int status = g_status.load();
    if (status == COMMON_DIALOG_STATUS_FINISHED) g_open = false;
    return status;
}


int APS5_VABI sceWebBrowserDialogSetCookie(const SetCookieParam* param) {
    if (g_status.load() == COMMON_DIALOG_STATUS_NONE) return COMMON_DIALOG_ERROR_NOT_INITIALIZED;
    if (g_open.load()) return COMMON_DIALOG_ERROR_INVALID_STATE;
    if (param == nullptr) return COMMON_DIALOG_ERROR_ARG_NULL;
    if (param->size != sizeof(SetCookieParam) || !LengthWithin(param->url, 0x800) || !LengthWithin(param->cookie, 0x1000) ||
        !ReservedClear(param->reserved))
        return COMMON_DIALOG_ERROR_PARAM_INVALID;
    return 0;
}

int APS5_VABI sceWebBrowserDialogOpenForPredeterminedContent(const void* param) {
    const int status = g_status.load();
    if (status == COMMON_DIALOG_STATUS_NONE) return COMMON_DIALOG_ERROR_NOT_INITIALIZED;
    if (status == COMMON_DIALOG_STATUS_RUNNING) return COMMON_DIALOG_ERROR_BUSY;
    if (param == nullptr) return COMMON_DIALOG_ERROR_ARG_NULL;
    g_status = COMMON_DIALOG_STATUS_FINISHED;
    g_open = true;
    return 0;
}

int APS5_VABI sceWebBrowserDialogResetCookie(const ResetCookieParam* param) {
    if (g_status.load() == COMMON_DIALOG_STATUS_NONE) return COMMON_DIALOG_ERROR_NOT_INITIALIZED;
    if (param == nullptr) return COMMON_DIALOG_ERROR_ARG_NULL;
    if (g_open.load()) return COMMON_DIALOG_ERROR_INVALID_STATE;
    if (param->size != sizeof(ResetCookieParam) || !ReservedClear(param->reserved)) return COMMON_DIALOG_ERROR_PARAM_INVALID;
    return 0;
}

int APS5_VABI sceWebBrowserDialogGetEvent() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceWebBrowserDialogNavigate() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceWebBrowserDialogSetZoom() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

}
