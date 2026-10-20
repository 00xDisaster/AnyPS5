#include "SceTypes.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <random>
#include <string>

extern "C" {
int APS5_VABI sceNpEntitlementAccessGetAddcontEntitlementInfo(std::uint32_t, const NpUnifiedEntitlementLabel*, NpEntitlementAccessAddcontEntitlementInfo*);
int APS5_VABI sceNpEntitlementAccessGetAddcontEntitlementInfoList(std::uint32_t, NpEntitlementAccessAddcontEntitlementInfo*, std::uint32_t, std::uint32_t*);
}

static void Require(bool value) { if (!value) std::abort(); }

int main() {
    const auto root = std::filesystem::temp_directory_path() / ("anyps5_guest_entitlements-" + std::to_string(std::random_device{}()));
    const auto directory = root / std::filesystem::path(u8"dlc-a\u00e7\u00e3o-\u65e5\u672c");
    std::filesystem::create_directories(directory);
    const auto file = directory / "anyps5-entitlements.ini";
    {
        std::ofstream ini(file, std::ios::binary);
        ini << "# owned add-ons\nFIRSTADDON0000\n  SECONDADDON ; second\n";
        Require(static_cast<bool>(ini));
    }
#ifdef _WIN32
    Require(_wputenv_s(L"ANYPS5_ENTITLEMENTS", file.wstring().c_str()) == 0);
#else
    Require(::setenv("ANYPS5_ENTITLEMENTS", file.c_str(), 1) == 0);
#endif
    NpEntitlementAccessAddcontEntitlementInfo list[3]{};
    std::uint32_t hits = 0;
    Require(sceNpEntitlementAccessGetAddcontEntitlementInfoList(0, list, 3, &hits) == 0);
    Require(hits == 2);
    Require(std::strcmp(list[0].entitlement_label.data, "FIRSTADDON0000") == 0);
    Require(std::strcmp(list[1].entitlement_label.data, "SECONDADDON") == 0);
    NpUnifiedEntitlementLabel label{};
    std::memcpy(label.data, "SECONDADDON", 11);
    NpEntitlementAccessAddcontEntitlementInfo info{};
    Require(sceNpEntitlementAccessGetAddcontEntitlementInfo(0, &label, &info) == 0);
    Require(std::strcmp(info.entitlement_label.data, "SECONDADDON") == 0 && info.package_type == 3 && info.download_status == 4);
    std::filesystem::remove_all(root);
    std::puts("Guest entitlements passed");
    return 0;
}
