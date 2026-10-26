#include "prx/libSceAgcDriver/Execution/include/VulkanDevice.hpp"
#include "prx/libSceAgcDriver/Graphics/include/PackedFloatResolve.hpp"
#include "prx/libSceAgcDriver/Graphics/include/Pipeline.hpp"
#include "prx/libSceAgcDriver/Graphics/include/Resources.hpp"
#include "prx/libc/include/Shutdown.hpp"
#include "VulkanTestDevice.hpp"
#include "PackedFloatResolveTestFill_spv.h"
#include "PackedFloatResolveTestVertex_spv.h"
#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

namespace AgcDriver {

struct VulkanDeviceTestAccess {
    static Graphics::Context ContextOf(const VulkanDevice& device) { return device.graphicsContext(); }
};

}

namespace {

using AgcDriver::Graphics::Check;
using AgcDriver::Graphics::Context;
using AgcDriver::Graphics::Require;

constexpr std::uint32_t Width = 16;
constexpr std::uint32_t Height = 8;

float Decode(std::uint32_t bits, std::uint32_t mantissa) {
    const std::uint32_t exponent = bits >> mantissa;
    const std::uint32_t fraction = bits & ((1u << mantissa) - 1u);
    if (exponent == 0u) return std::ldexp(static_cast<float>(fraction), -14 - static_cast<int>(mantissa));
    return std::ldexp(1.0f + std::ldexp(static_cast<float>(fraction), -static_cast<int>(mantissa)), static_cast<int>(exponent) - 15);
}

std::uint32_t Encode(float value, std::uint32_t mantissa) {
    if (!(value > 0.0f)) return 0u;
    const auto bits = std::bit_cast<std::uint32_t>(value);
    const int exponent = static_cast<int>(bits >> 23) - 127 + 15;
    if (exponent <= 0) return static_cast<std::uint32_t>(std::ldexp(value, 14 + static_cast<int>(mantissa)));
    if (exponent >= 31) return 31u << mantissa;
    return (static_cast<std::uint32_t>(exponent) << mantissa) | ((bits >> (23u - mantissa)) & ((1u << mantissa) - 1u));
}

std::uint32_t Pack(float red, float green, float blue) {
    return Encode(red, 6u) | (Encode(green, 6u) << 11u) | (Encode(blue, 5u) << 22u);
}

std::uint32_t Reference(std::span<const std::uint32_t> samples) {
    std::uint32_t result = 0;
    for (std::uint32_t field = 0; field < 3u; ++field) {
        const std::uint32_t mantissa = field == 2u ? 5u : 6u;
        float sum = 0.0f;
        bool special = false;
        std::uint32_t specialBits = 0;
        for (const auto word : samples) {
            const std::uint32_t bits = field == 0u ? (word & 0x7ffu) : field == 1u ? ((word >> 11u) & 0x7ffu) : (word >> 22u);
            if ((bits >> mantissa) == 31u) {
                if (!special) {
                    special = true;
                    specialBits = bits;
                }
            } else {
                sum += Decode(bits, mantissa);
            }
        }
        const std::uint32_t packed = special ? specialBits : Encode(sum / static_cast<float>(samples.size()), mantissa);
        result |= packed << (field == 0u ? 0u : field == 1u ? 11u : 22u);
    }
    return result;
}

struct Image {
    Context context;
    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;

