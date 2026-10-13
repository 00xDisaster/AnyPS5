#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdint>
#include <cstdlib>

extern "C" {
int APS5_VABI sigaction_nid_postfix(int, const void*, void*);
int APS5_VABI sigprocmask_nid_postfix(int, const void*, void*);
int APS5_VABI raise_nid_postfix(int);
}

struct GuestSignalSet {
    std::uint32_t bits[4];
};

struct GuestSigaction {
    std::uintptr_t handler;
    int flags;
    GuestSignalSet mask;
};

static constexpr int SigTerm = 15;
static constexpr int SigUsr1 = 30;
static constexpr int SigUsr2 = 31;
static constexpr int SaRestart = 0x2;
static constexpr int SaResethand = 0x4;
static constexpr int SaNodefer = 0x10;
static constexpr int SigBlock = 1;

static std::uint32_t Bit(int signal) { return 1u << (signal - 1); }

static void Require(bool value) { if (!value) std::abort(); }

static int calls = 0;
static int callsAfterNestedRaise = 0;
static std::uint32_t maskInHandler = 0;

static std::uint32_t CurrentMask() {
    GuestSignalSet set{};
    Require(sigprocmask_nid_postfix(SigBlock, nullptr, &set) == 0);
    return set.bits[0];
}

static void APS5_VABI RaiseAgain(int signal) {
    if (++calls == 1) {
        maskInHandler = CurrentMask();
        Require(raise_nid_postfix(signal) == 0);
        callsAfterNestedRaise = calls;
    }
}

static void APS5_VABI Count(int) {
    ++calls;
}

static void Install(int signal, void (APS5_VABI* handler)(int), int flags, std::uint32_t mask) {
    GuestSigaction action{reinterpret_cast<std::uintptr_t>(handler), flags, {{mask, 0, 0, 0}}};
    Require(sigaction_nid_postfix(signal, &action, nullptr) == 0);
}

static void CheckDeferred(int signal) {
    calls = 0;
    callsAfterNestedRaise = 0;
    Install(signal, RaiseAgain, SaRestart, Bit(SigUsr1));
    Require(raise_nid_postfix(signal) == 0);
    Require(callsAfterNestedRaise == 1);
    Require(calls == 2);
    Require(maskInHandler == (Bit(signal) | Bit(SigUsr1)));
    Require(CurrentMask() == 0);
}

static void CheckNodefer() {
    calls = 0;
    callsAfterNestedRaise = 0;
    Install(SigUsr2, RaiseAgain, SaRestart | SaNodefer, 0);
    Require(raise_nid_postfix(SigUsr2) == 0);
    Require(callsAfterNestedRaise == 2);
    Require(maskInHandler == 0);
    Require(CurrentMask() == 0);
}

static void CheckResethand() {
    calls = 0;
    Install(SigUsr1, Count, SaResethand, 0);
    Require(raise_nid_postfix(SigUsr1) == 0);
    Require(calls == 1);
    GuestSigaction current{};
    Require(sigaction_nid_postfix(SigUsr1, nullptr, &current) == 0);
    Require(current.handler == 0);
    Require((current.flags & SaResethand) != 0);
}

int main() {
    CheckDeferred(SigUsr2);
    CheckDeferred(SigTerm);
    CheckNodefer();
    CheckResethand();
    return 0;
}
