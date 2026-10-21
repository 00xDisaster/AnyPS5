#include <cstdint>
#include <cstddef>
#include <cstring>
#include <atomic>
#include <mutex>
#include <unordered_set>
#include <unordered_map>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"

namespace {

constexpr int SCE_ACM_ERROR_BAD_CONTEXT = static_cast<int>(0x81940002);
constexpr int SCE_ACM_ERROR_BAD_POINTER = static_cast<int>(0x81940006);

std::mutex g_mutex;
std::unordered_set<AcmContextId> g_contexts;
std::atomic<uint32_t> g_nextId{1};

}

extern "C" {

int APS5_VABI sceAcmBatchStartBuffer(AcmContextId context, const void* batch_commands, size_t batch_size, AcmBatchError* batch_error, AcmBatchId* batch) {
    (void)batch_commands;
    (void)batch_size;
    if (!batch) return SCE_ACM_ERROR_BAD_POINTER;
    if (context == 0) return SCE_ACM_ERROR_BAD_CONTEXT;
    std::lock_guard<std::mutex> lock(g_mutex);
    if (!g_contexts.contains(context)) return SCE_ACM_ERROR_BAD_CONTEXT;
    AcmBatchId id = g_nextId.fetch_add(1, std::memory_order_relaxed);
    while (id == 0) id = g_nextId.fetch_add(1, std::memory_order_relaxed);
    if (batch_error) std::memset(batch_error, 0, sizeof(*batch_error));
    *batch = id;
    return 0;
}

int APS5_VABI sceAcmBatchStartBuffers(AcmContextId context, uint32_t batch_info_count, const AcmBatchInfo* const batch_info[], AcmBatchError* batch_error, AcmBatchId* batch) {
    (void)batch_info_count;
    (void)batch_info;
    if (!batch) return SCE_ACM_ERROR_BAD_POINTER;
    if (context == 0) return SCE_ACM_ERROR_BAD_CONTEXT;
    std::lock_guard<std::mutex> lock(g_mutex);
    if (!g_contexts.contains(context)) return SCE_ACM_ERROR_BAD_CONTEXT;
    AcmBatchId id = g_nextId.fetch_add(1, std::memory_order_relaxed);
    while (id == 0) id = g_nextId.fetch_add(1, std::memory_order_relaxed);
    if (batch_error) std::memset(batch_error, 0, sizeof(*batch_error));
    *batch = id;
    return 0;
}

int APS5_VABI sceAcmBatchWait(AcmContextId context, AcmBatchId batch, uint32_t timeout) {
    (void)batch;
    (void)timeout;
    if (context == 0) return SCE_ACM_ERROR_BAD_CONTEXT;
    std::lock_guard<std::mutex> lock(g_mutex);
    if (!g_contexts.contains(context)) return SCE_ACM_ERROR_BAD_CONTEXT;
    return 0;
}

int APS5_VABI sceAcmContextCreate(AcmContextId* context) {
    if (!context) return SCE_ACM_ERROR_BAD_POINTER;
    AcmContextId id = g_nextId.fetch_add(1, std::memory_order_relaxed);
    while (id == 0) id = g_nextId.fetch_add(1, std::memory_order_relaxed);
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_contexts.insert(id);
    }
    *context = id;
    return 0;
}

int APS5_VABI sceAcmContextDestroy(AcmContextId context) {
    if (context == 0) return SCE_ACM_ERROR_BAD_CONTEXT;
    std::lock_guard<std::mutex> lock(g_mutex);
    auto it = g_contexts.find(context);
    if (it == g_contexts.end()) return SCE_ACM_ERROR_BAD_CONTEXT;
    g_contexts.erase(it);
    return 0;
}

int APS5_VABI sceAcm_ConvReverb_SharedInput(void) {
    return 0;
}

int APS5_VABI sceAcm_FFT(void) {
    return 0;
}

