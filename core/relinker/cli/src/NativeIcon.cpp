#include <elfpatcher/windows/WindowsIconResourceBuilder.hpp>
#include <domain/Types.hpp>
#include <fstream>

namespace Elfpatcher::Windows {

PeDirectory WindowsIconResourceBuilder::Build(const std::filesystem::path& iconPath, std::vector<PeSection>& sections, const std::uint32_t nextRva) const {
    if (iconPath.empty()) return {};
    std::error_code error;
    const bool exists = std::filesystem::exists(iconPath, error);
    if (error) throw Domain::RelinkerException("Cannot inspect Windows icon '" + iconPath.string() + "': " + error.message());
    if (!exists) return {};
    std::ifstream stream(iconPath, std::ios::binary | std::ios::ate);
    if (!stream) throw Domain::RelinkerException("Cannot open Windows icon '" + iconPath.string() + "'");
    const auto size = stream.tellg();
    if (size < 0 || static_cast<std::uint64_t>(size) > 0x7fffffffu - 184) throw Domain::RelinkerException("Invalid Windows icon file size: " + iconPath.string());
    std::vector<std::uint8_t> png(static_cast<std::size_t>(size));
    stream.seekg(0);
    if (!stream.read(reinterpret_cast<char*>(png.data()), size)) throw Domain::RelinkerException("Cannot read Windows icon '" + iconPath.string() + "'");
    return Build(png, iconPath, sections, nextRva);
}

}
