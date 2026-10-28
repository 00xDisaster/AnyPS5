#include "prx/libSceAgcDriver/Execution/include/HostReport.hpp"
#include <algorithm>
#include <cctype>
#include <cstdio>

namespace AgcDriver {

namespace {

bool hardwareAddress(const std::string& token) {
    if (token.size() != 17) return false;
    for (std::size_t index = 0; index < token.size(); ++index) {
        const auto value = static_cast<unsigned char>(token[index]);
        if (index % 3 == 2) {
            if (value != ':' && value != '-') return false;
        } else if (std::isxdigit(value) == 0) {
            return false;
        }
    }
    return true;
}

std::string tokenPlaceholder(const std::string& token) {
    auto begin = token.find_first_not_of("(\"'[,;");
    auto end = token.find_last_not_of(")\"']',;");
    if (begin == std::string::npos || end == std::string::npos || end < begin) return token;
    const auto core = token.substr(begin, end - begin + 1);
    if (core.find("://") != std::string::npos) return "<url>";
    if (hardwareAddress(core)) return "<redacted>";
    const bool drive = core.size() >= 3 && std::isalpha(static_cast<unsigned char>(core[0])) != 0 && core[1] == ':' && (core[2] == '\\' || core[2] == '/');
    if (core[0] == '/' || core[0] == '~' || core.find('\\') != std::string::npos || drive) return "<path>";
    return token;
}

HostFacts sanitized(const HostFacts& facts) {
    const auto clean = [&](const std::string& text) { return RedactHostText(text); };
    const auto cleanAll = [&](std::vector<std::string> values) {
        for (auto& value : values) value = clean(value);
        return values;
    };
    HostFacts result = facts;
    result.redactions.clear();
    result.commit = clean(facts.commit);
    if (facts.os) result.os = clean(*facts.os);
    if (facts.cpuModel) result.cpuModel = clean(*facts.cpuModel);
    if (facts.cpuMissing) result.cpuMissing = cleanAll(*facts.cpuMissing);
    if (facts.sdlLinked) result.sdlLinked = clean(*facts.sdlLinked);
    if (facts.sdlCompiled) result.sdlCompiled = clean(*facts.sdlCompiled);
    if (facts.gpu) {
        auto& gpu = *result.gpu;
        gpu.name = clean(gpu.name);
        gpu.type = clean(gpu.type);
        gpu.driverName = clean(gpu.driverName);
        gpu.driverInfo = clean(gpu.driverInfo);
        gpu.candidates = cleanAll(gpu.candidates);
        gpu.presentModes = cleanAll(gpu.presentModes);
        gpu.requestedPresentMode = clean(gpu.requestedPresentMode);
        for (auto& requirement : gpu.requirements) {
            requirement.name = clean(requirement.name);
            requirement.kind = clean(requirement.kind);
        }
    }
    for (auto& entry : result.environment) {
        entry.first = clean(entry.first);
        entry.second = clean(entry.second);
    }
    return result;
}

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

std::string RedactHostText(const std::string& text) {
    std::string flat = text;
    for (auto& character : flat) {
        const auto value = static_cast<unsigned char>(character);
        if (value < 0x20 || value == 0x7f) character = ' ';
    }
    const auto& named = flat;
    std::string result;
    std::size_t cursor = 0;
    while (cursor <= named.size()) {
        const auto space = named.find(' ', cursor);
        const auto stop = space == std::string::npos ? named.size() : space;
        result += tokenPlaceholder(named.substr(cursor, stop - cursor));
        if (space == std::string::npos) break;
        result += ' ';
        cursor = space + 1;
    }
    return result;
}

std::string FormatHostReport(const HostFacts& original) {
    const HostFacts facts = sanitized(original);
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
