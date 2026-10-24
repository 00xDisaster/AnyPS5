#pragma once

#include "SceTypes.hpp"

extern "C" {

int APS5_VABI sceNpBandwidthTestAbort(int contextId);
int APS5_VABI sceNpBandwidthTestGetStatus(int contextId, int* status);
int APS5_VABI sceNpBandwidthTestInitStartDownload(const void* param);
int APS5_VABI sceNpBandwidthTestInitStartUpload(const void* param);
int APS5_VABI sceNpBandwidthTestShutdown(int contextId, void* result);
int APS5_VABI sceNpBandwidthTestInitStart();
int APS5_VABI sceNpLookupCreateRequest(int titleCtxId);
int APS5_VABI sceNpLookupCreateTitleCtx(const void* selfNpId);
int APS5_VABI sceNpLookupDeleteRequest(int reqId);
int APS5_VABI sceNpLookupDeleteTitleCtx(int titleCtxId);
int APS5_VABI sceNpLookupNpId(int reqId, const NpOnlineId* onlineId, void* npId, void* option);
int APS5_VABI sceNpWordFilterAbortRequest(int reqId);
int APS5_VABI sceNpWordFilterCensorComment(int reqId, const char* comment, void* option);
int APS5_VABI sceNpWordFilterCreateAsyncRequest(int titleCtxId, const std::uint64_t* param);
int APS5_VABI sceNpWordFilterCreateRequest(int titleCtxId);
int APS5_VABI sceNpWordFilterCreateTitleCtx(const void* selfNpId);
int APS5_VABI sceNpWordFilterDeleteRequest(int reqId);
int APS5_VABI sceNpWordFilterDeleteTitleCtx(int titleCtxId);
int APS5_VABI sceNpWordFilterPollAsync(int reqId, int* result);
int APS5_VABI sceNpWordFilterSanitizeComment(int reqId, const char* comment, char* sanitizedComment, void* option);

}
