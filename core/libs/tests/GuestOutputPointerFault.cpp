#include "prx/libc/include/Api.hpp"
#include "prx/libkernel/Time/include/Time.hpp"
#include "prx/libkernel/include/Api.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <initializer_list>

static void Check(bool value, int line) {
    if (!value) {
        std::fprintf(stderr, "output pointer check failed at line %d\n", line);
        std::abort();
    }
}
#define Require(value) Check((value), __LINE__)

static constexpr int GuestEfault = 14;
static constexpr int KernelEfault = static_cast<int>(0x8002000Eu);

static bool Efault(int result) {
    return result == -1 && *__error_nid_postfix() == GuestEfault;
}

int main() {
    alignas(8) unsigned char valid[512]{};
    alignas(8) unsigned char zeroTimer[32]{};
    auto* timeValid = reinterpret_cast<KernelTimeval*>(valid);
    auto* zoneValid = reinterpret_cast<KernelTimezone*>(valid + 32);
    auto* timerValid = reinterpret_cast<GuestItimerval*>(valid);
    auto* zeroTimerValue = reinterpret_cast<const GuestItimerval*>(zeroTimer);
    for (const std::uintptr_t address : {std::uintptr_t{1}, ~std::uintptr_t{0xfff}}) {
        void* bad = reinterpret_cast<void*>(address);
        auto* badTime = reinterpret_cast<KernelTimeval*>(address);
        auto* badZone = reinterpret_cast<KernelTimezone*>(address);
        auto* badTimer = reinterpret_cast<GuestItimerval*>(address);
        *__error_nid_postfix() = 0;
        *__error_nid_postfix() = 0;
        *__error_nid_postfix() = 0;
        Require(Efault(gettimeofday_nid_postfix(badTime, nullptr)));
        *__error_nid_postfix() = 0;
        Require(Efault(gettimeofday_nid_postfix(timeValid, badZone)));
        *__error_nid_postfix() = 0;
        Require(Efault(getitimer_nid_postfix(0, badTimer)));
        *__error_nid_postfix() = 0;
        Require(Efault(setitimer_nid_postfix(0, zeroTimerValue, badTimer)));
        *__error_nid_postfix() = 0;
        Require(Efault(setitimer_nid_postfix(0, badTimer, timerValid)));
        Require(sceKernelGettimeofday(badTime) == KernelEfault);
        Require(sceKernelGettimezone(badZone) == KernelEfault);
    }
    Require(gettimeofday_nid_postfix(timeValid, zoneValid) == 0);
    Require(getitimer_nid_postfix(0, timerValid) == 0);
    Require(setitimer_nid_postfix(0, zeroTimerValue, timerValid) == 0);
    Require(sceKernelGettimeofday(timeValid) == 0);
    Require(sceKernelGettimezone(zoneValid) == 0);
}
