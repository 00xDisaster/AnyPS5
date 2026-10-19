#include "prx/libSceAgcDriver/Execution/include/GuestMemory.hpp"
#include "prx/libSceAgcDriver/Execution/include/VulkanDevice.hpp"
#include "prx/libSceAgcDriver/Graphics/include/Context.hpp"
#include "prx/libSceAgcDriver/Graphics/include/Recorder.hpp"
#include "prx/libc/include/GuestAllocations.hpp"
#include "Recompiler.hpp"
#include "VulkanTestDevice.hpp"
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <mutex>
#include <span>
#include <string>
#include <thread>
#include <vector>

namespace {

using AgcDriver::Graphics::Recorder;
using AgcDriver::Graphics::Require;

constexpr std::uint32_t Threads = 32;
constexpr std::size_t BlockBytes = 65536;
constexpr std::uint32_t RecordBytes = 16;
constexpr std::size_t StoredOffset = 8;
constexpr std::size_t CopiedRecordOffset = 0x1004;
constexpr std::size_t DirectRecordOffset = 0x1000;
constexpr std::uint32_t CpuValue = 0xc0001000u;
constexpr std::uint32_t GpuValue = 1;
constexpr std::uint32_t LabelValue = 1;

alignas(256) constexpr std::array<std::uint32_t, 6> StoreCode{
    0x7e000281u, 0x7e020288u, 0xe0701000u, 0x80000001u, 0xbf810000u, 0xbf810000u,
};

class GuestBlock {
public:
    GuestBlock() {
#ifdef _WIN32
        block = static_cast<std::uint8_t*>(VirtualAlloc(nullptr, BlockBytes, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
#else
        block = static_cast<std::uint8_t*>(std::aligned_alloc(BlockBytes, BlockBytes));
#endif
        Require(block != nullptr, "copied writer label order: cannot allocate a guest block");
        std::memset(block, 0, BlockBytes);
        GuestAllocations::Mutation().Add(block, BlockBytes, true, true, true);
    }

    ~GuestBlock() {
        GuestAllocations::Mutation().Remove(block);
#ifdef _WIN32
        VirtualFree(block, 0, MEM_RELEASE);
#else
        std::free(block);
#endif
    }

    GuestBlock(const GuestBlock&) = delete;
    GuestBlock& operator=(const GuestBlock&) = delete;

    volatile std::uint32_t* Word(std::size_t offset) const { return reinterpret_cast<volatile std::uint32_t*>(block + offset); }
    std::uint64_t Address() const { return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(block)); }

private:
    std::uint8_t* block = nullptr;
};

std::string Hex(std::uint32_t value) {
    char text[16];
    std::snprintf(text, sizeof(text), "0x%08x", value);
    return text;
}

void RecordDispatch(AgcDriver::VulkanDevice& device, std::uint64_t record) {
    std::vector<std::uint32_t> userData(16, 0u);
    const std::array<std::uint32_t, 4> descriptor{static_cast<std::uint32_t>(record), static_cast<std::uint32_t>((record >> 32u) & 0xffffu), RecordBytes, 0x31016facu};
    std::copy(descriptor.begin(), descriptor.end(), userData.begin());
    const std::span<const std::uint32_t> code(StoreCode);
    const std::array<ShaderRecompiler::MemoryRegion, 1> memory{{{reinterpret_cast<std::uintptr_t>(code.data()), std::as_bytes(code)}}};
    const ShaderRecompiler::ShaderComputeStageInfo compute{{Threads, 1, 1}, 0, {false, false, false}, false, 1};
    ShaderRecompiler::RecompileRequest request{
        {ShaderRecompiler::ShaderStage::Compute, reinterpret_cast<std::uintptr_t>(code.data()), code, 0, {}},
        {32, 0, userData, compute, std::nullopt, std::nullopt, memory},
        device.Target(),
        {0, 0, 0, 128}
    };
    request.useCache = false;
    const auto result = ShaderRecompiler::Recompile(request);
    device.Dispatch(result, 1, 1, 1, {}, reinterpret_cast<std::uintptr_t>(code.data()));
}

bool LabelPass(AgcDriver::VulkanDevice& device, const GuestBlock& records, std::size_t recordOffset, const GuestBlock& labels, std::size_t labelOffset, std::uint64_t stamp, std::uint64_t writeBacks, int expectedReason, const std::string& what) {
    volatile std::uint32_t* stored = records.Word(recordOffset + StoredOffset);
    volatile std::uint32_t* label = labels.Word(labelOffset);
    *stored = CpuValue;
    *label = 0;
    RecordDispatch(device, records.Address() + recordOffset);
    const auto pending = Recorder::PendingWriteBackCompletions();
    Require(pending == writeBacks, what + ": " + std::to_string(pending) + " write-back completions are pending after the dispatch, expected " + std::to_string(writeBacks) + " (the test needs APS5_CPU_COPIES=1 and a storage buffer offset alignment above 4)");
    const std::array<std::byte, 4> bytes{std::byte{LabelValue}, std::byte{0}, std::byte{0}, std::byte{0}};
    int reason = 0;
    {
        std::lock_guard gpu(AgcDriver::GuestMemory::GpuMutex());
        reason = device.WriteLabelOnGpu(labels.Address() + labelOffset, bytes, stamp, 0, false);
        if (reason == 3) {
            device.WaitIdle();
            return false;
        }
        Require(reason == expectedReason, what + ": the label after the dispatch was handled with reason " + std::to_string(reason) + ", expected " + std::to_string(expectedReason));
        if (reason >= 1 && reason <= 4) {
            if (reason != 1) device.WaitIdle();
            AgcDriver::GuestMemory::Write(labels.Address() + labelOffset, bytes, 4);
        } else {
            device.SubmitRecorded(true);
        }
    }
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
    while (*label != LabelValue) {
        Require(std::chrono::steady_clock::now() < deadline, what + ": the label never landed");
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
    const std::uint32_t seen = *stored;
    Require(seen == GpuValue, what + ": the label is visible but the record holds " + Hex(seen) + ", the dispatch stored " + Hex(GpuValue));
    *stored = CpuValue;
    device.WaitIdle();
    const std::uint32_t kept = *stored;
    Require(kept == CpuValue, what + ": the record holds " + Hex(kept) + " after the drain, a write-back landed over the value the CPU wrote after the label (" + Hex(CpuValue) + ")");
    Require(Recorder::PendingWriteBackCompletions() == 0, what + ": write-back completions outlived the drain");
    return true;
}

}

int main() {
    try {
#ifdef _WIN32
        _putenv_s("APS5_CPU_COPIES", "1");
#else
        setenv("APS5_CPU_COPIES", "1", 1);
#endif
        const GuestBlock copied;
        const GuestBlock direct;
        const GuestBlock labels;
        const auto device = OpenVulkanTestDevice();
        if (!device) return VulkanTestSkipped;
        if (!LabelPass(*device, copied, CopiedRecordOffset, labels, 0x2000, 1, 1, 2, "copied writer") || !LabelPass(*device, direct, DirectRecordOffset, labels, 0x3000, 2, 0, 0, "direct writer")) {
            std::puts("skipped, the device imports no host memory");
            return VulkanTestSkipped;
        }
        std::puts("copied writer label order tests passed");
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
