#include "prx/libc/include/General.hpp"

struct MbusProcessInfo;
struct MbusStartupInfo;
struct MbusStartupParam;
struct MbusControlStatus;
struct MbusDebugDeviceInfo;
struct MbusInternalInfo;
struct MbusPriorityInfo;

extern "C" {

int APS5_VABI sceMbusAcquireControl(std::uint64_t control, std::int32_t first, std::int32_t second) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusAcquireControlList() {
    return static_cast<int>(0x809B00FF);
}

int APS5_VABI sceMbusAcquireControlWithState(std::uint32_t deviceClass, std::int32_t state) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusAcquireControlWithState2(std::uint32_t deviceClass, std::int32_t moduleId, std::int32_t state) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusAcquireControlWithStateFlag(std::uint32_t deviceClass, std::uint32_t moduleId, std::uint32_t state, std::uint32_t flags) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusAddHandle(std::uint32_t deviceClass, std::int32_t handle, std::uint32_t flags) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusAddHandleByDeviceId(std::uint32_t deviceClass, std::int32_t handle, std::uint64_t deviceId, std::uint32_t first, std::uint32_t second, std::uint32_t third) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusAddHandleByUserId(std::uint32_t deviceClass, std::int32_t handle, std::int32_t userId, std::int32_t first, std::int32_t second, std::int32_t third) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusAddHandleByVidPid(std::uint32_t deviceClass, std::int32_t handle, std::int32_t first, std::uint16_t vendorId, std::uint16_t productId, std::int32_t second, std::int32_t third) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusAddHandleWithModuleId(std::uint32_t deviceClass, std::int32_t handle, std::uint32_t flags, std::uint32_t moduleId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusAttachToApplicationByPid(std::int32_t processId, std::int32_t deviceClass, std::int32_t flags) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusBindDeviceWithUserId(std::uint64_t deviceId, std::int32_t userId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusChangeAppModuleState(std::uint32_t deviceClass, std::int32_t moduleId, std::uint32_t state) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusCheckDriverCallback() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusConvertToLocalDeviceId(std::uint64_t deviceId, std::uint32_t* localId, std::uint32_t* deviceClass) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusConvertToLocalDeviceId2(std::uint64_t deviceId, std::uint32_t deviceClass, std::uint64_t* localId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusConvertToMbusDeviceId(std::uint64_t localId, std::uint32_t deviceClass, std::uint64_t* deviceId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDebugAcquireControl(std::uint64_t control, std::int32_t first, std::int32_t second, std::int32_t processId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDebugAcquireControlWithState(std::uint32_t deviceClass, std::int32_t state, std::int32_t processId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDebugAcquireControlWithState2(std::uint32_t deviceClass, std::int32_t moduleId, std::int32_t state, std::int32_t processId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDebugAcquireControlWithStateFlag(std::uint32_t deviceClass, std::uint32_t moduleId, std::uint32_t state, std::uint32_t flags, std::int32_t processId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDebugAddProcess(std::int32_t processId, std::int32_t flags, const MbusProcessInfo* info) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDebugCheckProcessResume(MbusProcessInfo* info, std::uint32_t flags) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDebugDisableBgmForShellUi(std::int32_t processId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDebugGetApplicationStartupInfo(std::uint32_t appId, const MbusProcessInfo* info, MbusStartupInfo* startupInfo) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDebugGetControlStatus(std::int32_t deviceClass, std::int32_t processId, MbusControlStatus* status) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDebugGetDeviceInfo(std::uint64_t deviceClass, MbusDebugDeviceInfo* info) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDebugGetInternalInfo(MbusInternalInfo* info) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDebugGetPriorityInfo(std::uint32_t deviceClass, MbusPriorityInfo* info) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDebugReenableBgmForShellUi(std::int32_t processId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDebugReleaseControl(std::uint64_t control, std::int32_t processId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDebugRemoveCameraAppModuleFocus(std::int32_t processId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDebugResumeApplication(std::int32_t appId, std::uint8_t firstFlag, std::uint8_t secondFlag, MbusProcessInfo* info) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDebugSetApplicationFocusByAppId(std::uint32_t appId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDebugSetCameraAppModuleFocus(std::int32_t processId, std::int32_t moduleId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDebugSetControllerFocusByAppId(std::uint32_t appId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDebugSetOtherProcessExcludedAction(std::uint64_t control, std::uint32_t action, std::int32_t processId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDebugSetPriority(std::uint64_t control, std::uint32_t priority, std::int32_t processId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDebugStartApplication(std::int32_t appId, std::int32_t first, std::int32_t second, std::uint8_t firstFlag, std::uint8_t secondFlag, const MbusStartupParam* param) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDebugStartApplication2(std::int32_t appId, std::int32_t first, std::int32_t second, std::uint8_t firstFlag, std::uint8_t secondFlag, const MbusStartupParam* param) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDebugStartApplicationNull() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDebugSuspendApplication() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDebugTerminateApplication() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDebugTerminateProcess() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDetachFromApplicationByPid() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDisableBgmForShellUi() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDisableChat() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDisconnectDevice() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDumpAppInfo() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDumpDeviceInfo() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDumpHandleStatusInfo() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDumpHandleStatusInfoAll() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDumpModuleStateInfo() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDumpModuleStateInfoAll() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDumpPlayerInfo() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDumpPortHandleInfo() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDumpProcMediaInfo() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDumpProcMediaInfoAll() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusDumpUserMediaInfo() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusEnableChat() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusEnterAudioOutAutoAttenuation() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusEventBusStatusChangeSubscribe() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusEventBusStatusChangeUnsubscribe() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusEventCallbackFuncsInit_() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusEventCreate_() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusEventDestroy() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusEventReceive() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusGetControlStatus() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusGetDeviceDescription() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusGetDeviceInfoByBusId_() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusGetDeviceInfoByConditionForDeviceService() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusGetDeviceInfoByCondition_() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusGetDeviceInfo_() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusGetSimulatedBusUsableStatusByBusType() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusGetSimulatedHandleStatusInfo() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusGetSimulatedHandleStatusInfo2() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusGetSparkState() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusInit() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusIsBgmProhibited() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusIsUsingDevice() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusLeaveAudioOutAutoAttenuation() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusNotifyBtDisconnection() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusNotifyDriverEvent() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusReenableBgmForShellUi() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusRegisterDriver() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusReleaseControl() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusRemoveCameraAppModuleFocus() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusRemoveHandle() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusRemoveProxy() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusResolveByDeviceId() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusResolveByHandle() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusResolveByPlayerId() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusResolveByUserId() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusSetAutoLoginMode() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusSetCameraAppModuleFocus() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusSetDeviceFunctionState() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusSetDuckingSetting() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusSetProxy() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusSetRoutingMode() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusSetScratchData() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusSetScratchDataUpdatedEventMask() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusSetSocialScreenAudioMode() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusSetTestFlag() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusSetThreadParam(std::int32_t priority, std::uint64_t affinityMask, std::uint64_t reserved) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusSetUsbPowerBlackList() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusSetUserMuteStatus() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusSetVolumeDown() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusStartAudioOutAuxBroadcast() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusStartAudioOutSharePlay() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusStopAudioOutAuxBroadcast() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusStopAudioOutSharePlay() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusTerm() {
    return 0;
}

int APS5_VABI sceMbusUnregisterDriver() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusUserLogin() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceMbusUserLogout() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

}
