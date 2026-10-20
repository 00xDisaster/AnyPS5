#include "Hmd.hpp"
#include "prx/libc/include/General.hpp"

#include <atomic>

namespace {

std::atomic<bool> initialized{false};

std::int32_t Initialize(const Hmd::InitializeParam* param) {
    if (initialized.load()) return Hmd::AlreadyInitialized;
    if (!param) return Hmd::ParameterNull;
    if (param->reserved0) return Hmd::UnsupportedFeature;
    return initialized.exchange(true) ? Hmd::AlreadyInitialized : 0;
}

}

struct HmdEyeDistances {
    std::uint32_t left;
    std::uint32_t right;
};

struct HmdEyeOffset {
    float x;
    float y;
    float z;
};

struct HmdScreenData {
    const void* data;
    std::int32_t size;
};

static_assert(sizeof(HmdScreenData) == 16);
static_assert(offsetof(HmdScreenData, size) == 8);
static_assert(sizeof(HmdEyeDistances) == 8);
static_assert(sizeof(HmdEyeOffset) == 12);

extern "C" {

std::int32_t APS5_VABI sceHmdInitialize(const Hmd::InitializeParam* param) {
    return Initialize(param);
}

std::int32_t APS5_VABI sceHmdInitialize315(const Hmd::InitializeParam* param) {
    return Initialize(param);
}

std::int32_t APS5_VABI sceHmdTerminate() {
    return initialized.exchange(false) ? 0 : Hmd::NotInitialized;
}

std::int32_t APS5_VABI sceHmdOpen(std::int32_t userId, std::int32_t type, std::int32_t index,
                                Hmd::OpenParam* param) {
    if (!initialized.load()) return Hmd::NotInitialized;
    if (type != 0 || index != 0 || param || userId == -1 || userId == 0xff)
        return Hmd::ParameterInvalid;
    return Hmd::DeviceDisconnected;
}

std::int32_t APS5_VABI sceHmdClose(std::int32_t handle) {
    (void)handle;
    return initialized.load() ? Hmd::InvalidHandle : Hmd::NotInitialized;
}

std::int32_t APS5_VABI sceHmdGetDeviceInformation(Hmd::DeviceInformation* info) {
    if (!info) return Hmd::ParameterNull;
    if (!initialized.load()) return Hmd::NotInitialized;
    *info = {};
    info->status = Hmd::DeviceStatus::NotDetected;
    info->userId = -1;
    return 0;
}

std::int32_t APS5_VABI sceHmdGetDeviceInformationByHandle(std::int32_t handle,
                                                        Hmd::DeviceInformation* info) {
    (void)handle;
    (void)info;
    return initialized.load() ? Hmd::InvalidHandle : Hmd::NotInitialized;
}

std::int32_t APS5_VABI sceHmdInternalGetDeviceStatus(Hmd::DeviceStatus* status) {
    if (!status) return Hmd::ParameterNull;
    *status = Hmd::DeviceStatus::NotDetected;
    return 0;
}

std::int32_t APS5_VABI sceHmdInternalMmapIsConnect() {
    return 0;
}

int APS5_VABI sceHmdInternalGetPUVersion(std::uint32_t* version) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalEventInitialize(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSetVirtualDisplaySize(std::uint8_t size) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSocialScreenSetOutput(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalGetVirtualDisplayDepth(std::uint8_t* depth) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalGetEyeStatus(std::int32_t handle, HmdEyeOffset* left, HmdEyeOffset* right) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalAnotherScreenOpen(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdReprojectionUnsetCallback(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSetCrashReportCommand(std::uint8_t command) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalGetDebugTextMode(std::uint8_t* mode) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdDistortionGetWorkMemoryAlign(void) {
    return 0x400;
}

int APS5_VABI sceHmdInternalRegisterEventCallback(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdDistortionGetWorkMemorySize(void) {
    return 0x20000;
}

int APS5_VABI sceHmdReprojectionStartWideNear(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalGetPUSerialNumber(char* serialNumber) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalGetDebugSocialScreenMode(std::uint8_t* mode) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSetM2LedBrightness(std::int32_t handle, std::uint32_t ledMask, const std::uint8_t* brightness) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSetIPD(std::int32_t mode, HmdEyeDistances distances) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalMirroringModeSetAspect(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalDfuGetStatus(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalAnotherScreenSendVideo(const HmdScreenData* data) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalGetRequiredPUPVersion(std::uint32_t* firstVersion, std::uint32_t* secondVersion) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdReprojectionSetCallback(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalDfuCheckAfterPvt(std::uint8_t* afterPvt) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalResetInertialSensor(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdReprojectionSetUserEventStart(std::int64_t eventQueue, std::int32_t eventId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdDistortionSetOutputMinColor(float red, float green, float blue) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSetVirtualDisplayDepth(std::uint8_t depth) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdGetDistortionWorkMemoryAlign(void) {
    return 0x400;
}

int APS5_VABI sceHmdReprojectionStartMultilayer(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalPushVr2dData(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdReprojectionClearUserEventEnd(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalGetIPD(HmdEyeDistances* distances) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSetVRMode(std::int32_t enabled) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalCrashReportReadDataSize(std::uint32_t* dataSize) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalGetVr2dData(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalAnotherScreenSendAudio(const HmdScreenData* data) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalGetPuPositionParam(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdReprojectionInitializeCapture(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdGet2DEyeOffset(std::int32_t handle, HmdEyeOffset* leftOffset, HmdEyeOffset* rightOffset) {
    (void)handle;
    (void)leftOffset;
    (void)rightOffset;
    return initialized.load() ? Hmd::InvalidHandle : Hmd::NotInitialized;
}

int APS5_VABI sceHmdInternalSetDebugSocialScreenMode(std::uint8_t mode) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalGetHmuPowerStatusForDebug(std::uint8_t* status) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdGetDistortionWorkMemorySize(void) {
    return 0x20000;
}

int APS5_VABI sceHmdReprojectionSetDisplayBuffers(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalAnotherScreenSetFadeAndSwitch(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalCreateSharedMemory(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalMirroringModeSetAspectDebug(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalGetPuBuildNumber(std::uint32_t* buildNumber) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdDistortionGetCompoundEyeCorrectionCommand(void) {
    return static_cast<int>(0x81110009);
}

int APS5_VABI sceHmdInternalSetS3dPassMode(std::uint8_t mode) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdReprojectionStopCapture(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

bool APS5_VABI sceHmdInternalIsCommonDlgMiniAppVr2d(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalMmapGetSensorCalibrationData(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdDistortionGetCorrectionCommand(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdReprojectionQueryOnionBuffAlign(void) {
    return 0x100;
}

int APS5_VABI sceHmdInternalGetDebugMode(std::uint8_t* mode) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

bool APS5_VABI sceHmdInternalIsMiniAppVr2d(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

bool APS5_VABI sceHmdInternalIsCommonDlgVr2d(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdReprojectionSetOutputMinColor(float red, float green, float blue) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSetPortConnection(std::uint64_t connection) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdReprojectionDebugGetLastInfo(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSetM2LedOn(std::int32_t handle, std::uint32_t ledMask, std::uint32_t brightness) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdGetFieldOfView(std::int32_t handle, void* fieldOfView) {
    (void)handle;
    if (!fieldOfView) return Hmd::ParameterNull;
    return initialized.load() ? Hmd::InvalidHandle : Hmd::NotInitialized;
}

int APS5_VABI sceHmdReprojectionAddDisplayBuffer(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSetIpdSettingEnableForSystemService(std::uint8_t enabled) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSetLedOn(std::int32_t handle, std::uint32_t ledMask) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternal3dAudioSendData(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdReprojectionInitialize(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalGetDeviceInformationByHandle(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdReprojectionStopLiveCapture(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalAnotherScreenClose(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalCrashReportReadData(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalGetDeviceInformation(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdReprojectionStartCapture(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalGetDefaultLedData(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalDfuClose(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdReprojectionQueryGarlicBuffAlign(void) {
    return 0x100;
}

int APS5_VABI sceHmdInternalSetHmuPowerControl(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalBindDeviceWithUserId(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalGetHmuSerialNumber(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdDistortionGetWideNearCorrectionCommand(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalCheckS3dPassModeAvailable(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalAnotherScreenGetAudioStatus(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSeparateModeGetAudioStatus(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdReprojectionStartLiveCapture(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalIsGameVr2d(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdGetAssyError(void* data) {
    if (!data) return Hmd::ParameterNull;
    return initialized.load() ? Hmd::DeviceDisconnected : Hmd::NotInitialized;
}

int APS5_VABI sceHmdReprojectionFinalize(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSocialScreenSetFadeAndSwitch(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalDfuStart(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSetVirtualDisplayHeight(std::uint8_t height) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdDistortionInitialize(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalDfuSetMode(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSetForcedCrash(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalDfuCheckPartialUpdateAvailable(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSeparateModeClose(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSetDebugTextMode(std::uint8_t mode) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalCrashReportClose(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSetHmuPowerControlForDebug(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalGetVirtualDisplayHeight(std::uint8_t* height) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdReprojectionStart(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalResetLedForVsh(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalDfuOpen(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSetDebugMode(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalCrashReportCancel(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSeparateModeGetVideoStatus(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalCheckDeviceModelMk3(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdSetVr2dEnable(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSetSidetone(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSeparateModeOpen(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalMmapGetModeId(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdDistortionGet2dVrCommand(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalDfuSendSize(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdGetWideNearDistortionCorrectionCommand(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdReprojectionStartMultilayer2(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdGetDistortionCorrectionCommand(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSetDeviceConnection(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalGetVirtualDisplaySize(std::uint8_t* size) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdReprojectionUnsetDisplayBuffers(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalGetHmuOpticalParam(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSeparateModeSendVideo(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalResetLedForVrTracker(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdReprojectionQueryOnionBuffSize(void) {
    return 0x810;
}

int APS5_VABI sceHmdReprojectionStartWithOverlay(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdReprojectionSetUserEventEnd(std::int64_t eventQueue, std::int32_t eventId) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalCrashReportOpen(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSetDemoMode(std::uint8_t mode) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdReprojectionStartWideNearWithOverlay(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdGetDistortionParams(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalDfuSend(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdReprojectionClearUserEventStart(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSetUserType(std::uint8_t userType) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalGetDemoMode(std::uint8_t* mode) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalMapSharedMemory(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalGetCrashDumpInfo(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalGetTv4kCapability(std::uint8_t* capability) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalDfuReset(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdReprojectionStart2dVr(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalMmapGetCount(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSetDefaultLedData(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdGetInertialSensorData(std::int32_t handle, void* data, std::int32_t count) {
    (void)handle;
    (void)data;
    (void)count;
    return initialized.load() ? Hmd::InvalidHandle : Hmd::NotInitialized;
}

int APS5_VABI sceHmdInternal3dAudioClose(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSetBrightness(std::uint8_t brightness) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSocialScreenGetFadeState(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSeparateModeSendAudio(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalGetStatusReport(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalGetBrightness(std::uint8_t* brightness) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdReprojectionFinalizeCapture(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSetPortStatus(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdReprojectionStop(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalAnotherScreenGetVideoStatus(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternal3dAudioOpen(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalAnotherScreenGetFadeState(std::int32_t* state) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalSetDebugGpo(std::uint32_t value) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalGetIpdSettingEnableForSystemService(std::uint8_t* enabled) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdReprojectionDebugGetLastInfoMultilayer(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdInternalGetPuRevision(std::uint32_t* revision) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmdReprojectionQueryGarlicBuffSize(void) {
    return 0x100000;
}

int APS5_VABI sceHmdGet2dVrCommand(void) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

}
