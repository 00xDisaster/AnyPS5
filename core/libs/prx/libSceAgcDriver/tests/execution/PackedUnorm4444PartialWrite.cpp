#include "prx/libSceAgcDriver/Execution/include/GuestMemory.hpp"
#include "prx/libSceAgcDriver/Execution/include/VulkanDevice.hpp"
#include "prx/libSceAgcDriver/Execution/include/Driver/Shaders/ShaderRegistry.hpp"
#include "prx/libSceAgcDriver/Graphics/include/ColorTargetLayout.hpp"
#include "prx/libSceAgcDriver/Graphics/include/Draw.hpp"
#include "prx/libSceAgcDriver/Graphics/include/ShaderInputState.hpp"
#include "prx/libSceAgcDriver/Graphics/include/State.hpp"
#include "prx/libSceAgcDriver/Graphics/include/Texture.hpp"
#include "prx/libc/include/GuestAllocations.hpp"
#include "prx/libc/include/GuestArena.hpp"
#include "prx/libc/include/GuestWriteWatch.hpp"
#include "Recompiler.hpp"
#include "Optimization/ResourceProgram.hpp"
#include "VulkanTestDevice.hpp"
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <sys/mman.h>
#endif
#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <span>
#include <string>
#include <vector>

namespace {

using AgcDriver::Graphics::ColorTargetLayout;
using AgcDriver::Graphics::ColorTileMode;
using AgcDriver::Graphics::Require;
using ShaderRecompiler::ShaderStage;

constexpr std::uint32_t Width = 256;
constexpr std::uint32_t Height = 128;
constexpr std::uint32_t ScissorWidth = 128;
constexpr std::size_t BlockBytes = 65536;
constexpr std::uint32_t Unorm4_4_4_4Info = 0x8000u | (19u << 2u) | (2u << 11u);
constexpr std::uint32_t TiledAttrib3 = 0x4dc6c000u;
constexpr std::uint32_t Float32Export = 9;
constexpr std::uint32_t Float16Export = 4;
constexpr std::uint32_t PositionInputs = 0x300u;
constexpr std::uint32_t LateKill = 0x40u;
constexpr std::array<std::uint32_t, 5> CheckerboardKill{0x7e040f00u, 0x7e060f01u, 0x3a040702u, 0x36040481u, 0x7da40480u};

alignas(256) constexpr std::array<std::uint32_t, 6> VertexCode{
    0xe0382000, 0x80000005, 0xbf8c3f70, 0xf80008cf, 0x03020100, 0xbf810000,
};

constexpr std::array<std::array<float, 4>, 3> Triangle{{
    {-1.0f, -1.0f, 0.5f, 1.0f}, {3.0f, -1.0f, 0.5f, 1.0f}, {-1.0f, 3.0f, 0.5f, 1.0f}
}};

alignas(256) std::array<std::array<std::uint32_t, 64>, 64> Programs{};
std::size_t programCount = 0;

std::uint32_t Unorm(float value) {
    if (!(value > 0.0f)) return 0;
    if (value >= 1.0f) return 15;
    return static_cast<std::uint32_t>(std::nearbyint(static_cast<double>(value) * 15.0));
}

std::uint32_t Packed(std::array<float, 4> rgba) {
    return Unorm(rgba[3]) | (Unorm(rgba[2]) << 4u) | (Unorm(rgba[1]) << 8u) | (Unorm(rgba[0]) << 12u);
}

std::uint32_t Half(float value) {
    Require(value == 0.0f || (value >= 0.0625f && value <= 1.0f), "the test's half values are normal and at most 1");
    if (value == 0.0f) return 0;
    const auto bits = std::bit_cast<std::uint32_t>(value);
    Require((bits & 0x1fffu) == 0, "the test's half values are exact");
    return ((((bits >> 23u) & 0xffu) - 112u) << 10u) | ((bits >> 13u) & 0x3ffu);
}

std::span<const std::uint32_t> Program(const std::array<std::uint32_t, 4>& vgprs, bool compressed, bool killing = false) {
    Require(programCount < Programs.size(), "too many test programs");
    auto& code = Programs[programCount++];
    std::size_t at = 0;
    if (killing) {
        for (const auto word : CheckerboardKill) code[at++] = word;
    }
    for (std::uint32_t vgpr = 0; vgpr < (compressed ? 2u : 4u); ++vgpr) {
        code[at++] = 0x7e0002ffu | ((4u + vgpr) << 17u);
        code[at++] = vgprs[vgpr];
    }
    code[at++] = compressed ? 0xf8001c0fu : 0xf800180fu;
    code[at++] = compressed ? 0x00000504u : 0x07060504u;
    code[at++] = 0xbf810000u;
    return std::span<const std::uint32_t>(code.data(), at);
}

std::span<const std::uint32_t> Float32Program(std::array<float, 4> rgba, bool killing = false) {
    return Program({std::bit_cast<std::uint32_t>(rgba[0]), std::bit_cast<std::uint32_t>(rgba[1]), std::bit_cast<std::uint32_t>(rgba[2]), std::bit_cast<std::uint32_t>(rgba[3])}, false, killing);
}

std::span<const std::uint32_t> Float16Program(std::array<float, 4> rgba) {
    return Program({Half(rgba[0]) | (Half(rgba[1]) << 16u), Half(rgba[2]) | (Half(rgba[3]) << 16u), 0, 0}, true);
}

struct Block {
    explicit Block(std::size_t bytes) : bytes(bytes), watched(AgcDriver::GuestMemory::WriteWatched()) {
#ifdef _WIN32
        if (watched) {
            data = static_cast<std::uint8_t*>(GuestArena::GuestArenaAllocate_nid_postfix(bytes, BlockBytes));
            GuestArena::GuestArenaCommit_nid_postfix(data, bytes, PAGE_READWRITE, bytes);
        } else {
            data = static_cast<std::uint8_t*>(VirtualAlloc(nullptr, bytes, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
        }
#else
        if (watched) {
            void* raw = mmap(nullptr, bytes + BlockBytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
            Require(raw != MAP_FAILED, "cannot map the color target");
            const auto begin = reinterpret_cast<std::uintptr_t>(raw);
            const auto aligned = (begin + BlockBytes - 1) & ~(static_cast<std::uintptr_t>(BlockBytes) - 1);
            if (aligned != begin) munmap(raw, aligned - begin);
            if (aligned + bytes != begin + bytes + BlockBytes) munmap(reinterpret_cast<void*>(aligned + bytes), begin + BlockBytes - aligned);
            data = reinterpret_cast<std::uint8_t*>(aligned);
            GuestWriteWatch::GuestWriteWatchRegister_nid_postfix(data, bytes);
        } else {
            data = static_cast<std::uint8_t*>(std::aligned_alloc(BlockBytes, bytes));
        }
#endif
        Require(data != nullptr, "cannot allocate the color target");
        Require(!watched || AgcDriver::GuestMemory::Watched(Address(), bytes), "the color target is not write-watched");
        GuestAllocations::Mutation mutation;
        mutation.Add(data, bytes, true, true, true);
    }
    ~Block() {
        AgcDriver::Graphics::StorageTexture::FlushPending(Address(), bytes, nullptr, "test release");
        {
            GuestAllocations::Mutation mutation;
            mutation.Remove(data);
        }
#ifdef _WIN32
        if (watched) {
            GuestArena::GuestArenaReset_nid_postfix(data, bytes);
            GuestArena::GuestArenaRelease_nid_postfix(data, bytes);
        } else {
            VirtualFree(data, 0, MEM_RELEASE);
        }
#else
        if (watched) {
            munmap(data, bytes);
            GuestWriteWatch::GuestWriteWatchUnregister_nid_postfix(data, bytes);
        } else {
            std::free(data);
        }
#endif
    }
    Block(const Block&) = delete;
    Block& operator=(const Block&) = delete;
    std::uint64_t Address() const { return reinterpret_cast<std::uintptr_t>(data); }
    std::size_t bytes;
    bool watched;
    std::uint8_t* data = nullptr;
};

AgcDriver::Graphics::ColorTarget DecodeTarget(const Block& block) {
    AgcDriver::Registers cx;
    cx[0x318] = static_cast<std::uint32_t>(block.Address() >> 8u);
    cx[0x31b] = 0;
    cx[0x31c] = Unorm4_4_4_4Info;
    cx[0x31d] = 0;
    cx[0x3b0] = ((Width - 1u) << 14u) | (Height - 1u);
    cx[0x3b8] = TiledAttrib3;
    cx[0x390] = static_cast<std::uint32_t>(block.Address() >> 40u);
    auto color = AgcDriver::Graphics::DecodeColorBuffer(cx, 0);
    Require(color.format == VK_FORMAT_R16_UINT && color.componentMapping == 0x1bu && color.packing == ShaderRecompiler::ColorExportPacking::Unorm4_4_4_4 && color.tileMode == ColorTileMode::RenderTarget && color.bytes == block.bytes, "the tiled 4_4_4_4 unorm target did not decode as a packed R16_UINT target");
    return color;
}

std::uint32_t Draw(AgcDriver::VulkanDevice& device, AgcDriver::Graphics::ColorTarget color, std::uint32_t exportedMask, std::uint32_t exportFormat, std::span<const std::uint32_t> pixelCode, bool killing = false) {
    const auto target = device.Target();
    constexpr std::uint32_t waveSize = 64;
    std::vector<std::uint32_t> vertexUserData(4, 0u);
    const auto address = reinterpret_cast<std::uintptr_t>(Triangle.data());
    const std::array<std::uint32_t, 4> vertexBuffer{static_cast<std::uint32_t>(address), static_cast<std::uint32_t>((address >> 32u) & 0xffffu) | (16u << 16u), static_cast<std::uint32_t>(Triangle.size()), 0x01016facu};
    std::copy(vertexBuffer.begin(), vertexBuffer.end(), vertexUserData.begin());
    const std::array<ShaderRecompiler::MemoryRegion, 1> vertexMemory{{{reinterpret_cast<std::uintptr_t>(VertexCode.data()), std::as_bytes(std::span(VertexCode))}}};
    ShaderRecompiler::RecompileRequest vertex{
        {ShaderStage::Vertex, reinterpret_cast<std::uintptr_t>(VertexCode.data()), VertexCode, 0, {}},
        {waveSize, 0, vertexUserData, std::nullopt, std::nullopt, ShaderRecompiler::ShaderVertexStageInfo{}, vertexMemory},
        target,
        {0, 0, 0, 64}
    };
    vertex.useCache = false;
    const auto vertexResult = ShaderRecompiler::Recompile(vertex);
    const auto vertexPush = static_cast<std::uint32_t>(vertexResult.pushConstants.size());

    const bool partial = exportedMask != 0xfu;
    color.partialWriteMask = partial ? AgcDriver::Graphics::PackedWriteMask(color.componentMapping, exportedMask) : 0u;
    AgcDriver::Graphics::State state{};
    state.stages = {AgcDriver::Graphics::ShaderPath::Vertex, 0u, waveSize, waveSize, std::nullopt, std::nullopt};
    state.colors = {color};
    state.color = color;
    state.hasColorTarget = true;
    state.renderExtent = {Width, Height};
    state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    state.viewport = {0, static_cast<float>(Height), static_cast<float>(Width), -static_cast<float>(Height), 0, 1};
    state.negativeOneToOne = false;
    state.scissor = {{0, 0}, {ScissorWidth, Height}};
    state.cullMode = VK_CULL_MODE_NONE;
    state.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    VkPipelineColorBlendAttachmentState blend{};
    blend.colorWriteMask = partial ? 0u : VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    state.blends = {blend};
    state.blend = blend;
    state.blendConstants = {};

    const auto codeAddress = reinterpret_cast<std::uintptr_t>(pixelCode.data());
    const AgcDriver::Registers context{{0x1b6, 0x8000}, {0x1b3, killing ? PositionInputs : 0u}, {0x1b4, killing ? PositionInputs : 0u}, {0x203, killing ? LateKill : 0u}, {0x1c5, exportFormat}};
    const std::array<ShaderRecompiler::MemoryRegion, 1> pixelMemory{{{codeAddress, std::as_bytes(pixelCode)}}};
    ShaderRecompiler::RecompileRequest fragment{
        {ShaderStage::Fragment, codeAddress, pixelCode, 0, {}},
        {waveSize, 0, {}, std::nullopt, AgcDriver::Graphics::DecodePixelStageInfo(context, {}), std::nullopt, pixelMemory},
        target,
        PixelPushLayout(vertexPush, target)
    };
    const auto handle = ShaderRecompiler::PrepareShader(fragment);
    AgcDriver::DriverDetail::ShaderSnapshot snapshot{codeAddress, 0, 1, {pixelCode.begin(), pixelCode.end()}, {}};
    snapshot.prepared->entries.push_back({0, handle});
    fragment.shader.code = snapshot.code;
    fragment.context.pixel = AgcDriver::Graphics::DecodePixelStageInfo(context, AgcDriver::Graphics::ExportMappings(state));
    fragment.context.pixel->targetExportPacking = AgcDriver::Graphics::ExportPackings(state);
    fragment.context.pixel->targetMerge = AgcDriver::Graphics::ExportTargetMerge(state);
    Require(fragment.context.pixel->pixelKillEnable == killing && !fragment.context.pixel->earlyZ && !fragment.context.pixel->targetMerge.depthStencilWrites, "the draw's pixel kill under late tests or its depth and stencil writes did not decode");
    Require((fragment.context.pixel->targetMerge.mask != 0u) == partial && fragment.context.pixel->targetMerge.mask == color.partialWriteMask, "the draw's target merge disagrees with its write mask");
    const ShaderRecompiler::SrtRuntime runtime{};
    const auto invocation = AgcDriver::DriverDetail::InvocationFor(snapshot, 0, fragment);
    const auto pixelResult = invocation.Materialize(*invocation.Capture(runtime));
    const bool registered = pixelResult->variantId == ShaderRecompiler::GetPreparedArtifact(*handle).variantId;
    Require(registered != partial, partial ? "the merging draw reused the registered pixel shader" : "the full write did not use the registered pixel shader");
    const std::array<AgcDriver::Graphics::CompiledShader, 2> shaders{{
        {ShaderStage::Vertex, &vertexResult, 0},
        {ShaderStage::Fragment, pixelResult.get(), PixelPushOffset(vertexPush, target)}
    }};
    const AgcDriver::Pm4::DrawParameters draw{0, static_cast<std::uint32_t>(Triangle.size()), 0, 1, 0, false};
    device.Draw(state, draw, shaders);
    return color.partialWriteMask;
}

std::vector<std::uint16_t> ReadBack(AgcDriver::VulkanDevice& device, const Block& block) {
    AgcDriver::Graphics::StorageTexture::FlushPending(block.Address(), block.bytes, nullptr, "test read-back");
    device.WaitIdle();
    std::vector<std::uint16_t> stored(block.bytes / 2u);
    AgcDriver::GuestMemory::Read(block.Address(), std::as_writable_bytes(std::span(stored)), 1);
    return stored;
}

void ExpectSurface(AgcDriver::VulkanDevice& device, const Block& block, const ColorTargetLayout& layout, const std::vector<std::uint16_t>& before, std::uint32_t word, std::uint32_t wordMask, const std::string& what) {
    const auto stored = ReadBack(device, block);
    for (std::uint32_t y = 0; y < Height; ++y) {
        for (std::uint32_t x = 0; x < Width; ++x) {
            const auto index = layout.Offset(x, y) / 2u;
            const bool inside = x < ScissorWidth;
            const std::uint32_t expected = inside ? (before[index] & ~wordMask) | (word & wordMask) : before[index];
            if (stored[index] == expected) continue;
            char detail[192];
            std::snprintf(detail, sizeof(detail), ": pixel (%u, %u) %s the scissor holds 0x%04x, expected 0x%04x (it held 0x%04x)", x, y, inside ? "inside" : "outside", stored[index], expected, before[index]);
            throw std::runtime_error(what + detail);
        }
    }
}

std::vector<std::uint16_t> Fill(Block& block, std::uint32_t seed) {
    std::vector<std::uint16_t> words(block.bytes / 2u);
    for (std::size_t index = 0; index < words.size(); ++index) words[index] = static_cast<std::uint16_t>(((index + seed) * 0x9e3779b9u) >> 11u);
    std::memcpy(block.data, words.data(), block.bytes);
    return words;
}

struct Case {
    std::uint32_t mask;
    std::array<float, 4> rgba;
};

void BackToBack(AgcDriver::VulkanDevice& device, Block& block, const ColorTargetLayout& layout, const AgcDriver::Graphics::ColorTarget& color, std::span<const Case> draws, std::uint32_t seed, const std::string& what) {
    const auto before = Fill(block, seed);
    auto expected = before;
    for (const auto& [mask, rgba] : draws) {
        const auto drawn = Draw(device, color, mask, Float32Export, Float32Program(rgba));
        const auto wordMask = mask == 0xfu ? 0xffffu : drawn;
        for (std::uint32_t y = 0; y < Height; ++y) {
            for (std::uint32_t x = 0; x < ScissorWidth; ++x) {
                auto& word = expected[layout.Offset(x, y) / 2u];
                word = static_cast<std::uint16_t>((word & ~wordMask) | (Packed(rgba) & wordMask));
            }
        }
    }
    const auto stored = ReadBack(device, block);
    for (std::uint32_t y = 0; y < Height; ++y) {
        for (std::uint32_t x = 0; x < Width; ++x) {
            const auto index = layout.Offset(x, y) / 2u;
            if (stored[index] == expected[index]) continue;
            char detail[160];
            std::snprintf(detail, sizeof(detail), ": pixel (%u, %u) holds 0x%04x, expected 0x%04x (it held 0x%04x)", x, y, stored[index], expected[index], before[index]);
            throw std::runtime_error(what + detail);
        }
    }
}

void ExpectKilled(AgcDriver::VulkanDevice& device, const Block& block, const ColorTargetLayout& layout, const std::vector<std::uint16_t>& before, std::uint32_t word, std::uint32_t wordMask, const std::string& what) {
    const auto stored = ReadBack(device, block);
    for (std::uint32_t y = 0; y < Height; ++y) {
        for (std::uint32_t x = 0; x < Width; ++x) {
            const auto index = layout.Offset(x, y) / 2u;
            const bool kept = x < ScissorWidth && ((x ^ y) & 1u) == 0u;
            const std::uint32_t expected = kept ? (before[index] & ~wordMask) | (word & wordMask) : before[index];
            if (stored[index] == expected) continue;
            char detail[192];
            std::snprintf(detail, sizeof(detail), ": %s pixel (%u, %u) holds 0x%04x, expected 0x%04x (it held 0x%04x)", kept ? "a surviving" : x < ScissorWidth ? "a killed" : "an unscissored", x, y, stored[index], expected, before[index]);
            throw std::runtime_error(what + detail);
        }
    }
}

std::string Describe(std::uint32_t mask, std::array<float, 4> rgba) {
    char text[160];
    std::snprintf(text, sizeof(text), "CB_TARGET_MASK 0x%x write of (%.9g, %.9g, %.9g, %.9g)", mask, rgba[0], rgba[1], rgba[2], rgba[3]);
    return text;
}

}

int main() {
    try {
        const auto device = OpenVulkanTestDevice();
        if (!device) return VulkanTestSkipped;
        if (!TargetHasCapability(device->Target(), spv::CapabilityFragmentShaderPixelInterlockEXT)) {
            std::puts("skipped, the device lacks fragmentShaderPixelInterlock");
            return VulkanTestSkipped;
        }
        const ColorTargetLayout layout(Width, Height, ColorTileMode::RenderTarget, 2);
        Block block(layout.Bytes());
        const auto color = DecodeTarget(block);
        Require(AgcDriver::Graphics::PackedWriteMask(color.componentMapping, 1u) == 0xf000u && AgcDriver::Graphics::PackedWriteMask(color.componentMapping, 8u) == 0x000fu && AgcDriver::Graphics::PackedWriteMask(color.componentMapping, 5u) == 0xf0f0u, "the export components did not map to the guest nibbles (X in 15..12, W in 3..0)");
        const std::array<Case, 9> cases{{
            {0x1u, {1.0f, 0.0f, 0.0f, 0.0f}},
            {0x1u, {7.0f / 15.0f, 1.0f, 1.0f, 1.0f}},
            {0x5u, {0.2f, 0.4f, 0.6f, 0.8f}},
            {0x2u, {0.0f, 2.0f / 15.0f, 1.0f, 1.0f}},
            {0x8u, {1.0f, 1.0f, 1.0f, 11.0f / 15.0f}},
            {0x6u, {0.0f, 0.0f, 0.0f, 0.0f}},
            {0xeu, {0.0f, 1.0f, 0.5f, 0.25f}},
            {0xfu, {0.1f, 0.7f, 0.9f, 0.3f}},
            {0x1u, {0.0f, 0.0f, 0.0f, 0.0f}},
        }};
        auto before = Fill(block, 0);
        for (const auto& [mask, rgba] : cases) {
            const auto wordMask = Draw(*device, color, mask, Float32Export, Float32Program(rgba));
            ExpectSurface(*device, block, layout, before, Packed(rgba), mask == 0xfu ? 0xffffu : wordMask, "a 32_ABGR " + Describe(mask, rgba) + " into a tiled 4_4_4_4 unorm target");
            before = ReadBack(*device, block);
        }
        const std::array<float, 4> b{2.0f / 15.0f, 1.0f, 1.0f, 1.0f};
        const std::array<float, 4> c{0.6f, 0.2f, 0.8f, 0.4f};
        const std::array<float, 4> d{1.0f, 0.0f, 9.0f / 15.0f, 0.0f};
        const std::array<Case, 2> mergeThenFull{{{0x1u, b}, {0xfu, c}}};
        const std::array<Case, 2> fullThenMerge{{{0xfu, c}, {0x1u, b}}};
        const std::array<Case, 3> mergeThenMerge{{{0x1u, b}, {0x4u, d}, {0x8u, c}}};
        BackToBack(*device, block, layout, color, mergeThenFull, 11, "a merged X write followed at once by a full write");
        BackToBack(*device, block, layout, color, fullThenMerge, 12, "a full write followed at once by a merged X write");
        BackToBack(*device, block, layout, color, mergeThenMerge, 13, "merged X, Z and W writes drawn back to back");
        before = Fill(block, 77);
        const std::array<float, 4> halves{0.5f, 0.75f, 0.125f, 0.25f};
        const auto wordMask = Draw(*device, color, 0x9u, Float16Export, Float16Program(halves));
        ExpectSurface(*device, block, layout, before, Packed(halves), wordMask, "an FP16_ABGR " + Describe(0x9u, halves) + " into a tiled 4_4_4_4 unorm target");
        for (const auto& [mask, rgba] : {Case{0x1u, {0.6f, 0.2f, 0.8f, 0.4f}}, Case{0x6u, {1.0f, 2.0f / 15.0f, 13.0f / 15.0f, 0.0f}}}) {
            before = Fill(block, 91u + mask);
            const auto killedMask = Draw(*device, color, mask, Float32Export, Float32Program(rgba, true), true);
            ExpectKilled(*device, block, layout, before, Packed(rgba), killedMask, "a late-test checkerboard-killing " + Describe(mask, rgba) + " into a tiled 4_4_4_4 unorm target");
        }
        std::printf("4_4_4_4 unorm partial write tests passed: %zu masked draws, 3 back-to-back sequences and 2 checkerboard-killing draws merged into a %ux%u tiled target, the killed pixels and those outside the scissor kept\n", cases.size() + 1u, Width, Height);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
