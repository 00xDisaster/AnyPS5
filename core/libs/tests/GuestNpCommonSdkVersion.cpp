#include "prx/libSceNpCommon/include/Api.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>

static void Require(bool value) {
    if (value) return;
    std::fputs("NpCommon SDK version check failed\n", stderr);
    std::abort();
}

int main() {
    char version[8] = {};
    sceNpGetSdkVersion(version);
    Require(version[0] >= '0' && version[0] <= '9');
    Require(version[1] == '.');
    Require((version[2] >= '0' && version[2] <= '9') && (version[3] >= '0' && version[3] <= '9'));
    Require(version[4] == '\0');

    std::uint32_t numeric = 0xFFFFFFFFu;
    sceNpGetSdkVersionUInt(&numeric);
    Require(numeric == 0x01000000u);

    sceNpGetSdkVersion(nullptr);
    sceNpGetSdkVersionUInt(nullptr);

    return 0;
}
