#include "prx/libSceAgcDriver/Execution/include/HostReport.hpp"
#include <algorithm>
#include <cstdio>

namespace AgcDriver {

namespace {

std::string hex(std::uint32_t value) {
    char buffer[16];
    std::snprintf(buffer, sizeof buffer, "0x%08x", value);
    return buffer;
}

std::string hex16(std::uint32_t value) {
    char buffer[16];
    std::snprintf(buffer, sizeof buffer, "0x%04x", value);
    return buffer;
}

std::string vulkanVersion(std::uint32_t version) {
    return std::to_string((version >> 22) & 0x7Fu) + "." + std::to_string((version >> 12) & 0x3FFu) + "." + std::to_string(version & 0xFFFu);
}

std::string join(const std::vector<std::string>& values) {
    std::string text;
    for (const auto& value : values) {
        if (!text.empty()) text += ", ";
        text += value;
    }
    return text;
}

void line(std::string& output, const std::string& key, const std::string& value) {
    output += key;
    output += ": ";
    output += value;
    output += '\n';
}

void optionalLine(std::string& output, const std::string& key, const std::optional<std::string>& value) {
    line(output, key, value ? *value : std::string("unavailable"));
}

std::optional<std::string> text(const std::optional<std::uint64_t>& value) {
    if (!value) return std::nullopt;
    return std::to_string(*value);
}

}

std::string FormatHostReport(const HostFacts& facts) {
    std::string output = "AnyPS5 host report (format 1)\n";
    line(output, "commit", facts.commit.empty() ? "unknown" : facts.commit);
    output += "[system]\n";
    optionalLine(output, "os", facts.os);
    optionalLine(output, "cpu", facts.cpuModel);
    optionalLine(output, "cpu_threads", text(facts.cpuThreads));
    optionalLine(output, "memory_mib", text(facts.memoryMiB));
    if (facts.cpuMissing) line(output, "cpu_gate", facts.cpuMissing->empty() ? "passed" : "failed, lacks " + join(*facts.cpuMissing));
    else line(output, "cpu_gate", "unavailable");
    output += "[sdl]\n";
    optionalLine(output, "linked", facts.sdlLinked);
    optionalLine(output, "compiled", facts.sdlCompiled);
    output += "[gpu]\n";
    if (!facts.gpu) {
        line(output, "device", "none selected");
    } else {
        const auto& gpu = *facts.gpu;
        line(output, "device", gpu.name);
        line(output, "type", gpu.type);
        line(output, "vendor_id", hex16(gpu.vendorId));
        line(output, "device_id", hex16(gpu.deviceId));
        line(output, "vulkan_api", vulkanVersion(gpu.apiVersion));
        line(output, "driver", gpu.driverName.empty() ? "unknown" : gpu.driverName);
        line(output, "driver_info", gpu.driverInfo.empty() ? "unknown" : gpu.driverInfo);
        line(output, "driver_version_raw", hex(gpu.driverVersion));
        line(output, "subgroup_size", std::to_string(gpu.subgroupSize));
        line(output, "device_local_mib", std::to_string(gpu.deviceLocalMiB));
        line(output, "candidates", gpu.candidates.empty() ? "none" : join(gpu.candidates));
        auto presentModes = gpu.presentModes;
        std::sort(presentModes.begin(), presentModes.end());
        line(output, "present_modes", presentModes.empty() ? "unknown" : join(presentModes));
        line(output, "present_mode_requested", gpu.requestedPresentMode.empty() ? "default" : gpu.requestedPresentMode);
        output += "[vulkan]\n";
        auto requirements = gpu.requirements;
        std::sort(requirements.begin(), requirements.end(), [](const auto& left, const auto& right) {
            if (left.required != right.required) return left.required;
            if (left.kind != right.kind) return left.kind < right.kind;
            return left.name < right.name;
        });
        for (const auto& requirement : requirements) {
            line(output, requirement.name, std::string(requirement.kind) + " " + (requirement.required ? "required" : "optional") + " " + (requirement.present ? "present" : "missing"));
        }
    }
    output += "[shader_cache]\n";
    if (!facts.shaderCache) {
        line(output, "state", "unavailable");
    } else if (!facts.shaderCache->enabled) {
        line(output, "state", "disabled");
    } else if (!facts.shaderCache->present) {
        line(output, "state", "absent");
    } else {
        line(output, "state", "present");
        line(output, "size_mib", std::to_string(facts.shaderCache->sizeMiB));
    }
    output += "[environment]\n";
    auto environment = facts.environment;
    std::sort(environment.begin(), environment.end());
    if (environment.empty()) output += "none\n";
    for (const auto& entry : environment) line(output, entry.first, entry.second);
    return output;
}

}