    Image(const Context& context, std::uint32_t samples, VkImageUsageFlags usage) : context(context) {
        try {
            VkImageCreateInfo info{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
            info.imageType = VK_IMAGE_TYPE_2D;
            info.format = VK_FORMAT_R32_UINT;
            info.extent = {Width, Height, 1u};
            info.mipLevels = 1u;
            info.arrayLayers = 1u;
            info.samples = static_cast<VkSampleCountFlagBits>(samples);
            info.tiling = VK_IMAGE_TILING_OPTIMAL;
            info.usage = usage;
            info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            Check(context.Function<PFN_vkCreateImage>("vkCreateImage")(context.device, &info, nullptr, &image), "vkCreateImage packed float resolve test");
            VkMemoryRequirements requirements{};
            context.Function<PFN_vkGetImageMemoryRequirements>("vkGetImageMemoryRequirements")(context.device, image, &requirements);
            VkMemoryAllocateInfo allocation{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
            allocation.allocationSize = requirements.size;
            allocation.memoryTypeIndex = context.MemoryType(requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            Check(context.Function<PFN_vkAllocateMemory>("vkAllocateMemory")(context.device, &allocation, nullptr, &memory), "vkAllocateMemory packed float resolve test");
            Check(context.Function<PFN_vkBindImageMemory>("vkBindImageMemory")(context.device, image, memory, 0u), "vkBindImageMemory packed float resolve test");
            VkImageViewCreateInfo viewInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
            viewInfo.image = image;
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = VK_FORMAT_R32_UINT;
            viewInfo.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u};
            Check(context.Function<PFN_vkCreateImageView>("vkCreateImageView")(context.device, &viewInfo, nullptr, &view), "vkCreateImageView packed float resolve test");
        } catch (...) {
            Release();
            throw;
        }
    }

    ~Image() { Release(); }
    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;

    void Release() noexcept {
        if (view) context.Function<PFN_vkDestroyImageView>("vkDestroyImageView")(context.device, view, nullptr);
        if (image) context.Function<PFN_vkDestroyImage>("vkDestroyImage")(context.device, image, nullptr);
        if (memory) context.Function<PFN_vkFreeMemory>("vkFreeMemory")(context.device, memory, nullptr);
        view = VK_NULL_HANDLE;
        image = VK_NULL_HANDLE;
        memory = VK_NULL_HANDLE;
    }
};

struct FillPass {
    Context context;
    VkRenderPass pass = VK_NULL_HANDLE;
    VkPipelineLayout layout = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;
    std::array<VkShaderModule, 2> modules{};

    FillPass(const Context& context, std::uint32_t samples) : context(context) {
        try {
            VkAttachmentDescription attachment{};
            attachment.format = VK_FORMAT_R32_UINT;
            attachment.samples = static_cast<VkSampleCountFlagBits>(samples);
            attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            attachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;
            const VkAttachmentReference reference{0u, VK_IMAGE_LAYOUT_GENERAL};
            VkSubpassDescription subpass{};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1u;
            subpass.pColorAttachments = &reference;
            VkRenderPassCreateInfo renderPass{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
            renderPass.attachmentCount = 1u;
            renderPass.pAttachments = &attachment;
            renderPass.subpassCount = 1u;
            renderPass.pSubpasses = &subpass;
            Check(context.Function<PFN_vkCreateRenderPass>("vkCreateRenderPass")(context.device, &renderPass, nullptr, &pass), "vkCreateRenderPass packed float resolve test");
            const auto makeModule = [&](std::span<const std::uint32_t> words, VkShaderModule& module) {
                VkShaderModuleCreateInfo shader{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
                shader.codeSize = words.size_bytes();
                shader.pCode = words.data();
                Check(context.Function<PFN_vkCreateShaderModule>("vkCreateShaderModule")(context.device, &shader, nullptr, &module), "vkCreateShaderModule packed float resolve test");
            };
            makeModule(PACKED_FLOAT_RESOLVE_TEST_VERTEX_SPV, modules[0]);
            makeModule(PACKED_FLOAT_RESOLVE_TEST_FILL_SPV, modules[1]);
            const VkPushConstantRange push{VK_SHADER_STAGE_FRAGMENT_BIT, 0u, 8u * sizeof(std::uint32_t)};
            VkPipelineLayoutCreateInfo layoutInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
            layoutInfo.pushConstantRangeCount = 1u;
            layoutInfo.pPushConstantRanges = &push;
            Check(context.Function<PFN_vkCreatePipelineLayout>("vkCreatePipelineLayout")(context.device, &layoutInfo, nullptr, &layout), "vkCreatePipelineLayout packed float resolve test");
            std::array<VkPipelineShaderStageCreateInfo, 2> stages{};
            for (std::uint32_t index = 0; index < stages.size(); ++index) {
                stages[index] = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
                stages[index].stage = index == 0u ? VK_SHADER_STAGE_VERTEX_BIT : VK_SHADER_STAGE_FRAGMENT_BIT;
                stages[index].module = modules[index];
                stages[index].pName = "main";
            }
            const VkPipelineVertexInputStateCreateInfo vertices{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
            VkPipelineInputAssemblyStateCreateInfo assembly{VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
            assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            const VkViewport viewport{0.0f, 0.0f, static_cast<float>(Width), static_cast<float>(Height), 0.0f, 1.0f};
            const VkRect2D scissor{{0, 0}, {Width, Height}};
            VkPipelineViewportStateCreateInfo viewportState{VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
            viewportState.viewportCount = viewportState.scissorCount = 1u;
            viewportState.pViewports = &viewport;
            viewportState.pScissors = &scissor;
            VkPipelineRasterizationStateCreateInfo raster{VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
            raster.polygonMode = VK_POLYGON_MODE_FILL;
            raster.cullMode = VK_CULL_MODE_NONE;
            raster.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            raster.lineWidth = 1.0f;
            VkPipelineMultisampleStateCreateInfo multisample{VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
            multisample.rasterizationSamples = static_cast<VkSampleCountFlagBits>(samples);
            multisample.sampleShadingEnable = VK_TRUE;
            multisample.minSampleShading = 1.0f;
            VkPipelineColorBlendAttachmentState blend{};
            blend.colorWriteMask = 15u;
            VkPipelineColorBlendStateCreateInfo color{VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
            color.attachmentCount = 1u;
            color.pAttachments = &blend;
            VkGraphicsPipelineCreateInfo graphics{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
            graphics.stageCount = static_cast<std::uint32_t>(stages.size());
            graphics.pStages = stages.data();
            graphics.pVertexInputState = &vertices;
            graphics.pInputAssemblyState = &assembly;
            graphics.pViewportState = &viewportState;
            graphics.pRasterizationState = &raster;
            graphics.pMultisampleState = &multisample;
            graphics.pColorBlendState = &color;
            graphics.layout = layout;
            graphics.renderPass = pass;
            Check(context.Function<PFN_vkCreateGraphicsPipelines>("vkCreateGraphicsPipelines")(context.device, VK_NULL_HANDLE, 1u, &graphics, nullptr, &pipeline), "vkCreateGraphicsPipelines packed float resolve test");
        } catch (...) {
            Release();
            throw;
        }
    }

    ~FillPass() { Release(); }
    FillPass(const FillPass&) = delete;
    FillPass& operator=(const FillPass&) = delete;

    void Release() noexcept {
        if (pipeline) context.Function<PFN_vkDestroyPipeline>("vkDestroyPipeline")(context.device, pipeline, nullptr);
        if (layout) context.Function<PFN_vkDestroyPipelineLayout>("vkDestroyPipelineLayout")(context.device, layout, nullptr);
        for (const auto module : modules) if (module) context.Function<PFN_vkDestroyShaderModule>("vkDestroyShaderModule")(context.device, module, nullptr);
        if (pass) context.Function<PFN_vkDestroyRenderPass>("vkDestroyRenderPass")(context.device, pass, nullptr);
        pipeline = VK_NULL_HANDLE;
        layout = VK_NULL_HANDLE;
        modules = {};
        pass = VK_NULL_HANDLE;
    }
};

void Barrier(const Context& context, VkCommandBuffer commands, VkPipelineStageFlags from, VkPipelineStageFlags to, VkAccessFlags source, VkAccessFlags destination) {
    VkMemoryBarrier barrier{VK_STRUCTURE_TYPE_MEMORY_BARRIER};
    barrier.srcAccessMask = source;
    barrier.dstAccessMask = destination;
    context.Function<PFN_vkCmdPipelineBarrier>("vkCmdPipelineBarrier")(commands, from, to, 0, 1, &barrier, 0, nullptr, 0, nullptr);
}

std::vector<std::uint32_t> ResolveSamples(const Context& context, std::span<const std::uint32_t> samples) {
    const auto count = static_cast<std::uint32_t>(samples.size());
    Image multisampled(context, count, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    Image resolved(context, 1u, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
    FillPass fill(context, count);
    VkFramebufferCreateInfo framebufferInfo{VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    framebufferInfo.renderPass = fill.pass;
    framebufferInfo.attachmentCount = 1u;
    framebufferInfo.pAttachments = &multisampled.view;
    framebufferInfo.width = Width;
    framebufferInfo.height = Height;
    framebufferInfo.layers = 1u;
    VkFramebuffer fillFramebuffer = VK_NULL_HANDLE;
    Check(context.Function<PFN_vkCreateFramebuffer>("vkCreateFramebuffer")(context.device, &framebufferInfo, nullptr, &fillFramebuffer), "vkCreateFramebuffer packed float resolve test");
    {
        AgcDriver::Graphics::CommandBatch batch(context);
        std::array<std::uint32_t, 8> push{};
        std::copy(samples.begin(), samples.end(), push.begin());
        const VkClearValue clear{};
        VkRenderPassBeginInfo begin{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
        begin.renderPass = fill.pass;
        begin.framebuffer = fillFramebuffer;
        begin.renderArea = {{0, 0}, {Width, Height}};
        begin.clearValueCount = 1u;
        begin.pClearValues = &clear;
        context.Function<PFN_vkCmdBeginRenderPass>("vkCmdBeginRenderPass")(batch.Handle(), &begin, VK_SUBPASS_CONTENTS_INLINE);
        context.Function<PFN_vkCmdBindPipeline>("vkCmdBindPipeline")(batch.Handle(), VK_PIPELINE_BIND_POINT_GRAPHICS, fill.pipeline);
        context.Function<PFN_vkCmdPushConstants>("vkCmdPushConstants")(batch.Handle(), fill.layout, VK_SHADER_STAGE_FRAGMENT_BIT, 0u, sizeof(push), push.data());
        context.Function<PFN_vkCmdDraw>("vkCmdDraw")(batch.Handle(), 3u, 1u, 0u, 0u);
        context.Function<PFN_vkCmdEndRenderPass>("vkCmdEndRenderPass")(batch.Handle());
        batch.SubmitAndWait();
    }
    context.Function<PFN_vkDestroyFramebuffer>("vkDestroyFramebuffer")(context.device, fillFramebuffer, nullptr);

    AgcDriver::Graphics::PackedFloatResolve resolver(context);
    const std::array<VkImageView, 1> targets{resolved.view};
    AgcDriver::Graphics::Framebuffer framebuffer(context, resolver.RenderPass(), targets, {Width, Height});
    AgcDriver::Graphics::Buffer readback(context, static_cast<std::size_t>(Width) * Height * sizeof(std::uint32_t), VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    {
        AgcDriver::Graphics::CommandBatch batch(context);
        const auto commands = batch.Handle();
        VkImageMemoryBarrier toGeneral{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
        toGeneral.srcQueueFamilyIndex = toGeneral.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        toGeneral.image = resolved.image;
        toGeneral.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
        toGeneral.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        toGeneral.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        toGeneral.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        context.Function<PFN_vkCmdPipelineBarrier>("vkCmdPipelineBarrier")(commands, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &toGeneral);
        Barrier(context, commands, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT);
        std::shared_ptr<void> keep;
        resolver.Record(commands, nullptr, multisampled.view, framebuffer.Handle(), {Width, Height}, count, keep);
        Barrier(context, commands, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT);
        VkBufferImageCopy copy{};
        copy.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
        copy.imageExtent = {Width, Height, 1u};
        context.Function<PFN_vkCmdCopyImageToBuffer>("vkCmdCopyImageToBuffer")(commands, resolved.image, VK_IMAGE_LAYOUT_GENERAL, readback.Handle(), 1, &copy);
        Barrier(context, commands, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_HOST_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_HOST_READ_BIT);
        batch.SubmitAndWait();
    }
    readback.Invalidate();
    std::vector<std::uint32_t> words(static_cast<std::size_t>(Width) * Height);
    std::memcpy(words.data(), readback.Bytes().data(), words.size() * sizeof(std::uint32_t));
    return words;
}

bool ResolvesLikeTheReference(const Context& context) {
    VkImageFormatProperties supported{};
    Check(context.imageFormatProperties(context.physical, VK_FORMAT_R32_UINT, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 0u, &supported), "packed float resolve test format query");
    if (!context.sampleRateShading || (supported.sampleCounts & VK_SAMPLE_COUNT_2_BIT) == 0) {
        std::cout << "skipped, the device has no per-sample shading or 2x R32_UINT color targets\n";
        return false;
    }
    struct Case {
        const char* name;
        std::vector<std::uint32_t> samples;
    };
    const std::vector<Case> cases{
        {"two samples averaging 1.0 and 3.0", {Pack(1.0f, 1.0f, 1.0f), Pack(3.0f, 3.0f, 3.0f)}},
        {"two samples with different fields", {Pack(0.5f, 8.0f, 0.25f), Pack(1.5f, 2.0f, 0.75f)}},
        {"truncation of an average that needs one more mantissa bit", {Pack(1.0f, 1.0f, 1.0f), Pack(1.015625f, 1.015625f, 1.03125f)}},
        {"denormal and zero", {Pack(0.00003f, 0.0f, 0.00002f), Pack(0.00001f, 0.0f, 0.0f)}},
        {"large values", {Pack(60000.0f, 100.0f, 30000.0f), Pack(20000.0f, 300.0f, 10000.0f)}},
        {"infinity in one sample", {(31u << 6u) | 0u, Pack(1.0f, 1.0f, 1.0f)}},
        {"four samples", {Pack(1.0f, 2.0f, 4.0f), Pack(2.0f, 4.0f, 8.0f), Pack(3.0f, 6.0f, 12.0f), Pack(4.0f, 8.0f, 16.0f)}},
        {"eight samples", {Pack(0.0f, 1.0f, 2.0f), Pack(1.0f, 1.0f, 2.0f), Pack(2.0f, 1.0f, 2.0f), Pack(3.0f, 1.0f, 2.0f), Pack(4.0f, 1.0f, 2.0f), Pack(5.0f, 1.0f, 2.0f), Pack(6.0f, 1.0f, 2.0f), Pack(7.0f, 1.0f, 2.0f)}},
    };
    for (const auto& item : cases) {
        const auto count = static_cast<std::uint32_t>(item.samples.size());
        if ((supported.sampleCounts & count) == 0) continue;
        const auto expected = Reference(item.samples);
        const auto resolved = ResolveSamples(context, item.samples);
        for (std::size_t texel = 0; texel < resolved.size(); ++texel) {
            if (resolved[texel] != expected) {
                char text[256];
                std::snprintf(text, sizeof(text), "packed 11_11_10 float resolve, %s: texel %zu is 0x%08x, the average of the samples packed with truncation is 0x%08x", item.name, texel, resolved[texel], expected);
                throw std::runtime_error(text);
            }
        }
    }
    return true;
}

}

int main() {
    try {
        const auto device = OpenVulkanTestDevice();
        if (!device) return VulkanTestSkipped;
        const bool tested = ResolvesLikeTheReference(AgcDriver::VulkanDeviceTestAccess::ContextOf(*device));
        LibcRunShutdown_nid_postfix();
        if (!tested) return VulkanTestSkipped;
        std::cout << "packed float resolve tests passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        try {
            LibcRunShutdown_nid_postfix();
        } catch (...) {
        }
        return 1;
    }
}