int APS5_VABI sceAcm_Panner(AcmBatchInfo* batch, std::uint32_t sourceCount, const void* const* sources, std::uint32_t sourceMode, std::uint32_t outputMode, std::uint32_t channelCount, const void* const* sourceParameters, const void* const* sourceMatrices, const void* const* inputChannels, void* const* outputChannels) {
    if (!batch || sourceCount == 0 || sourceCount > 65535 || sourceMode > 2 || outputMode > 1 || channelCount == 0 || channelCount > 36 || !sources || !sourceParameters || !sourceMatrices || !outputChannels) return SCE_ACM_ERROR_BAD_POINTER;
    std::uintptr_t alignment = reinterpret_cast<std::uintptr_t>(sources) | reinterpret_cast<std::uintptr_t>(sourceParameters) | reinterpret_cast<std::uintptr_t>(sourceMatrices) | reinterpret_cast<std::uintptr_t>(inputChannels) | reinterpret_cast<std::uintptr_t>(outputChannels);
    for (std::uint32_t index = 0; index < sourceCount; ++index) {
        if (!sources[index] || !sourceParameters[index] || !sourceMatrices[index]) return SCE_ACM_ERROR_BAD_POINTER;
        std::uint32_t parameterMode;
        std::memcpy(&parameterMode, sourceParameters[index], sizeof(parameterMode));
        if (parameterMode > 1) return SCE_ACM_ERROR_BAD_POINTER;
        alignment |= reinterpret_cast<std::uintptr_t>(sources[index]) | reinterpret_cast<std::uintptr_t>(sourceParameters[index]) | reinterpret_cast<std::uintptr_t>(sourceMatrices[index]);
    }
    for (std::uint32_t index = 0; index < channelCount; ++index) {
        if (!outputChannels[index]) return SCE_ACM_ERROR_BAD_POINTER;
        alignment |= reinterpret_cast<std::uintptr_t>(outputChannels[index]);
        if (inputChannels) alignment |= reinterpret_cast<std::uintptr_t>(inputChannels[index]);
    }
    if (alignment & 127) return static_cast<int>(0x8194000A);
    const auto offset = batch->offset;
    const auto capacity = batch->buffer_size;
    if (offset > capacity || capacity - offset < 8) return static_cast<int>(0x81940009);
    auto* buffer = static_cast<std::byte*>(batch->buffer);
    const auto writeWord = [buffer](std::size_t position, std::uint64_t value) { std::memcpy(buffer + position, &value, sizeof(value)); };
    const auto encodedCount = sourceCount < 255 ? sourceCount : 255;
    writeWord(offset, (static_cast<std::uint64_t>(channelCount) << 24) | (static_cast<std::uint64_t>(encodedCount) << 16) | (std::uint64_t{1} << (sourceMode + 32)) | (std::uint64_t{1} << (outputMode + 35)) | 0x300);
    if (capacity < 128 || capacity - offset < 152) return static_cast<int>(0x81940009);
    const auto descriptorOffset = capacity - 128;
    std::memset(buffer + descriptorOffset, 0, 128);
    writeWord(descriptorOffset, sourceCount);
    writeWord(descriptorOffset + 8, reinterpret_cast<std::uintptr_t>(inputChannels));
    writeWord(descriptorOffset + 16, reinterpret_cast<std::uintptr_t>(outputChannels));
    writeWord(descriptorOffset + 24, reinterpret_cast<std::uintptr_t>(sources));
    writeWord(descriptorOffset + 32, sourceMode | (static_cast<std::uint64_t>(outputMode) << 32));
    writeWord(descriptorOffset + 40, channelCount);
    writeWord(descriptorOffset + 48, reinterpret_cast<std::uintptr_t>(sourceParameters));
    writeWord(descriptorOffset + 56, reinterpret_cast<std::uintptr_t>(sourceMatrices));
    writeWord(offset + 8, 0x8000000000000D);
    writeWord(offset + 16, reinterpret_cast<std::uintptr_t>(buffer + descriptorOffset));
    if (capacity - offset < 160) return static_cast<int>(0x81940009);
    writeWord(offset + 24, 10);
    batch->offset = offset + 32;
    batch->buffer_size = descriptorOffset;
    return 0;
}

int APS5_VABI sceAcmBatchJobPriority(AcmBatchInfo* batch, std::uint32_t priority) {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceAcm_ConvReverb_SharedIr() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceAcm_IFFT() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}


int APS5_VABI sceAcmBatchJobNotification() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

}
