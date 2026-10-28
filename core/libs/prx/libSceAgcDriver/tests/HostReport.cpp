#include "prx/libSceAgcDriver/Execution/include/HostReport.hpp"
#include <algorithm>
#include <cstdio>
#include <string>

namespace {

int failures = 0;

void Check(bool condition, const char* what) {
    if (condition) return;
    std::fprintf(stderr, "failed: %s\n", what);
    ++failures;
}

AgcDriver::HostFacts SampleFacts() {
    AgcDriver::HostFacts facts;
    facts.commit = "0123456789ab";
    facts.os = "Linux 6.1.0-test";
    facts.cpuModel = "Test CPU 9000";
    facts.cpuThreads = 12;
    facts.memoryMiB = 32000;
    facts.cpuMissing = std::vector<std::string>{};
    facts.sdlLinked = "2.30.0";
    facts.sdlCompiled = "2.30.1";
    AgcDriver::HostGpuFacts gpu;
    gpu.name = "Test GPU";
    gpu.type = "discrete";
    gpu.driverName = "test-driver";
    gpu.driverInfo = "Test 1.2.3";
    gpu.vendorId = 0x10de;
    gpu.deviceId = 0x2504;
    gpu.apiVersion = (1u << 22) | (3u << 12) | 280u;
    gpu.driverVersion = 0x12345678;
    gpu.subgroupSize = 32;
    gpu.deviceLocalMiB = 12288;
    gpu.candidates = {"Test GPU", "Other GPU"};
    gpu.presentModes = {"mailbox", "fifo", "immediate"};
    gpu.requirements = {
        {"VK_KHR_swapchain", "extension", true, true},
        {"shaderInt64", "feature", true, true},
        {"VK_EXT_memory_budget", "extension", false, true},
        {"VK_KHR_maintenance5", "extension", false, false},
    };
    facts.gpu = gpu;
    facts.shaderCache = AgcDriver::HostShaderCacheFacts{true, true, 512};
    facts.environment = {{"ANYPS5_OVERLAY", "1"}, {"APS5_PROFILE_GPU", "1"}};
    return facts;
}

constexpr const char* Golden =
    "AnyPS5 host report (format 1)\n"
    "commit: 0123456789ab\n"
    "[system]\n"
    "os: Linux 6.1.0-test\n"
    "cpu: Test CPU 9000\n"
    "cpu_threads: 12\n"
    "memory_mib: 32000\n"
    "cpu_gate: passed\n"
    "[sdl]\n"
    "linked: 2.30.0\n"
    "compiled: 2.30.1\n"
    "[gpu]\n"
    "device: Test GPU\n"
    "type: discrete\n"
    "vendor_id: 0x10de\n"
    "device_id: 0x2504\n"
    "vulkan_api: 1.3.280\n"
    "driver: test-driver\n"
    "driver_info: Test 1.2.3\n"
    "driver_version_raw: 0x12345678\n"
    "subgroup_size: 32\n"
    "device_local_mib: 12288\n"
    "candidates: Test GPU, Other GPU\n"
    "present_modes: fifo, immediate, mailbox\n"
    "present_mode_requested: default\n"
    "[vulkan]\n"
    "VK_KHR_swapchain: extension required present\n"
    "shaderInt64: feature required present\n"
    "VK_EXT_memory_budget: extension optional present\n"
    "VK_KHR_maintenance5: extension optional missing\n"
    "[shader_cache]\n"
    "state: present\n"
    "size_mib: 512\n"
    "[environment]\n"
    "ANYPS5_OVERLAY: 1\n"
    "APS5_PROFILE_GPU: 1\n";

void GoldenOutput() {
    Check(AgcDriver::FormatHostReport(SampleFacts()) == Golden, "the report differs from the golden text");
}

void OrderIsStable() {
    auto shuffled = SampleFacts();
    std::reverse(shuffled.environment.begin(), shuffled.environment.end());
    std::reverse(shuffled.gpu->requirements.begin(), shuffled.gpu->requirements.end());
    std::reverse(shuffled.gpu->presentModes.begin(), shuffled.gpu->presentModes.end());
    Check(AgcDriver::FormatHostReport(shuffled) == Golden, "the entry order of the input changed the report");
    Check(AgcDriver::FormatHostReport(SampleFacts()) == AgcDriver::FormatHostReport(SampleFacts()), "two formats of the same facts differ");
}

void MissingFactsAreNamed() {
    AgcDriver::HostFacts facts;
    const auto text = AgcDriver::FormatHostReport(facts);
    Check(text.find("os: unavailable\n") != std::string::npos, "an absent OS is not reported as unavailable");
    Check(text.find("cpu_gate: unavailable\n") != std::string::npos, "an unknown CPU gate is not reported as unavailable");
    Check(text.find("device: none selected\n") != std::string::npos, "an absent GPU is not reported");
    Check(text.find("state: unavailable\n") != std::string::npos, "an unknown shader cache is not reported as unavailable");
    Check(text.find("commit: unknown\n") != std::string::npos, "an empty commit is not reported as unknown");
    facts.cpuMissing = std::vector<std::string>{"AVX2", "BMI2"};
    facts.shaderCache = AgcDriver::HostShaderCacheFacts{false, false, 0};
    const auto failed = AgcDriver::FormatHostReport(facts);
    Check(failed.find("cpu_gate: failed, lacks AVX2, BMI2\n") != std::string::npos, "a failed CPU gate does not list the missing extensions");
    Check(failed.find("state: disabled\n") != std::string::npos, "a disabled shader cache is not reported");
    facts.shaderCache = AgcDriver::HostShaderCacheFacts{true, false, 0};
    Check(AgcDriver::FormatHostReport(facts).find("state: absent\n") != std::string::npos, "an absent shader cache directory is not reported");
}

}

int main() {
    GoldenOutput();
    OrderIsStable();
    MissingFactsAreNamed();
    return failures == 0 ? 0 : 1;
}
