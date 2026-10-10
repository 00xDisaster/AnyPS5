#include "SceTypes.hpp"
#include <atomic>
#include <cstdint>
#include <cstdlib>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

extern "C" {
int APS5_VABI scePthreadCreate(Pthread* thread, const PthreadAttr* attr, PthreadEntry entry, void* arg, const char* name);
int APS5_VABI scePthreadJoin(Pthread thread, void** retval);
int APS5_VABI sceKernelIsStack(void* addr, void** start, void** end);
int APS5_VABI mprotect_nid_postfix(void* address, std::size_t length, int protection) noexcept;
}

static constexpr int SCE_OK = 0;
static constexpr std::uintptr_t Page = 0x4000;
static constexpr std::size_t GuardBytes = 0x10000;

static void Require(bool value) { if (!value) std::abort(); }

static void* APS5_VABI Worker(void* arg) {
    auto& passed = *static_cast<std::atomic<bool>*>(arg);
    int local = 0;
    void* start = nullptr;
    void* end = nullptr;
    Require(sceKernelIsStack(&local, &start, &end) == SCE_OK);
    const auto low = (reinterpret_cast<std::uintptr_t>(start) + Page - 1) & ~(Page - 1);
    Require(low + GuardBytes + Page < reinterpret_cast<std::uintptr_t>(&local));
    auto* guard = reinterpret_cast<volatile unsigned char*>(low);
    Require(mprotect_nid_postfix(const_cast<unsigned char*>(guard), GuardBytes, 0) == 0);
#ifdef _WIN32
    MEMORY_BASIC_INFORMATION memory{};
    Require(VirtualQuery(const_cast<unsigned char*>(guard), &memory, sizeof(memory)) == sizeof(memory));
    Require(memory.Protect == PAGE_NOACCESS && memory.RegionSize >= GuardBytes);
#endif
    Require(mprotect_nid_postfix(const_cast<unsigned char*>(guard), GuardBytes, 3) == 0);
    guard[0] = 1;
    guard[GuardBytes - 1] = 2;
    Require(guard[0] == 1 && guard[GuardBytes - 1] == 2);
    const auto outside = reinterpret_cast<std::uintptr_t>(start) & ~(Page - 1);
    if (outside >= Page) Require(mprotect_nid_postfix(reinterpret_cast<void*>(outside - Page), 2 * Page, 3) == -1);
    passed.store(true);
    return nullptr;
}

int main() {
    std::atomic<bool> passed{false};
    Pthread thread = nullptr;
    Require(scePthreadCreate(&thread, nullptr, Worker, &passed, nullptr) == SCE_OK);
    Require(scePthreadJoin(thread, nullptr) == SCE_OK);
    Require(passed.load());
}
