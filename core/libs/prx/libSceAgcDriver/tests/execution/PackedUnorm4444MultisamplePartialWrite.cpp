#include "prx/libSceAgcDriver/Execution/include/GuestMemory.hpp"
#include "prx/libSceAgcDriver/Execution/include/VulkanDevice.hpp"
#include "prx/libSceAgcDriver/Execution/include/Driver/Shaders/ShaderRegistry.hpp"
#include "prx/libSceAgcDriver/Graphics/include/Draw.hpp"
#include "prx/libSceAgcDriver/Graphics/include/GuestTextureResource.hpp"
#include "prx/libSceAgcDriver/Graphics/include/ShaderInputState.hpp"
#include "prx/libSceAgcDriver/Graphics/include/State.hpp"
#include "prx/libc/include/GuestAllocations.hpp"
#include "Recompiler.hpp"
#include "Optimization/ResourceProgram.hpp"
#include "VulkanTestDevice.hpp"
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif
#include <algorithm>
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

namespace AgcDriver {

struct VulkanDeviceTestAccess {
    static Graphics::Context ContextOf(const VulkanDevice& device) { return device.graphicsContext(); }
};

}

namespace {

using AgcDriver::Graphics::Require;
using ShaderRecompiler::ShaderStage;

constexpr std::uint32_t Side = 16;
constexpr std::uint32_t Samples = 2;
constexpr std::uint32_t SampleShift = 1;
constexpr std::size_t BlockBytes = 65536;
constexpr std::uint32_t Unorm4_4_4_4Info = 0x8000u | (19u << 2u) | (2u << 11u);
constexpr std::uint32_t MultisampledAttrib = (SampleShift << 12u) | (SampleShift << 15u);
constexpr std::uint32_t TiledAttrib3 = 0x4dc6c000u;
constexpr std::uint32_t Float32Export = 9;
constexpr std::uint32_t Threads = 32;
constexpr std::uint32_t Words = 32;
constexpr std::uint32_t LoadResult = 12;
constexpr double EdgeMargin = 1.0 / 32.0;
constexpr std::array<float, 4> First{0.2f, 0.4f, 0.6f, 0.8f};
constexpr std::array<float, 4> Second{13.0f / 15.0f, 0.0f, 1.0f, 0.0f};

alignas(256) constexpr std::array<std::uint32_t, 6> VertexCode{
    0xe0382000, 0x80000005, 0xbf8c3f70, 0xf80008cf, 0x03020100, 0xbf810000,
};

alignas(256) constexpr std::array<std::uint32_t, 26> LoadCode{
    0x34020087, 0xe0301000, 0x80000201, 0xe0301004, 0x80000301, 0xe0301008, 0x80000401, 0xe030100c,
    0x80000501, 0xe0301010, 0x80000601, 0xe0301014, 0x80000701, 0xbf8c3f70, 0xf0001f30, 0x00011005,
    0xbf8c3f70, 0xe0701030, 0x80001001, 0xe0701034, 0x80001101, 0xe0701038, 0x80001201, 0xe070103c,
    0x80001301, 0xbf810000,
};

constexpr std::array<std::array<float, 4>, 3> Covering{{
    {-1.0f, -1.0f, 0.5f, 1.0f}, {3.0f, -1.0f, 0.5f, 1.0f}, {-1.0f, 3.0f, 0.5f, 1.0f}
}};

constexpr std::array<std::array<float, 4>, 3> Cutting{{
    {-0.93f, -1.0f, 0.5f, 1.0f}, {0.37f, -1.0f, 0.5f, 1.0f}, {-0.21f, 0.94f, 0.5f, 1.0f}
}};

alignas(256) std::array<std::array<std::uint32_t, 16>, 4> Programs{};
std::size_t programCount = 0;
alignas(256) std::array<std::uint32_t, Threads * Words> Buffer{};

std::uint32_t Unorm(float value) {
    if (!(value > 0.0f)) return 0;
    if (value >= 1.0f) return 15;
    return static_cast<std::uint32_t>(std::nearbyint(static_cast<double>(value) * 15.0));
}

std::uint32_t Packed(std::array<float, 4> rgba) {
    return Unorm(rgba[3]) | (Unorm(rgba[2]) << 4u) | (Unorm(rgba[1]) << 8u) | (Unorm(rgba[0]) << 12u);
}

std::span<const std::uint32_t> Float32Program(std::array<float, 4> rgba) {
    Require(programCount < Programs.size(), "too many test programs");
    auto& code = Programs[programCount++];
    std::size_t at = 0;
    for (std::uint32_t vgpr = 0; vgpr < 4u; ++vgpr) {
        code[at++] = 0x7e0002ffu | ((4u + vgpr) << 17u);
        code[at++] = std::bit_cast<std::uint32_t>(rgba[vgpr]);
    }
    code[at++] = 0xf800180fu;
    code[at++] = 0x07060504u;
    code[at++] = 0xbf810000u;
    return std::span<const std::uint32_t>(code.data(), at);
}

struct Block {
    explicit Block(std::size_t bytes) : bytes(bytes) {
#ifdef _WIN32
        data = static_cast<std::uint8_t*>(VirtualAlloc(nullptr, bytes, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
#else
        data = static_cast<std::uint8_t*>(std::aligned_alloc(BlockBytes, bytes));
#endif
        Require(data != nullptr, "cannot allocate the multisampled color target");
        std::memset(data, 0, bytes);
        GuestAllocations::Mutation mutation;
        mutation.Add(data, bytes, true, true, true);
    }
    ~Block() {
        {
            GuestAllocations::Mutation mutation;
            mutation.Remove(data);
        }
#ifdef _WIN32
        VirtualFree(data, 0, MEM_RELEASE);
#else
        std::free(data);
#endif
    }
    Block(const Block&) = delete;
    Block& operator=(const Block&) = delete;
    std::uint64_t Address() const { return reinterpret_cast<std::uintptr_t>(data); }
    std::size_t bytes;
    std::uint8_t* data = nullptr;
};

AgcDriver::Graphics::ColorTarget DecodeTarget(const Block& block) {
    AgcDriver::Registers cx;
    cx[0x318] = static_cast<std::uint32_t>(block.Address() >> 8u);
    cx[0x31b] = 0;
    cx[0x31c] = Unorm4_4_4_4Info;
    cx[0x31d] = MultisampledAttrib;
    cx[0x3b0] = ((Side - 1u) << 14u) | (Side - 1u);
    cx[0x3b8] = TiledAttrib3;
    cx[0x390] = static_cast<std::uint32_t>(block.Address() >> 40u);
    const auto color = AgcDriver::Graphics::DecodeColorBuffer(cx, 0);
    Require(color.format == VK_FORMAT_R16_UINT && color.componentMapping == 0x1bu && color.packing == ShaderRecompiler::ColorExportPacking::Unorm4_4_4_4 && color.samples == Samples && color.extent.width == Side && color.extent.height == Side, "the target did not decode as a 16x16 packed R16_UINT surface of 2 samples");
    return color;
}

AgcDriver::Graphics::State TargetState(AgcDriver::Graphics::ColorTarget color, std::uint32_t exportedMask) {
    const bool partial = exportedMask != 0xfu;
    color.partialWriteMask = partial ? AgcDriver::Graphics::PackedWriteMask(color.componentMapping, exportedMask) : 0u;
    VkPipelineColorBlendAttachmentState blend{};
    blend.colorWriteMask = partial ? 0u : 0xfu;
    AgcDriver::Graphics::State state{};
    state.stages = {AgcDriver::Graphics::ShaderPath::Vertex, 0u, 64u, 64u, std::nullopt, std::nullopt};
    state.colors = {color};
    state.color = color;
    state.hasColorTarget = true;
    state.renderExtent = {Side, Side};
    state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    state.viewport = {0, static_cast<float>(Side), static_cast<float>(Side), -static_cast<float>(Side), 0, 1};
    state.negativeOneToOne = false;
    state.scissor = {{0, 0}, {Side, Side}};
    state.cullMode = VK_CULL_MODE_NONE;
    state.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    state.blends = {blend};
    state.blend = blend;
    state.blendConstants = {};
    state.samples = Samples;
    state.perSampleShading = partial;
    return state;
}

void Draw(AgcDriver::VulkanDevice& device, const AgcDriver::Graphics::State& state, const std::array<std::array<float, 4>, 3>& triangle, std::span<const std::uint32_t> pixelCode) {
    const auto target = device.Target();
    constexpr std::uint32_t waveSize = 64;
    std::vector<std::uint32_t> vertexUserData(4, 0u);
    const auto address = reinterpret_cast<std::uintptr_t>(triangle.data());
    const std::array<std::uint32_t, 4> vertexBuffer{static_cast<std::uint32_t>(address), static_cast<std::uint32_t>((address >> 32u) & 0xffffu) | (16u << 16u), static_cast<std::uint32_t>(triangle.size()), 0x01016facu};
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
    const auto codeAddress = reinterpret_cast<std::uintptr_t>(pixelCode.data());
    const AgcDriver::Registers context{{0x1b6, 0x8000}, {0x1b3, 0}, {0x1b4, 0}, {0x203, 0}, {0x1c5, Float32Export}};
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
    const ShaderRecompiler::SrtRuntime runtime{};
    const auto invocation = AgcDriver::DriverDetail::InvocationFor(snapshot, 0, fragment);
    const auto pixelResult = invocation.Materialize(*invocation.Capture(runtime));
    const std::array<AgcDriver::Graphics::CompiledShader, 2> shaders{{
        {ShaderStage::Vertex, &vertexResult, 0},
        {ShaderStage::Fragment, pixelResult.get(), PixelPushOffset(vertexPush, target)}
    }};
    const AgcDriver::Pm4::DrawParameters draw{0, static_cast<std::uint32_t>(triangle.size()), 0, 1, 0, false};
    device.Draw(state, draw, shaders);
}

std::vector<std::uint32_t> LoadSamples(AgcDriver::VulkanDevice& device, const std::array<std::uint32_t, 8>& texture) {
    std::vector<std::uint32_t> userData(16, 0u);
    const auto address = reinterpret_cast<std::uintptr_t>(Buffer.data());
    const std::array<std::uint32_t, 4> buffer{static_cast<std::uint32_t>(address), static_cast<std::uint32_t>((address >> 32u) & 0xffffu), static_cast<std::uint32_t>(Buffer.size() * 4u), 0x31016facu};
    std::copy(buffer.begin(), buffer.end(), userData.begin());
    std::copy(texture.begin(), texture.end(), userData.begin() + 4);
    const std::array<ShaderRecompiler::MemoryRegion, 1> memory{{{reinterpret_cast<std::uintptr_t>(LoadCode.data()), std::as_bytes(std::span(LoadCode))}}};
    const ShaderRecompiler::ShaderComputeStageInfo compute{{Threads, 1, 1}, 0u, {false, false, false}, false, 1};
    ShaderRecompiler::RecompileRequest request{
        {ShaderStage::Compute, reinterpret_cast<std::uintptr_t>(LoadCode.data()), LoadCode, 0, {}},
        {32, 0, userData, compute, std::nullopt, std::nullopt, memory},
        device.Target(),
        {0, 0, 0, 128}
    };
    request.useCache = false;
    const auto program = ShaderRecompiler::Recompile(request);
    std::vector<std::uint32_t> loaded(Side * Side * Samples);
    for (std::uint32_t first = 0; first < loaded.size(); first += Threads) {
        Buffer.fill(0xdeadbeefu);
        for (std::uint32_t tid = 0; tid < Threads; ++tid) {
            const auto index = first + tid;
            const auto pixel = index / Samples;
            const std::array<std::uint32_t, 6> words{0u, 0u, 0u, pixel % Side, pixel / Side, index % Samples};
            std::copy(words.begin(), words.end(), &Buffer[tid * Words]);
        }
        device.Dispatch(program, 1, 1, 1, {}, reinterpret_cast<std::uintptr_t>(LoadCode.data()));
        device.WaitIdle();
        for (std::uint32_t tid = 0; tid < Threads; ++tid) loaded[first + tid] = Buffer[tid * Words + LoadResult];
    }
    return loaded;
}

double EdgeDistance(double x, double y, std::size_t from) {
    const auto pixels = [](const std::array<float, 4>& vertex) {
        return std::array<double, 2>{(static_cast<double>(vertex[0]) + 1.0) * 0.5 * Side, Side - (static_cast<double>(vertex[1]) + 1.0) * 0.5 * Side};
    };
    const auto a = pixels(Cutting[from]);
    const auto b = pixels(Cutting[(from + 1u) % Cutting.size()]);
    const auto c = pixels(Cutting[(from + 2u) % Cutting.size()]);
    const auto length = std::hypot(b[0] - a[0], b[1] - a[1]);
    const auto side = [&](double px, double py) { return ((b[0] - a[0]) * (py - a[1]) - (b[1] - a[1]) * (px - a[0])) / length; };
    return side(c[0], c[1]) < 0 ? -side(x, y) : side(x, y);
}

}

int main() {
    try {
        const auto device = OpenVulkanTestDevice();
        if (!device) return VulkanTestSkipped;
        const auto context = AgcDriver::VulkanDeviceTestAccess::ContextOf(*device);
        if (!context.storageImageMultisample || !context.sampleRateShading || !TargetHasCapability(device->Target(), spv::CapabilityFragmentShaderPixelInterlockEXT)) {
            std::puts("skipped, the device lacks storage image multisampling, sample rate shading or fragmentShaderPixelInterlock");
            return VulkanTestSkipped;
        }
        Block block(4u * BlockBytes);
        const auto color = DecodeTarget(block);
        const auto full = TargetState(color, 0xfu);
        Draw(*device, full, Covering, Float32Program(First));
        const auto merged = TargetState(color, 0x1u);
        Require(merged.colors.front().partialWriteMask == 0xf000u, "an X write of the 4_4_4_4 target did not merge bits 15..12");
        Draw(*device, merged, Cutting, Float32Program(Second));
        const auto texture = AgcDriver::Graphics::EncodeTextureResource(AgcDriver::Graphics::TargetMergeSurface(color));
        const auto loaded = LoadSamples(*device, texture);
        const auto background = Packed(First);
        const auto written = (background & 0x0fffu) | (Packed(Second) & 0xf000u);
        Require(written != background, "the test colors do not differ in the merged nibble");
        const auto locations = AgcDriver::Graphics::SampleLocations(merged);
        std::array<std::uint32_t, Samples + 1u> pixelsByCoverage{};
        std::uint32_t checked = 0;
        for (std::uint32_t y = 0; y < Side; ++y) {
            for (std::uint32_t x = 0; x < Side; ++x) {
                std::uint32_t covered = 0;
                for (std::uint32_t sample = 0; sample < Samples; ++sample) {
                    const auto word = loaded[(y * Side + x) * Samples + sample];
                    char where[160];
                    std::snprintf(where, sizeof(where), "sample %u of pixel (%u, %u) holds 0x%04x", sample, x, y, word);
                    Require(word == written || word == background, std::string(where) + ", neither the merged word 0x" + std::to_string(written) + " nor the first draw's word");
                    covered += word == written ? 1u : 0u;
                    const auto px = x + static_cast<double>(locations[sample].x);
                    const auto py = y + static_cast<double>(locations[sample].y);
                    const auto distance = std::min({EdgeDistance(px, py, 0), EdgeDistance(px, py, 1), EdgeDistance(px, py, 2)});
                    if (std::fabs(distance) < EdgeMargin) continue;
                    ++checked;
                    Require((word == written) == (distance > 0), std::string(where) + (distance > 0 ? ", which the merging triangle covers" : ", which the merging triangle does not cover"));
                }
                ++pixelsByCoverage[covered];
            }
        }
        Require(checked >= Side * Side * Samples * 9u / 10u, "fewer than nine in ten samples are clear of the triangle's edges");
        Require(pixelsByCoverage[0] != 0 && pixelsByCoverage[Samples] != 0 && pixelsByCoverage[1] != 0, "the merging triangle does not leave pixels uncovered, covered and partly covered");
        std::printf("4_4_4_4 unorm 2-sample partial write tests passed: %u partly covered pixels merged bits 15..12 into their covered samples only\n", pixelsByCoverage[1]);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
