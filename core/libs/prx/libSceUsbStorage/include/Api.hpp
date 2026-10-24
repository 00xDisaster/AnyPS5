#pragma once

#include "prx/libSceSystemService/include/SystemService.hpp"
#include "SceTypes.hpp"

extern "C" {

int APS5_VABI sceUsbStorageInit(void* attr);
int APS5_VABI sceUsbStorageTerm(void);
int APS5_VABI sceUsbStorageRegisterCallback(int32_t eventType, UsbStorageCallback callback, void* arg);
int APS5_VABI sceUsbStorageUnregisterCallback(int32_t eventType, UsbStorageCallback callback);
int APS5_VABI sceUsbStorageRegisterCallbackForMapAvailable(uint32_t device, const char* directory, UsbStorageCallback callback, void* arg);
int APS5_VABI sceUsbStorageUnregisterCallbackForMapAvailable(uint32_t device, const char* directory, UsbStorageCallback callback);
int APS5_VABI sceUsbStorageRegisterCallbackForMapUnavailable(uint32_t device, const char* directory, UsbStorageCallback callback, void* arg);
int APS5_VABI sceUsbStorageUnregisterCallbackForMapUnavailable(uint32_t device, const char* directory, UsbStorageCallback callback);
int APS5_VABI sceUsbStorageGetDeviceInfo(uint32_t device, UsbStorageDeviceInfo* info);
int APS5_VABI sceUsbStorageGetDeviceList(uint32_t* devices, int32_t* count);
int APS5_VABI sceUsbStorageGetMountPointOfShellCore(uint32_t device, char* mountPoint);
bool APS5_VABI sceUsbStorageIsExist(uint32_t device, const char* directory, std::uint8_t* exists);
int APS5_VABI sceUsbStorageRequestMap(uint32_t device, const char* directory, int32_t flags, uint64_t volumeSize, char* mountPoint, uint64_t* mountId);
int APS5_VABI sceUsbStorageRequestUnmap(uint32_t device, const char* directory);
int APS5_VABI sceUsbStorageGetdentsOpen(uint32_t device, int32_t* fd, uint64_t* bufferSize);
int APS5_VABI sceUsbStorageGetdentsRead(int32_t fd, void* buffer, uint64_t bufferSize, uint64_t* bytesRead);
int APS5_VABI sceUsbStorageGetdentsClose(int32_t fd);
int APS5_VABI sceUsbStorageClearFakeWriteProtection();
int APS5_VABI sceUsbStorageRequestMapWSB(uint32_t deviceId, const char* subpath, int32_t flags, int32_t unknown, char* outMountPath, int32_t* outStatus);
int APS5_VABI sceUsbStorageSetFakeMapLockForDebug();
int APS5_VABI sceUsbStorageSetFakeWriteProtection();

}
