# libSceHmd technical debt

### Silent stubs

- [libSceHmd](../../../core/libs/prx/libSceHmd/Export.cpp) implements only the disconnected-headset path: initialization succeeds, device queries report `NotDetected`, and opening a device returns `DeviceDisconnected`. Headset support, tracking, rendering and additional HMD exports are not implemented; SDK-level ABI compatibility and in-game behaviour remain unverified.

### Unknown function info

- [libSceHmd internal and capture exports](../../../core/libs/prx/libSceHmd/Export.cpp) - input/output directions and argument registers are recovered for shared VR2D data, device binding, optical and serial queries, screen status and capture operations. VR2D records are 128 bytes, optical output 16, serial output 16, PU position output 20, crash-dump output 68, audio/video status 32, and reprojection debug outputs 216 or 1136. Payload field meanings and capture callback contracts remain incomplete; these structures use forward declarations.

- [libSceHmd queries without a headset](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - the signatures, the distortion work memory (`0x20000`, aligned to `0x400`) and reprojection buffer sizes (garlic `0x100000`, onion `0x810`, both aligned to `0x100`), and the results of `sceHmdGetFieldOfView`, `sceHmdGet2DEyeOffset`, `sceHmdGetInertialSensorData` (`HANDLE_INVALID` once initialized; field of view checks its output for null first) and `sceHmdGetAssyError` (null output, then `DEVICE_DISCONNECTED` once initialized) follow [shadPS4](https://github.com/shadps4-emu/shadPS4/blob/a808e76/src/core/libraries/hmd/hmd.cpp) (PS4) with no headset, not a PS5 measurement. Before initialization `sceHmdGetFieldOfView` returns `NOT_INITIALIZED` for a non-null output, where shadPS4 checks the handle first and returns `HANDLE_INVALID`. The third argument of `sceHmdGetInertialSensorData` is unnamed there and taken as a count
- [sceHmdReprojectionStartWideNear](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdInternalDfuGetStatus](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdGetDistortionWorkMemoryAlign](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdReprojectionStartMultilayer](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdGetDistortionWorkMemorySize](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdReprojectionSetDisplayBuffers](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdInternalMirroringModeSetAspectDebug](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdDistortionGetCompoundEyeCorrectionCommand](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdDistortionGetCorrectionCommand](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdReprojectionAddDisplayBuffer](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdReprojectionInitialize](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdInternalGetDeviceInformationByHandle](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdInternalGetDeviceInformation](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdReprojectionStartCapture](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdInternalGetDefaultLedData](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdDistortionGetWideNearCorrectionCommand](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdDistortionInitialize](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdReprojectionStart](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdInternalCrashReportCancel](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdInternalSeparateModeOpen](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdDistortionGet2dVrCommand](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdGetWideNearDistortionCorrectionCommand](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdReprojectionStartMultilayer2](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdGetDistortionCorrectionCommand](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdReprojectionStartWithOverlay](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdReprojectionStartWideNearWithOverlay](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdGetDistortionParams](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdReprojectionStart2dVr](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdInternalSetDefaultLedData](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdInternalGetStatusReport](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
- [sceHmdGet2dVrCommand](../../../core/libs/prx/libSceHmd/Export.cpp) (libSceHmd) - unknown signature
