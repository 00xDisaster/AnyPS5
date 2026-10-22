#include "prx/libc/include/general/VabiMacros.hpp"
#include "SceTypes.hpp"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>

extern "C" {
int APS5_VABI sceNpMatching2Initialize(const void* param);
int APS5_VABI sceNpMatching2Terminate(void);
int APS5_VABI sceNpSignalingDeactivateConnection(void);
}

namespace {

constexpr int Matching2AlreadyInitialized = static_cast<int>(0x80550C02u);
constexpr int Matching2NotInitialized = static_cast<int>(0x80550C03u);
constexpr int Matching2InvalidArgument = static_cast<int>(0x80550C0Au);

void Require(bool condition, const char* message) {
    if (!condition) {
        std::fprintf(stderr, "NpPs4Imports: %s\n", message);
        std::abort();
    }
}

template <typename TCall>
void RequireNotImplemented(TCall call, const std::string& name) {
    try {
        call();
    } catch (const std::runtime_error& error) {
        Require(std::string(error.what()) == name + " not implemented", "a declared function threw another error");
        return;
    }
    Require(false, "a declared function returned");
}

void CheckMatching2() {
    unsigned char param[0x30]{};
    const auto setSize = [&](std::uint64_t size) { std::memcpy(param + 0x20, &size, sizeof(size)); };
    Require(sceNpMatching2Terminate() == Matching2NotInitialized, "NpMatching2 terminated before initialization");
    Require(sceNpMatching2Initialize(nullptr) == Matching2InvalidArgument, "a null NpMatching2 parameter was accepted");
    setSize(0x20u);
    Require(sceNpMatching2Initialize(param) == Matching2InvalidArgument, "an NpMatching2 parameter of size 0x20 was accepted");
    setSize(0x28u);
    Require(sceNpMatching2Initialize(param) == 0, "an NpMatching2 parameter of size 0x28 was refused");
    Require(sceNpMatching2Initialize(param) == Matching2AlreadyInitialized, "NpMatching2 initialized twice");
    Require(sceNpMatching2Terminate() == 0, "NpMatching2 did not terminate");
    setSize(0x30u);
    Require(sceNpMatching2Initialize(param) == 0, "an NpMatching2 parameter with an SSL pool was refused");
    Require(sceNpMatching2Terminate() == 0, "NpMatching2 did not terminate after the SSL form");
}

void CheckSignaling() {
    RequireNotImplemented([] { sceNpSignalingDeactivateConnection(); }, "sceNpSignalingDeactivateConnection");
}

}

int main() {
    CheckMatching2();
    CheckSignaling();
    return 0;
}
