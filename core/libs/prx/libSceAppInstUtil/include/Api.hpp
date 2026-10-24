#pragma once

#include "SceTypes.hpp"

struct AppInstallProgressInfo;

extern "C" {

int APS5_VABI sceAppInstUtilInitialize(void);
int APS5_VABI sceAppInstUtilTerminate(void);
int APS5_VABI sceAppInstUtilAppInstallAll(void* reserved);
int APS5_VABI sceAppInstUtilAppInstallTitleDir(const char* titleId, const char* titleDir, void* reserved);
int APS5_VABI sceAppInstUtilAppIsInInstalling(int32_t* installing);
int APS5_VABI sceAppInstUtilAppIsInUpdating(const char* titleId, bool* updating);
int APS5_VABI sceAppInstUtilAppExists(const char* titleId, int32_t* exists);
int APS5_VABI sceAppInstUtilAppGetSize(const char* titleId, std::uint64_t* size);
int APS5_VABI sceAppInstUtilAppInstallPkg(const char* packagePath, std::uint64_t serviceArgument);
int APS5_VABI sceAppInstUtilAppPrepareOverwritePkg();
int APS5_VABI sceAppInstUtilAppRecoverApp();
int APS5_VABI sceAppInstUtilAppUnInstall(const char* titleId);
int APS5_VABI sceAppInstUtilAppUnInstallAddcont(const char* titleId, const char* contentLabel);
int APS5_VABI sceAppInstUtilAppUnInstallPat(const char* titleId);
int APS5_VABI sceAppInstUtilAppUnInstallTheme();
int APS5_VABI sceAppInstUtilAppUnInstallTypes(const char* titleId, std::uint32_t types);
int APS5_VABI sceAppInstUtilGetInstallProgressInfo(const char* titleId, AppInstallProgressInfo* info);
int APS5_VABI sceAppInstUtilGetInstallProgress(const char* titleId, void* progress);
int APS5_VABI sceAppInstUtilGetPrimaryAppSlot(const char* titleId, void* slot);

}
