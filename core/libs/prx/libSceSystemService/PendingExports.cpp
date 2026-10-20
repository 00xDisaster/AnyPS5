#include "prx/libc/include/General.hpp"

namespace {

constexpr int SystemServiceUnavailable = static_cast<int>(0x80A10002);
constexpr int SystemServiceRejected = static_cast<int>(0x80A10005);

}

struct ShellCoreLaunchByUriParam;
struct ShellCoreTelemetryToken;
struct ShellCoreUmmStatus;
struct ShellCorePbtcUserInfoList;
struct ShellCorePrivacyEventData;
struct ShellCorePsnAccountInfoFirst;
struct ShellCorePsnAccountInfoSecond;
struct ShellCorePsnAccountInfoThird;
struct ShellCorePsnAccountInfoFourth;
struct ShellCorePsnAccountInfoFifth;
struct ShellCoreAppLaunchTypeInfo;
struct LncLaunchParam;
struct LncDaemonEvent;
struct LncShellUiEvent;
struct LncLocalProcessStatus;
struct LncCoredumpState;
struct LncVideoFinalizeState;
struct LncLocalProcessParam;
struct LncAppStatus;
struct LncShellUiAppStatus;
struct AppMessagingMessage;

extern "C" {

int APS5_VABI sceAppMessagingClearEventFlag(std::int32_t appId, std::uint64_t flags) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceAppMessagingReceiveMsg(AppMessagingMessage* message) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceAppMessagingSendMsg(std::uint32_t appId, std::uint32_t messageId, const void* data, std::size_t size, std::uint32_t flags) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceAppMessagingSendMsgToShellCore(std::uint32_t messageId, const void* data, std::size_t size, std::uint32_t flags) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceAppMessagingSendMsgToShellUI(std::uint32_t messageId, const void* data, std::size_t size, std::uint32_t flags) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceAppMessagingSetEventFlag(std::int32_t appId, std::uint64_t flags) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceAppMessagingTryGetEventFlag(std::uint64_t mask, std::uint64_t* flags, std::uint8_t clear) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceAppMessagingTryReceiveMsg(AppMessagingMessage* message) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilAcquireCpuBudgetOfExtraAudioDevices() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilAcquireCpuBudgetOfImeForBigApp() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilAcquireCpuBudgetOfInGameStore() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilActivateCdlg() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilAddLocalProcess(std::int32_t appId, const char* path, const char* const* argv, const LncLocalProcessParam* param) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilBlockAppSuspend(const char* reason) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilBlockingGetEventForDaemon() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilContinueApp() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilCrashSyscore() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilDeactivateCdlg() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilDeclareReadyForSuspend() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilDisableSuspendNotification() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilEnableSuspendNotification() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilFinishSpecialResume() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilForceKillApp(std::int32_t appId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilForceKillLocalProcess(std::int32_t appId, std::int32_t processId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilGetAppCategory(std::int32_t appId, std::uint32_t* category) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilGetAppCategoryType(std::int32_t appId, std::uint32_t* categoryType) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilGetAppFocusedAppStatus() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilGetAppId() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilGetAppIdListOfBigApp(std::int32_t* appIds, std::uint32_t capacity, std::uint32_t* count) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilGetAppIdOfRunningBigApp() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilGetAppLaunchedUser(std::int32_t appId, std::int32_t* userId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilGetAppStatus() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilGetAppStatusList(LncAppStatus* statuses, std::uint32_t capacity, std::uint32_t* count) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilGetAppStatusListForShellUIReboot(LncShellUiAppStatus* statuses, std::uint32_t capacity, std::uint32_t* count) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilGetAppTitleId(std::int32_t appId, char* titleId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilGetAppType(std::int32_t appId, std::uint32_t* type) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilGetCdlgExec(std::int32_t appId, bool* executing) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilGetCdlgType(std::int32_t appId, std::uint32_t* type) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilGetCoredumpState(std::int32_t appId, std::int32_t processId, LncCoredumpState* state) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilGetDbgExecutablePath(char* path, std::uint64_t selector) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilGetEventForDaemon(LncDaemonEvent* event) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilGetEventForShellUI(LncShellUiEvent* event) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilGetGpuCrashFullDumpAppStatus() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilGetLocalProcessStatusList(LncLocalProcessStatus* statuses, std::uint32_t capacity, std::uint32_t* count) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilGetParentSocket() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilGetResultKillApp(std::uint32_t requestId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilGetResultLaunchAppByTitleId(std::uint32_t requestId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilGetVideoFinalizeState(std::int32_t appId, std::int32_t processId, LncVideoFinalizeState* state) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilInitialize() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilIsActiveCdlg(bool* active) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilIsAppLaunched(const char* titleId, bool* launched) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilIsAppSuspended(std::int32_t appId, bool* suspended) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilIsCdlgActivated() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

bool APS5_VABI sceLncUtilIsCpuBudgetOfExtraAudioDevicesAvailable() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilIsHostBoot(std::int32_t appId, bool* hostBoot) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilIsPs2Emu(bool* emulated) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

bool APS5_VABI sceLncUtilIsShellUiFgAndGameBgCpuMode() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilIsSpeculativeLaunch(std::int32_t appId, bool* speculative) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilKickCoredumpOnlyProcMem(std::int32_t appId, std::int32_t processId, const char* first, const char* second) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilKillLocalProcess(std::int32_t appId, std::int32_t processId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilLaunchApp(const char* titleId, const char* const* argv, const LncLaunchParam* param) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilLoadExec(const char* path, const char* const* argv) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilNotifyCoredumpRequestEnd(std::int32_t appId, std::int32_t processId, std::int32_t result) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilNotifyCoredumpRequestProgress(std::int32_t appId, std::int32_t processId, std::int32_t progress) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilNotifyVshReady() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilRaiseException(std::int32_t appId, std::uint64_t exception) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilRaiseExceptionLocalProcess(std::int32_t appId, std::int32_t processId, std::uint64_t exception) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilRegisterCdlgSharedMemoryName() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilRegisterDaemon() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilRegisterShellUI() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilReleaseCpuBudgetOfExtraAudioDevices() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilReleaseCpuBudgetOfImeForBigApp() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilReleaseCpuBudgetOfInGameStore() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilResumeApp(std::int32_t appId, std::uint64_t flags) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilResumeLocalProcess(std::int32_t processId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilSetAppFocus(std::int32_t appId, std::uint64_t first, std::uint64_t second) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilSetCdlgExec(std::int32_t appId, std::uint8_t executing) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilSetControllerFocus(std::int32_t appId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilSetControllerFocusPermission(std::uint8_t permission) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilStartKillApp(std::int32_t appId, std::uint32_t* requestId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilStartLaunchAppByTitleId(const char* titleId, const char* const* argv, const LncLaunchParam* param, std::uint32_t* requestId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilSuspendApp(std::int32_t appId, std::uint64_t flags, std::int32_t reason) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilSuspendBackgroundApp(std::int32_t first, std::int32_t second) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilSuspendLocalProcess(std::int32_t processId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilTerminate() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilTryBlockAppSuspend(const char* reason) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilUnblockAppSuspend(const char* reason) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilUnregisterCdlgSharedMemoryName() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilUnregisterDaemon() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceLncUtilUnregisterShellUI() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilAccessibilityZoomLock() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilAccessibilityZoomUnlock() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilAcquireBgmCpuBudget() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilAcquireRemotePlayCpuBudget() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilAcquireSharePlayCpuBudget() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilActivateAbort() {
    return 0;
}

int APS5_VABI sceShellCoreUtilActivateGetStatus() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilActivateInit() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilActivateIsActivated() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilActivateRecordActivation() {
    return 0;
}

int APS5_VABI sceShellCoreUtilActivateStart() {
    return 0;
}

int APS5_VABI sceShellCoreUtilActivateStartAsync() {
    return 0;
}

int APS5_VABI sceShellCoreUtilActivateTerm() {
    return 0;
}

int APS5_VABI sceShellCoreUtilChangeRunLevel(std::int32_t runLevel) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilChangeToStaffModeForIDU() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilCheckerAbort() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilCleanupCrashReport() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilClearAppData() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilClearPs4AppCategoryDefaultsListUpdate() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilClearPsnAccountInfo() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilCrashReportRequestCancel() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilDeclareBeginOfExternalStorageAppMove() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilDeclareEndOfExternalStorageAppMove() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilDeleteDiscInstalledTitleWorkaroundFile() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilDeleteDownloadedHidConfigFile(std::uint32_t deviceType) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilDeleteDownloadedNetEvConfigFile() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilDeleteDownloadedTitleWorkaroundFile() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilDoFsck(const char* path) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilDownloadHidConfigFileFromServer(std::uint32_t deviceType) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilDownloadNetEvConfigFileFromServer() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilDownloadTitleWorkaroundFileFromServer() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilEndUmmDictation() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilEnterPowerLockSection() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilExecuteCrashReport() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilExfatFormatExternalHdd(const char* path) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilExitApp() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilExitMiniApp() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilExitMiniAppWithValue(const char* value) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilFormatExternalHdd(const char* path) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilFormatHddForRestore() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetAppData(const char* key, void* data, std::int32_t capacity, std::int32_t* size) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetAppEnableTTS(std::int32_t appId, bool* enabled) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetAppEnterButtonAssign(std::int32_t appId, std::int32_t* assignment) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetAppLaunchTypeInfo(std::int32_t appId, ShellCoreAppLaunchTypeInfo* info) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetAppLaunchedParamInt(std::int32_t appId, const char* key, std::int32_t* value) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetAppLaunchedParamIntByBudgetType(std::int32_t budgetType, const char* key, std::int32_t* value) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetAppLaunchedParamString(std::int32_t appId, const char* key, char* value, std::int32_t capacity) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetAppLaunchedParamStringByBudgetType(std::int32_t budgetType, const char* key, char* value, std::int32_t capacity) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetAutoPowerDownRemainingSeconds() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetBasicProductShape() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetCheckerString(const char* key, char* value, std::size_t capacity) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetCheckerStringEx(const char* firstKey, const char* secondKey, char* value, std::size_t capacity) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetCloudClientStatus(std::uint32_t* status) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetCrashReportCoreFileSetSize() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetCrashReportFilterInfoStart() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetCrashReportInfoForBoot() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetCrashReportInfoForBootStart() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetCrashReportInfoStart() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetCrashReportResult() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetCrashReportStatus() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetCrashReportUploadStatus() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetDeviceIndexBehavior(std::uint32_t* first, std::uint32_t* second, std::uint32_t* third) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetDeviceIndexBehaviorWithTimeout(std::uint32_t* first, std::uint32_t* second, std::uint32_t* third, std::uint8_t timeout) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetEffectiveTotalSizeOfUserPartition(std::uint64_t* size) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

std::uint64_t APS5_VABI sceShellCoreUtilGetFreeSizeOfAvContentsTmp() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetFreeSizeOfUserPartition(std::uint64_t* firstSize, std::uint64_t* secondSize) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetFsckProgress(const char* path, std::uint32_t* progress) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetGameLiveStreamingStatus(bool* active) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

bool APS5_VABI sceShellCoreUtilGetGnmCompositorOnScreenProfilerFlag() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetGpuLoadEmulationMode() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetGpuLoadEmulationModeByAppId(std::int32_t appId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetHidConfigFileInfoString(char* value, std::size_t capacity, std::int32_t index) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetHidConfigFileString(char* value, std::size_t capacity, std::int32_t index) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetHidConfigName(char* value, std::size_t capacity, std::int32_t index) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetHidConfigNum(std::uint32_t* count) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetIDUMode(std::uint8_t* mode) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetImposeMenuFlagForPs2Emu() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetManifestFileStatus() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetNeedSizeOfAppContent(std::uint64_t* size) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetNetEvConfigFileInfoString(char* value, std::size_t capacity) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetOutOfVrPlayZoneWarning() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetPapcGamePcl() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetPbtcUserInfoList(ShellCorePbtcUserInfoList* info) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetPlatformPrivacyDefinitionEventData(const char* firstKey, const char* secondKey, ShellCorePrivacyEventData* data) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetProgressOfFormatExternalHdd() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetProgressOfFsck() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetPs4AppCategoryDefaultsListId(void* data, std::size_t capacity) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetPs4AppCategoryForTitleId(const char* titleId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetPsStoreIconLayout(std::int32_t appId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetPsStoreIconState(std::int32_t appId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetPsnAccountInfo(ShellCorePsnAccountInfoFirst* first, ShellCorePsnAccountInfoSecond* second, ShellCorePsnAccountInfoThird* third, ShellCorePsnAccountInfoFourth* fourth, ShellCorePsnAccountInfoFifth* fifth) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetRemotePlayStatus(bool* active) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetRunLevel() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetSharePlayStatus(bool* active) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetSocialScreenStatus(bool* active) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetSplashScreenState(std::int32_t appId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetSupportSiteURL(std::uint32_t first, std::uint32_t second, char* value, std::size_t capacity) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetSuspendConfirmationDialogFlag(std::int32_t appId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetSystemBGState(std::int32_t appId, std::uint32_t* state) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetSystemBGWaveColor(std::int32_t appId, std::uint32_t* color) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetSystemBGWaveState(std::int32_t appId, std::uint32_t* state) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetTelemetryToken(ShellCoreTelemetryToken* token) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetTitleWorkaroundFileInfoString(char* value, std::size_t capacity) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetTitleWorkaroundFileString(char* value, std::size_t capacity) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetUIStatus(std::uint64_t* status) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetUmmStatus(std::uint32_t id, ShellCoreUmmStatus* status) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetUserFocus(std::int32_t userId, std::int32_t* appId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetUserIdOfMorpheusUser() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetVMStatsForTelemetry(char* value, std::size_t capacity) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGetVersionNumberOfCameraCalibrationData() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilGoBackToKratosCurrentSessionGame() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilHideBlocksFromUser(std::uint64_t first, std::int32_t second) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilIncrementVersionNumberOfCameraCalibrationData() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilIsAccessibilityZoomLocked() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

bool APS5_VABI sceShellCoreUtilIsBgmCpuBudgetAcquired() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

bool APS5_VABI sceShellCoreUtilIsBgmCpuBudgetAvailable() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilIsBgmPlaying() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

bool APS5_VABI sceShellCoreUtilIsExternalStorageAppMoveInProgress() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilIsEyeToEyeDistanceAdjusted(std::uint32_t userId, std::uint8_t* adjusted) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

bool APS5_VABI sceShellCoreUtilIsGameLiveStreamingOnAir() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilIsIDUState() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

bool APS5_VABI sceShellCoreUtilIsImposeScreenOverlaid() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilIsInSystemSuspendBlackList() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilIsInternalKratosUser() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilIsKilledOrSuspendedByLogout(std::int32_t first, std::int32_t second, std::uint8_t* result) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilIsNeededCrashReport() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

bool APS5_VABI sceShellCoreUtilIsPowerSaveAlertRequested() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

bool APS5_VABI sceShellCoreUtilIsRemotePlayCpuBudgetAcquired() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

bool APS5_VABI sceShellCoreUtilIsScreenSaverOn() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

bool APS5_VABI sceShellCoreUtilIsSharePlayCpuBudgetAcquired() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilIsShowCrashReport() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilIsTemperatureDanger() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilIsTitleWorkaroundEnabled(std::int32_t selector, const char* key) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

bool APS5_VABI sceShellCoreUtilIsUsbMassStorageMounted(std::uint32_t deviceIndex) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilLaunchByUri(const char* uri, const ShellCoreLaunchByUriParam* param) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilLeavePowerLockSection() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilMakeManifestFile() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilMountAppRight(const char* key) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilMountDownloadDataForShellUI(const char* key) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilMountHddForBackup() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilMountHddForRestore() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilNavigateToAnotherApp(std::uint32_t target) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilNavigateToGoHome(std::uint32_t target) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilNavigateToLaunchedApp(std::uint32_t target) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilNotificationCancelForIDU(std::int64_t notificationId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilNotificationRequestedForIDU(std::int64_t notificationId, const char* text, std::uint32_t type) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilNotifyBgmCoreTermination() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilNotifyFsReadError(std::int32_t first, std::int32_t second, std::int32_t third, const char* detail) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilNotifyImeStatusChanged(std::int32_t appId, std::uint8_t status) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilNotifyPsnAccountInfoReceived() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilNotifySystemSuspendTelemetryInfo(std::uint32_t info) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilPfAuthClientConsoleTokenClearCache() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilPostActivityForPsNow(std::int32_t type, const char* activity) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilPostErrorLog(const char* firstText, std::uint32_t firstValue, const char* secondText, const char* thirdText, std::uint32_t secondValue, const char* fourthText) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilPostLaunchConfirmResult(std::uint32_t first, std::uint32_t second) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilPostPsmEventToShellUI(std::int32_t eventId, const char* data) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilPreNotifyOfGameLiveStreaming() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilPreNotifyOfRemotePlay() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilPreNotifyOfSharePlay() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilReleaseBgmCpuBudget() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilReleaseRemotePlayCpuBudget() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilReleaseSharePlayCpuBudget() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilReportSessionErrorToGaikaiController(std::int32_t error, const char* detail) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilReportUnexpectedFatalErrorToSystemTelemetry(std::int32_t error, const char* detail) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilRequestCameraCalibration() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilRequestEjectDevice(const char* path) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilRequestRebootApp(std::int32_t appId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilRequestShutdown(std::int32_t reason) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilResetAutoPowerDownTimer() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilResetBgdcConfig() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilSetAppData() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilSetBgmProhibition() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilSetDeviceIndexBehavior() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilSetGameLiveStreamingOnAirFlag() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilSetGameLiveStreamingStatus() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilSetGnmCompositorOnScreenProfilerFlag() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilSetGpuLoadEmulationMode() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilSetGpuLoadEmulationModeByAppId() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilSetIDUMode() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilSetImposeStatusFlag() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilSetPsStoreIconLayout() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilSetPsStoreIconState() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilSetRemotePlayStatus() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilSetSharePlayStatus() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilSetSkipUpdateCheck() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilSetSocialScreenStatus() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilSetSplashScreenState() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilSetSystemBGState() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilSetSystemBGWaveColor() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilSetSystemBGWaveState() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilSetUIStatus() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilSetUserFocus() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilShowCriticalErrorDialog() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilShowErrorDialog() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilShowErrorDialogWithFormatArgs() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilShowErrorDialogWithParam() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilShowPsUnderLockIndicator() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilSignalUserInput() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilStartPsNowGame() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilStopPsNowGame() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilTalkKeyEventReceived() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilTestBusTransferSpeed() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilTickHeartBeat() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilTriggerPapcUpdate() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilTurnOffScreenSaver() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilUnmountAppRight() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilUnmountDownloadDataForShellUI() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilUnmountHddForBackup() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreUtilUnmountHddForRestore() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceShellCoreutilGetCrashReportProcessInformation() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemModalDialogClose() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemModalDialogOpen() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceAcquireBgmCpuBudget() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceAcquireFb0() {
    return SystemServiceUnavailable;
}

int APS5_VABI sceSystemServiceActivateHevc() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceActivateHevcAbort() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceActivateHevcGetStatus() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceActivateHevcInit() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceActivateHevcIsActivated() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceActivateHevcSoft() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceActivateHevcSoftAbort() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceActivateHevcSoftGetStatus() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceActivateHevcSoftInit() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceActivateHevcSoftIsActivated() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceActivateHevcSoftStart() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceActivateHevcSoftTerm() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceActivateHevcStart() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceActivateHevcTerm() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceActivateMpeg2Abort() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceActivateMpeg2GetStatus() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceActivateMpeg2Init() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceActivateMpeg2IsActivated() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceActivateMpeg2Start() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceActivateMpeg2Term() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceAddLocalProcess() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceAddLocalProcessForJvm() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceAddLocalProcessForPs2Emu() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceAppGetAppInstallStatus() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceChangeAcpClock() {
    return 0;
}

int APS5_VABI sceSystemServiceChangeClock() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceChangeCpuClock() {
    return 0;
}

int APS5_VABI sceSystemServiceChangeCpuPstate() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceChangeGpuClock() {
    return 0;
}

int APS5_VABI sceSystemServiceChangeMemPstate() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceChangeMemoryClock() {
    return 0;
}

int APS5_VABI sceSystemServiceChangeMemoryClockToBaseMode() {
    return 0;
}

int APS5_VABI sceSystemServiceChangeMemoryClockToDefault() {
    return 0;
}

int APS5_VABI sceSystemServiceChangeMemoryClockToMultiMediaMode() {
    return 0;
}

int APS5_VABI sceSystemServiceChangeNumberOfGpuCu() {
    return 0;
}

int APS5_VABI sceSystemServiceChangeSamuClock() {
    return 0;
}

int APS5_VABI sceSystemServiceChangeUvdClock() {
    return 0;
}

int APS5_VABI sceSystemServiceChangeVceClock() {
    return 0;
}

int APS5_VABI sceSystemServiceDeclareReadyForSuspend() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceDisablePartyVoice() {
    return SystemServiceUnavailable;
}

int APS5_VABI sceSystemServiceDisablePersonalEyeToEyeDistanceSetting() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceDisableSuspendConfirmationDialog() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceDisableSuspendNotification() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceDisableVoiceRecognition() {
    return SystemServiceUnavailable;
}

int APS5_VABI sceSystemServiceEnablePersonalEyeToEyeDistanceSetting() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceEnableSuspendConfirmationDialog() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceEnableSuspendNotification() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetAppCategoryType() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetAppFocusedAppStatus() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetAppId() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetAppIdListOfBigApp() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetAppLaunchedUser() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetAppStatus() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetAppTitleId() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetAppType() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetDbgExecutablePath() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetEventForDaemon() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetGpuLoadEmulationMode() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetLocalProcessStatusList() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetMainAppTitleId() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetPSButtonEvent() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetParentSocket() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetParentSocketForJvm() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetParentSocketForPs2Emu() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetParentSocketForPsmKit() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetPlatformPrivacyDefinitionData() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetPlatformPrivacyDefinitionVersion() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetPlatformPrivacySetting() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetRecentLotusPushedButton() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetRemoteplayStatus() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetRenderingMode() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetSubAppTitleId() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetSubAppTitleIdForSP() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetTitleWorkaroundInfo() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetUIStatus() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceGetVersionNumberOfCameraCalibrationData() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceInitializeForShellCore() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceInvokeAppLaunchLink() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceIsAppSuspended() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceIsBgmCpuBudgetAvailable() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceIsBgmPlaying() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceIsEyeToEyeDistanceAdjusted() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceIsGameLiveStreamingOnAir() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceIsScreenSaverOn() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceIsShellUiFgAndGameBgCpuMode() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceKillLocalProcess() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceKillLocalProcessForJvm() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceKillLocalProcessForPs2Emu() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceLaunchEventDetails() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceLaunchStore() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceLaunchTournamentList() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceLaunchTournamentsTeamProfile() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceLaunchUdsApp() {
    return SystemServiceRejected;
}

int APS5_VABI sceSystemServiceLaunchWebApp() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceLoadExecVideoServiceWebApp() {
    return SystemServiceUnavailable;
}

int APS5_VABI sceSystemServiceNavigateToAnotherApp() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceNavigateToGoBack() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceNavigateToGoBackWithValue() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceNavigateToGoHome() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceNotifyBgmCoreTermination() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceRaiseExceptionLocalProcess() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceReenablePartyVoice() {
    return SystemServiceUnavailable;
}

int APS5_VABI sceSystemServiceReenableVoiceRecognition() {
    return SystemServiceUnavailable;
}

int APS5_VABI sceSystemServiceRegisterDaemon() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceReleaseBgmCpuBudget() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceReleaseFb0() {
    return SystemServiceUnavailable;
}

int APS5_VABI sceSystemServiceRequestCameraCalibration() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceRequestPowerOff() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceRequestReboot() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceRequestToChangeRenderingMode() {
    return SystemServiceRejected;
}

int APS5_VABI sceSystemServiceResumeLocalProcess() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceSaveVideoToken() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceSetControllerFocusPermission() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceSetGpuLoadEmulationMode() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceSetOutOfVrPlayAreaFlag() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceSetOutOfVrPlayZoneWarning() {
    return 0;
}

int APS5_VABI sceSystemServiceSetPowerSaveLevel() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceShowClosedCaptionAdvancedSettings() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceShowClosedCaptionSettings() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceShowErrorDialog() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceShowEyeToEyeDistanceSetting() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceShowImposeMenuForPs2Emu() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceSuspendBackgroundApp() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceSuspendLocalProcess() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceTelemetrySetData() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceTelemetrySetData2() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceTickVideoPlayback() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceTurnOffScreenSaver() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceUsbStorageGetDeviceInfo() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceUsbStorageGetDeviceList() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceUsbStorageGetdentsClose() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceUsbStorageGetdentsOpen() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceUsbStorageGetdentsRead() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceUsbStorageInit() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceUsbStorageIsExist() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceUsbStorageRegisterCallback() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceUsbStorageRegisterCallbackForMapAvailable() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceUsbStorageRegisterCallbackForMapUnavailable() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceUsbStorageRequestMap() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceUsbStorageRequestMapWSB() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceUsbStorageRequestUnmap() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceUsbStorageTerm() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceUsbStorageUnregisterCallback() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceUsbStorageUnregisterCallbackForMapAvailable() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemServiceUsbStorageUnregisterCallbackForMapUnavailable() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemStateMgrCancelShutdownTimer() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemStateMgrEnterMediaPlaybackMode() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemStateMgrEnterStandby() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemStateMgrExtendShutdownTimer() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemStateMgrExtendShutdownTimerForPostAutoUpdateProcess() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemStateMgrGetCurrentState() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemStateMgrGetTriggerCode() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemStateMgrIsBdDriveReady() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemStateMgrIsGpuPerformanceNormal() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemStateMgrIsShellUIShutdownInProgress() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemStateMgrIsStandbyModeEnabled() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemStateMgrLeaveMediaPlaybackMode() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemStateMgrNotifySystemSuspendResumeProgress() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemStateMgrReboot() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemStateMgrRequestToKeepMainOnStandby() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemStateMgrSendCecOneTouchPlayCommand() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemStateMgrStartRebootTimer() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemStateMgrStartShutdownTimer() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemStateMgrStartStadbyTimer() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemStateMgrStartVshAutoUpdateTimer() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemStateMgrTickMusicPlayback() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemStateMgrTickPartyChat() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemStateMgrTurnOff() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemStateMgrVshAutoUpdate() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemStateMgrWaitVshAutoUpdateVerifyDone() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceSystemStateMgrWakeUp() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

}
