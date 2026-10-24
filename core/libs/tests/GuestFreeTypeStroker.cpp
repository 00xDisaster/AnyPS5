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
int APS5_VABI FT_Stroker_GetCounts_nid_postfix(void*, std::uint32_t*, std::uint32_t*);
int APS5_VABI FT_Stroker_BeginSubPath_nid_postfix(void*, const GuestVector*, std::uint8_t);
int APS5_VABI FT_Stroker_LineTo_nid_postfix(void*, const GuestVector*);
int APS5_VABI FT_Stroker_EndSubPath_nid_postfix(void*);
}

static void Check(bool value, int line) {
    if (!value) {
        std::fprintf(stderr, "FreeType stroker check failed at line %d\n", line);
        std::abort();
    }
}
#define Require(value) Check((value), __LINE__)

struct Pair {
    void* guest = nullptr;
    FT_Stroker native = nullptr;

    Pair(GuestLibrary& library, FT_Library nativeLibrary) {
        Require(FT_Stroker_New_nid_postfix(&library, &guest) == 0);
        Require(FT_Stroker_New(nativeLibrary, &native) == 0);
        FT_Stroker_Set(static_cast<FT_Stroker>(guest), 64, FT_STROKER_LINECAP_BUTT, FT_STROKER_LINEJOIN_ROUND, 0);
        FT_Stroker_Set(native, 64, FT_STROKER_LINECAP_BUTT, FT_STROKER_LINEJOIN_ROUND, 0);
    }

    ~Pair() {
        FT_Stroker_Done_nid_postfix(guest);
        FT_Stroker_Done(native);
    }

    void CountsMatch() const {
        std::uint32_t points = 11;
        std::uint32_t contours = 13;
        FT_UInt nativePoints = 11;
        FT_UInt nativeContours = 13;
        Require(FT_Stroker_GetCounts_nid_postfix(guest, &points, &contours) == FT_Stroker_GetCounts(native, &nativePoints, &nativeContours));
        Require(points == nativePoints && contours == nativeContours);
    }

    void Square(std::uint8_t open, bool finish = true) const {
        const GuestVector corners[] = {{0, 0}, {640, 0}, {640, 640}, {0, 640}};
        FT_Vector nativeStart{0, 0};
        Require(FT_Stroker_BeginSubPath_nid_postfix(guest, &corners[0], open) == FT_Stroker_BeginSubPath(native, &nativeStart, open));
        for (int index = 1; index < 4; ++index) {
            FT_Vector nativePoint{static_cast<FT_Pos>(corners[index].x), static_cast<FT_Pos>(corners[index].y)};
            Require(FT_Stroker_LineTo_nid_postfix(guest, &corners[index]) == FT_Stroker_LineTo(native, &nativePoint));
        }
        if (!finish) return;
        Require(FT_Stroker_EndSubPath_nid_postfix(guest) == FT_Stroker_EndSubPath(native));
    }
};

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

static void TestGetCounts(GuestLibrary& library, FT_Library native) {
    Pair pair(library, native);
    pair.CountsMatch();
    std::uint32_t points = 7;
    std::uint32_t contours = 9;
    FT_UInt nativePoints = 7;
    FT_UInt nativeContours = 9;
    Require(FT_Stroker_GetCounts_nid_postfix(nullptr, &points, &contours) == FT_Stroker_GetCounts(nullptr, &nativePoints, &nativeContours));
    Require(points == nativePoints && contours == nativeContours);
    Require(FT_Stroker_GetCounts_nid_postfix(pair.guest, nullptr, nullptr) == FT_Stroker_GetCounts(pair.native, nullptr, nullptr));
}

static void TestBeginSubPath(GuestLibrary& library, FT_Library native) {
    for (const std::uint8_t open : {0, 1}) {
        Pair pair(library, native);
        const GuestVector start{64, 128};
        FT_Vector nativeStart{64, 128};
        Require(FT_Stroker_BeginSubPath_nid_postfix(pair.guest, &start, open) == FT_Stroker_BeginSubPath(pair.native, &nativeStart, open));
        pair.CountsMatch();
    }
    Pair pair(library, native);
    const GuestVector start{0, 0};
    FT_Vector nativeStart{0, 0};
    Require(FT_Stroker_BeginSubPath_nid_postfix(nullptr, &start, 0) == FT_Stroker_BeginSubPath(nullptr, &nativeStart, 0));
    Require(FT_Stroker_BeginSubPath_nid_postfix(pair.guest, nullptr, 0) == FT_Stroker_BeginSubPath(pair.native, nullptr, 0));
}

static void TestLineTo(GuestLibrary& library, FT_Library native) {
    Pair pair(library, native);
    const GuestVector start{0, 0};
    FT_Vector nativeStart{0, 0};
    Require(FT_Stroker_BeginSubPath_nid_postfix(pair.guest, &start, 1) == FT_Stroker_BeginSubPath(pair.native, &nativeStart, 1));
    for (const GuestVector point : {GuestVector{640, 0}, GuestVector{640, 640}, GuestVector{0, 640}}) {
        FT_Vector nativePoint{static_cast<FT_Pos>(point.x), static_cast<FT_Pos>(point.y)};
        Require(FT_Stroker_LineTo_nid_postfix(pair.guest, &point) == FT_Stroker_LineTo(pair.native, &nativePoint));
        pair.CountsMatch();
    }
    const GuestVector point{1, 1};
    FT_Vector nativePoint{1, 1};
    Require(FT_Stroker_LineTo_nid_postfix(nullptr, &point) == FT_Stroker_LineTo(nullptr, &nativePoint));
    Require(FT_Stroker_LineTo_nid_postfix(pair.guest, nullptr) == FT_Stroker_LineTo(pair.native, nullptr));
}

static void TestEndSubPath(GuestLibrary& library, FT_Library native) {
    for (const std::uint8_t open : {0, 1}) {
        Pair pair(library, native);
        pair.Square(open, false);
        Require(FT_Stroker_EndSubPath_nid_postfix(pair.guest) == FT_Stroker_EndSubPath(pair.native));
        pair.CountsMatch();
    }
    Require(FT_Stroker_EndSubPath_nid_postfix(nullptr) == FT_Stroker_EndSubPath(nullptr));
}

int main() {
    GuestLibrary library{};
    FT_Library native = nullptr;
    Require(FT_Init_FreeType(&library.host) == 0 && FT_Init_FreeType(&native) == 0);
    TestNew(library, native);
    TestDone(library, native);
    TestGetCounts(library, native);
    TestBeginSubPath(library, native);
    TestLineTo(library, native);
    TestEndSubPath(library, native);
    Require(FT_Done_FreeType(library.host) == 0 && FT_Done_FreeType(native) == 0);
}
