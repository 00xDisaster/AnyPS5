#include "prx/libSceNpCommon/include/Api.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>

static void Require(bool value) {
    if (value) return;
    std::fputs("NpCommon system clock check failed\n", stderr);
    std::abort();
}

int main() {
    Require(sceNpGetSystemClockUsec(nullptr) == NpInvalidArgument, "a null destination reports an invalid argument");

    std::int64_t usec = 0;
    const int result = sceNpGetSystemClockUsec(&usec);
    Require(result == 0 || result != 0, "the clock query either succeeds or propagates the kernel result");
    if (result == 0) Require(usec >= 0, "a successful clock query produces a non-negative microsecond count");

    return 0;
}
