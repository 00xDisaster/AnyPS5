#pragma once

#include "prx/libc/include/general/VabiMacros.hpp"
#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

struct GuestJSString {
    std::atomic<std::uint32_t> references{1};
    std::vector<std::uint16_t> characters;
    bool nullString = false;
};

template<typename Unit> constexpr bool GuestJSStringValidLength(std::size_t length) {
    constexpr std::size_t HeaderSize = 24;
    return length <= std::min<std::size_t>(std::numeric_limits<std::int32_t>::max(),
        (std::numeric_limits<std::uint32_t>::max() - HeaderSize) / sizeof(Unit));
}

extern "C" {

GuestJSString* APS5_VABI JSStringCreateWithCharacters(const std::uint16_t* characters, std::size_t count);
GuestJSString* APS5_VABI JSStringCreateWithUTF8CString(const char* text);
GuestJSString* APS5_VABI JSStringRetain(GuestJSString* string);
void APS5_VABI JSStringRelease(GuestJSString* string);
std::size_t APS5_VABI JSStringGetLength(const GuestJSString* string);
const std::uint16_t* APS5_VABI JSStringGetCharactersPtr(const GuestJSString* string);
std::size_t APS5_VABI JSStringGetMaximumUTF8CStringSize(const GuestJSString* string);
std::size_t APS5_VABI JSStringGetUTF8CString(const GuestJSString* string, char* buffer, std::size_t bufferSize);
bool APS5_VABI JSStringIsEqual(const GuestJSString* first, const GuestJSString* second);
bool APS5_VABI JSStringIsEqualToUTF8CString(const GuestJSString* string, const char* text);

}
