#include "SceTypes.hpp"
#include "prx/libc/include/Shutdown.hpp"
#include "tests/VideoOutTestEnvironment.hpp"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>

extern "C" {
int APS5_VABI sceVideoOutOpen(int userId, int busType, int index, const void* param);
int APS5_VABI sceVideoOutClose(int handle);
int APS5_VABI sceVideoOutInitializeOutputOptions(VideoOutOutputOptions* options);
int APS5_VABI sceVideoOutIsOutputSupported(int handle, std::uint64_t mode, const VideoOutOutputOptions* options, void* reservedPtr, std::uint64_t reserved);
int APS5_VABI sceVideoOutConfigureOutput(int handle, std::uint64_t mode, const VideoOutOutputOptions* options, void* reservedPtr, std::uint64_t reserved);
void APS5_VABI sceVideoOutSetBufferAttribute2(VideoOutBufferAttribute2* attribute, std::uint64_t pixelFormat, std::uint32_t tilingMode, std::uint32_t width, std::uint32_t height, std::uint64_t option, std::uint32_t dccControl, std::uint64_t dccCbRegisterClearColor);
}

static constexpr int SYSTEM_USER = 255;
static constexpr int MAIN_BUS = 0;
static constexpr int UNSUPPORTED_OUTPUT_MODE = static_cast<int>(0x80290016);
static constexpr std::uint64_t TILING_MODE_LINEAR = 0;
static constexpr std::uint64_t PIXEL_FORMAT = 0x8000000022000000ull;

static void Require(bool value) { if (!value) std::abort(); }

int main() {
    setenv("ANYPS5_VIDEO_OUT_MAX_WIDTH", "1920", 1);
    setenv("ANYPS5_VIDEO_OUT_MAX_HEIGHT", "1080", 1);

    VideoOutTestEnvironment environment;
    VideoOutOutputOptions options{};
    Require(sceVideoOutInitializeOutputOptions(&options) == 0);
    int handle = 0;
    try {
        handle = sceVideoOutOpen(SYSTEM_USER, MAIN_BUS, 0, nullptr);
    } catch (const std::runtime_error& error) {
        if (std::getenv("ANYPS5_REQUIRE_DISPLAY") != nullptr) throw;
        std::printf("skipped, no display or Vulkan device: %s\n", error.what());
        return 77;
    }
    Require(handle > 0);

    Require(sceVideoOutIsOutputSupported(handle, 1, &options, nullptr, 0) == 1);
    Require(sceVideoOutConfigureOutput(handle, 1, &options, nullptr, 0) == 0);
    for (std::uint64_t mode : {0xD000000Aull, 0x8000000000000001ull, ~0ull}) {
        Require(sceVideoOutIsOutputSupported(handle, mode, &options, nullptr, 0) == 0);
        Require(sceVideoOutConfigureOutput(handle, mode, &options, nullptr, 0) == UNSUPPORTED_OUTPUT_MODE);
    }
    for (std::uint64_t mode : {0x4ull, 0x7ull, 0x8ull, 0xCull, 0xDull, 0xEull, 0x10ull, 0x11ull, 0x13ull}) {
        Require(sceVideoOutIsOutputSupported(handle, mode, &options, nullptr, 0) == UNSUPPORTED_OUTPUT_MODE);
    }

    VideoOutBufferAttribute2 attribute{};
    sceVideoOutSetBufferAttribute2(&attribute, PIXEL_FORMAT, TILING_MODE_LINEAR, 3840, 2160, 0, 0, 0);
    Require(attribute.width == 1920);
    Require(attribute.height == 1080);
    sceVideoOutSetBufferAttribute2(&attribute, PIXEL_FORMAT, TILING_MODE_LINEAR, 1280, 720, 0, 0, 0);
    Require(attribute.width == 1280);
    Require(attribute.height == 720);

    Require(sceVideoOutClose(handle) == 0);
    LibcRunShutdown_nid_postfix();
}
