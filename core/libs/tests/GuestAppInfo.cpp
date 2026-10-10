#include "SceTypes.hpp"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>

extern "C" {
int APS5_VABI sceKernelGetAppInfo(int pid, void* info);
int APS5_VABI getpid_nid_postfix(void);
}

static constexpr int SCE_KERNEL_ERROR_ESRCH = static_cast<int>(0x80020003);
static constexpr int SCE_KERNEL_ERROR_EFAULT = static_cast<int>(0x8002000e);

static void Require(bool value) { if (!value) std::abort(); }

struct AppInfo {
    std::uint32_t appId;
    std::uint64_t unknown1;
    char titleId[14];
    char unknown2[0x3c];
};

int main() {
    const auto previous = std::filesystem::current_path();
    const auto root = std::filesystem::temp_directory_path() / ("guest_app_info_" + std::to_string(getpid_nid_postfix()));
    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root / "app0" / "sce_sys");
    std::ofstream(root / "app0" / "sce_sys" / "param.json") << "{\n  \"contentId\": \"IV0000-PPSA97358_00-XPSEMU0000000000\",\n  \"titleId\": \"PPSA97358\"\n}\n";
    std::filesystem::current_path(root);
    AppInfo info;
    std::memset(&info, 0x5a, sizeof(info));
    const int self = getpid_nid_postfix();
    Require(sceKernelGetAppInfo(self, &info) == 0);
    Require(info.appId == 1);
    Require(std::strcmp(info.titleId, "PPSA97358") == 0);
    Require(info.unknown2[0] == 0x5a);
    Require(sceKernelGetAppInfo(self + 1, &info) == SCE_KERNEL_ERROR_ESRCH);
    Require(sceKernelGetAppInfo(self, nullptr) == SCE_KERNEL_ERROR_EFAULT);
    std::filesystem::current_path(previous);
    std::filesystem::remove_all(root);
}
