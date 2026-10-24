#include "prx/libSceFreeType/include/GuestObjects.hpp"
#include "prx/libc/include/GuestArena.hpp"
#include "prx/libc/include/general/VabiMacros.hpp"

#include FT_STROKER_H

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>

using namespace FreeType;

namespace {

FT_Pos HostCoordinate(std::int64_t value, const char* function) {
    if (value < std::numeric_limits<FT_Pos>::min() || value > std::numeric_limits<FT_Pos>::max()) {
        throw std::runtime_error(std::string(function) + ": a value outside the host FT_Pos range is not implemented");
    }
    return static_cast<FT_Pos>(value);
}

struct NativeVector {
    FT_Vector value{};
    FT_Vector* pointer = nullptr;

    NativeVector(const GuestVector* guest, const char* function) {
        if (!guest) return;
        value = {HostCoordinate(guest->x, function), HostCoordinate(guest->y, function)};
        pointer = &value;
    }
};

void WriteCount(std::uint32_t* target, FT_UInt value) {
    if (!target) return;
    GuestArena::HostWrite write(target, sizeof(*target));
    *target = value;
}

}

extern "C" {

int APS5_VABI FT_Stroker_New_nid_postfix(GuestLibrary* library, void** stroker) {
    FT_Stroker native = nullptr;
    const FT_Error error = FT_Stroker_New(library ? library->host : nullptr, stroker ? &native : nullptr);
    if (stroker) *stroker = native;
    return error;
}

void APS5_VABI FT_Stroker_Done_nid_postfix(void* stroker) {
    FT_Stroker_Done(static_cast<FT_Stroker>(stroker));
}

int APS5_VABI FT_Stroker_GetCounts_nid_postfix(void* stroker, std::uint32_t* points, std::uint32_t* contours) {
    FT_UInt nativePoints = 0;
    FT_UInt nativeContours = 0;
    const FT_Error error = FT_Stroker_GetCounts(static_cast<FT_Stroker>(stroker), points ? &nativePoints : nullptr, contours ? &nativeContours : nullptr);
    WriteCount(points, nativePoints);
    WriteCount(contours, nativeContours);
    return error;
}

}
