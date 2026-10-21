#ifndef CORE_LIBS_PRX_LIBSCECOMMONDIALOG_COMMONDIALOG_HPP
#define CORE_LIBS_PRX_LIBSCECOMMONDIALOG_COMMONDIALOG_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>

struct CommonDialogBaseParam {
    std::uint64_t size;
    std::uint8_t reserved[36];
    std::uint32_t magic;
};

struct G2PDialogParam {
    CommonDialogBaseParam base;
    std::int32_t userId;
    std::uint32_t padding;
    std::uint64_t size;
    std::uint8_t reserved[64];
};

struct PlayerReviewDialogParam {
    CommonDialogBaseParam base;
    std::uint64_t size;
    std::int32_t userId;
    std::uint8_t target[40];
    std::uint32_t mode;
    std::uint8_t reserved[64];
};

struct NpCommerceDialogParam {
    CommonDialogBaseParam base;
    std::uint32_t size;
    std::int32_t userId;
    std::uint32_t mode;
    std::uint32_t option;
    const char* const* products;
    std::uint32_t productCount;
    std::uint32_t padding;
    std::uint64_t argument;
    std::uint64_t userData;
    std::uint8_t reserved[32];
};

static_assert(sizeof(NpCommerceDialogParam) == 128);
static_assert(offsetof(NpCommerceDialogParam, products) == 64);
static_assert(offsetof(NpCommerceDialogParam, userData) == 88);

struct NpCommerceDialogParam2 {
    CommonDialogBaseParam base;
    std::uint32_t size;
    std::int32_t userId;
    std::uint32_t mode;
    std::uint32_t option;
    std::uint32_t contentMode;
    std::uint32_t padding0;
    const char* const* products;
    std::uint32_t productCount;
    std::uint32_t padding1;
    std::uint64_t argument;
    std::uint64_t userData;
    std::uint8_t reserved[32];
};

static_assert(sizeof(NpCommerceDialogParam2) == 136);
static_assert(offsetof(NpCommerceDialogParam2, products) == 72);
static_assert(offsetof(NpCommerceDialogParam2, userData) == 96);

struct WebBrowserDialogExtendedResult {
    std::uint64_t size;
    std::uint32_t type;
    char content[2048];
    std::uint8_t reserved0[4];
    char* extendedContent;
    std::uint32_t extendedCapacity;
    std::uint8_t reserved1[20];
};

struct WebBrowserDialogResult {
    std::int32_t result;
    std::uint32_t padding;
    WebBrowserDialogExtendedResult* extended;
    std::uint8_t reserved[240];
};

static_assert(sizeof(WebBrowserDialogExtendedResult) == 2096);
static_assert(offsetof(WebBrowserDialogExtendedResult, extendedContent) == 2064);
static_assert(sizeof(WebBrowserDialogResult) == 256);

struct WebBrowserDialogAllowList {
    std::uint64_t size;
    const char* addresses[20];
    std::uint8_t reserved[256];
};

static_assert(sizeof(WebBrowserDialogAllowList) == 424);

struct CommonDialogResultPrefix {
    std::uint8_t untouched[8];
    std::int32_t result;
};

static_assert(sizeof(CommonDialogBaseParam) == 48);
static_assert(sizeof(G2PDialogParam) == 128);
static_assert(sizeof(PlayerReviewDialogParam) == 168);
static_assert(offsetof(CommonDialogResultPrefix, result) == 8);

inline bool ValidateCommonDialogBaseParam(const CommonDialogBaseParam& param) {
    return param.size == sizeof(param) && param.magic == static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&param)) - 1060003575u && std::all_of(std::begin(param.reserved), std::end(param.reserved), [](std::uint8_t value) { return value == 0; });
}

constexpr int COMMON_DIALOG_OK = 0;
constexpr int COMMON_DIALOG_ERROR_ALREADY_INITIALIZED = static_cast<int>(0x80B80004);

#endif
