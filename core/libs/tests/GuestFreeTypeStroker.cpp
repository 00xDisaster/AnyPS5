#include "prx/libSceFreeType/include/GuestObjects.hpp"
#include "prx/libc/include/general/VabiMacros.hpp"

#include FT_STROKER_H

#include <cstdint>
#include <cstdio>
#include <cstdlib>

using namespace FreeType;

extern "C" {
int APS5_VABI FT_Stroker_New_nid_postfix(GuestLibrary*, void**);
void APS5_VABI FT_Stroker_Done_nid_postfix(void*);
}

static void Check(bool value, int line) {
    if (!value) {
        std::fprintf(stderr, "FreeType stroker check failed at line %d\n", line);
        std::abort();
    }
}
#define Require(value) Check((value), __LINE__)

static void TestNew(GuestLibrary& library, FT_Library native) {
    void* guest = reinterpret_cast<void*>(1);
    FT_Stroker expected = nullptr;
    Require(FT_Stroker_New_nid_postfix(nullptr, &guest) == FT_Stroker_New(nullptr, &expected));
    Require(guest == nullptr && expected == nullptr);
    Require(FT_Stroker_New_nid_postfix(&library, nullptr) == FT_Stroker_New(native, nullptr));
    Require(FT_Stroker_New_nid_postfix(&library, &guest) == FT_Stroker_New(native, &expected));
    Require(guest != nullptr && expected != nullptr);
    FT_Stroker_Done(static_cast<FT_Stroker>(guest));
    FT_Stroker_Done(expected);
}

static void TestDone(GuestLibrary& library, FT_Library native) {
    for (int round = 0; round < 3; ++round) {
        void* guest = nullptr;
        FT_Stroker expected = nullptr;
        Require(FT_Stroker_New_nid_postfix(&library, &guest) == 0 && FT_Stroker_New(native, &expected) == 0);
        FT_Stroker_Done_nid_postfix(guest);
        FT_Stroker_Done(expected);
    }
    FT_Stroker_Done_nid_postfix(nullptr);
    FT_Stroker_Done(nullptr);
}

int main() {
    GuestLibrary library{};
    FT_Library native = nullptr;
    Require(FT_Init_FreeType(&library.host) == 0 && FT_Init_FreeType(&native) == 0);
    TestNew(library, native);
    TestDone(library, native);
    Require(FT_Done_FreeType(library.host) == 0 && FT_Done_FreeType(native) == 0);
}
