#include "prx/libkernel/include/Api.hpp"
#include "prx/libkernel/KernelErrors.hpp"
#include "prx/libkernel/Pthread/include/Cond.hpp"
#include "prx/libkernel/Pthread/include/Mutex.hpp"
#include "prx/libkernel/Time/include/Time.hpp"
#include "SceTypes.hpp"
#include <chrono>
#include <cstdint>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#endif

static void Require(bool value) {
    if (!value) std::abort();
}

#ifdef _WIN32
static std::uint64_t CurrentThreadCpuNanos() {
    FILETIME creation{}, exit{}, kernel{}, user{};
    if (!GetThreadTimes(GetCurrentThread(), &creation, &exit, &kernel, &user)) return 0;
    ULARGE_INTEGER k, u;
    k.LowPart = kernel.dwLowDateTime;
    k.HighPart = kernel.dwHighDateTime;
    u.LowPart = user.dwLowDateTime;
    u.HighPart = user.dwHighDateTime;
    return (k.QuadPart + u.QuadPart) * 100ULL;
}
#endif

static void TestUsleep500us() {
    constexpr int iterations = 50;
#ifdef _WIN32
    const auto cpuStart = CurrentThreadCpuNanos();
#endif
    const auto wallStart = std::chrono::steady_clock::now();

    for (int i = 0; i < iterations; ++i) {
        const auto t0 = std::chrono::steady_clock::now();
        Require(sceKernelUsleep_nid_postfix(500) == 0);
        const auto t1 = std::chrono::steady_clock::now();
        const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);
        Require(elapsed >= std::chrono::microseconds(500));
        Require(elapsed < std::chrono::microseconds(3000));
    }

    const auto wallEnd = std::chrono::steady_clock::now();
#ifdef _WIN32
    const auto cpuEnd = CurrentThreadCpuNanos();
    const auto wallNanos = static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(wallEnd - wallStart).count());
    const auto cpuNanos = cpuEnd - cpuStart;
    Require(cpuNanos <= (wallNanos * 40ULL) / 100ULL);
#endif
}

static void TestCondTimedwait500us() {
    constexpr int iterations = 50;
    PthreadMutex mutex = nullptr;
    PthreadCond cond = nullptr;
    Require(scePthreadMutexInit(&mutex, nullptr, nullptr) == 0);
    Require(scePthreadCondInit(&cond, nullptr, nullptr) == 0);
    Require(scePthreadMutexLock(&mutex) == 0);

#ifdef _WIN32
    const auto cpuStart = CurrentThreadCpuNanos();
#endif
    const auto wallStart = std::chrono::steady_clock::now();

    for (int i = 0; i < iterations; ++i) {
        const auto t0 = std::chrono::steady_clock::now();
        const int result = scePthreadCondTimedwait(&cond, &mutex, 500);
        Require(result == SCE_KERNEL_ERROR_ETIMEDOUT);
        const auto t1 = std::chrono::steady_clock::now();
        const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);
        Require(elapsed >= std::chrono::microseconds(500));
        Require(elapsed < std::chrono::microseconds(3000));
    }

    const auto wallEnd = std::chrono::steady_clock::now();
#ifdef _WIN32
    const auto cpuEnd = CurrentThreadCpuNanos();
    const auto wallNanos = static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(wallEnd - wallStart).count());
    const auto cpuNanos = cpuEnd - cpuStart;
#endif

    Require(scePthreadMutexUnlock(&mutex) == 0);
    Require(scePthreadCondDestroy(&cond) == 0);
    Require(scePthreadMutexDestroy(&mutex) == 0);

#ifdef _WIN32
    Require(cpuNanos <= (wallNanos * 40ULL) / 100ULL);
#endif
}

int main() {
    TestUsleep500us();
    TestCondTimedwait500us();
    return 0;
}

