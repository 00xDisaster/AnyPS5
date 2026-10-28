#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_PADSTATUSOVERLAY_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_PADSTATUSOVERLAY_HPP

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include "prx/libSceAgcDriver/Execution/include/PerformanceOverlay.hpp"
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace AgcDriver {

inline constexpr std::size_t PadStatusSlots = 4;

enum class PadStatusMode : std::uint8_t { Off, Events };

enum class PadKind : std::uint8_t { Unknown, DualSense, DualShock4, DualShock3, Xbox, Switch, Other };

enum class PadPower : std::uint8_t { Unknown, Empty, Low, Medium, Full, Wired };

struct PadSlotStatus {
    bool connected = false;
    PadKind kind = PadKind::Unknown;
    std::string name;
    PadPower power = PadPower::Unknown;

    bool operator==(const PadSlotStatus&) const = default;
};

struct PadStatusSnapshot {
    std::array<PadSlotStatus, PadStatusSlots> slots;
};

struct PadStatusProvider {
    std::function<PadStatusSnapshot()> read;
};

struct PadStatusMetrics {
    static constexpr std::int32_t Columns = 32;
    static constexpr std::int32_t KindColumns = 12;
};

PadStatusMode ParsePadStatusMode(const char* value);
std::string FormatPadStatusLine(std::size_t slot, const PadSlotStatus& status);
OverlayCorner PadStatusCorner(OverlayCorner overlay);
OverlayContent ComposePadStatus(const std::vector<std::string>& lines);
OverlayLayout LayOutPadStatus(const std::vector<std::string>& lines, VkExtent2D extent, OverlayCorner overlay);
bool PadStatusDrawn(bool visible, bool keyboardShown);
std::optional<std::vector<std::string>> PadStatusOverlayLines();

class PadStatusDisplay {
public:
    using Clock = std::chrono::steady_clock;
    static constexpr Clock::duration Duration = std::chrono::seconds(4);

    explicit PadStatusDisplay(PadStatusMode mode);

    bool Observe(Clock::time_point now, const PadStatusSnapshot& snapshot);
    void Request(Clock::time_point now);
    bool Visible(Clock::time_point now) const;
    std::vector<std::string> Lines() const;

private:
    PadStatusMode mode;
    std::optional<PadStatusSnapshot> previous;
    std::array<bool, PadStatusSlots> seen{};
    std::optional<Clock::time_point> shownUntil;
};

}

extern "C" void AgcDriverPadStatusObserve_nid_postfix(const AgcDriver::PadStatusSnapshot* snapshot);
extern "C" void AgcDriverPadStatusShow_nid_postfix();

#endif
