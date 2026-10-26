#include "prx/libc/include/SonyCrt.hpp"
#include "prx/libc/include/Api.hpp"
#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>

static void Require(bool value, const char* message) {
    if (!value) {
        std::fprintf(stderr, "%s\n", message);
        std::abort();
    }
}

int main() {
    const auto before = static_cast<std::int64_t>(std::time(nullptr));
    const std::int64_t now = time_nid_postfix(nullptr);
    const auto after = static_cast<std::int64_t>(std::time(nullptr));
    Require(now >= before - 1 && now <= after + 1, "time(NULL) is within a second of the host clock");

    constexpr std::int64_t sentinel = 0x5a5a5a5a5a5a5a5aLL;
    std::int64_t stored = sentinel;
    const std::int64_t returned = time_nid_postfix(&stored);
    Require(stored == returned, "time(&t) stores the value it returns");
    Require(returned >= before - 1 && returned <= static_cast<std::int64_t>(std::time(nullptr)) + 1, "time(&t) is within a second of the host clock");

    std::int64_t viaAlias = sentinel;
    const std::int64_t aliasReturned = libc_time_nid_postfix(&viaAlias);
    Require(viaAlias == aliasReturned, "libc_time(&t) stores the value it returns");
    Require(aliasReturned >= before - 1 && aliasReturned <= static_cast<std::int64_t>(std::time(nullptr)) + 1, "libc_time(&t) is within a second of the host clock");
    return 0;
}
