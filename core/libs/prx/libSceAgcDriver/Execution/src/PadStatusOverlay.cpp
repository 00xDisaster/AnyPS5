#include "prx/libSceAgcDriver/Execution/include/PadStatusOverlay.hpp"
#include <algorithm>
#include <cstdlib>
#include <mutex>
#include <stdexcept>
#include <string_view>

namespace AgcDriver {
namespace {

bool batteryRank(PadPower power, int& rank) {
    switch (power) {
        case PadPower::Empty: rank = 0; return true;
        case PadPower::Low: rank = 1; return true;
        case PadPower::Medium: rank = 2; return true;
        case PadPower::Full: rank = 3; return true;
        case PadPower::Unknown:
        case PadPower::Wired: break;
    }
    return false;
}

bool batteryDropped(PadPower before, PadPower after) {
    int from = 0;
    int to = 0;
    return batteryRank(before, from) && batteryRank(after, to) && to < from;
}

bool padEvent(const PadSlotStatus& before, const PadSlotStatus& after) {
    if (after.connected != before.connected) return true;
    if (!after.connected) return false;
    return after.kind != before.kind || after.name != before.name || batteryDropped(before.power, after.power);
}

}

PadStatusMode ParsePadStatusMode(const char* value) {
    const std::string_view mode = value != nullptr ? value : "";
    if (mode.empty() || mode == "0") return PadStatusMode::Off;
    if (mode == "1") return PadStatusMode::Events;
    throw std::runtime_error("ANYPS5_PAD_STATUS must be 0 or 1, not '" + std::string(mode) + "'");
}

PadStatusDisplay::PadStatusDisplay(PadStatusMode mode) : mode(mode) {
}

bool PadStatusDisplay::Observe(Clock::time_point now, const PadStatusSnapshot& snapshot) {
    const PadStatusSnapshot baseline;
    const auto& before = previous ? *previous : baseline;
    bool event = false;
    for (std::size_t slot = 0; slot < PadStatusSlots; ++slot) {
        event = event || padEvent(before.slots[slot], snapshot.slots[slot]);
        seen[slot] = seen[slot] || snapshot.slots[slot].connected;
    }
    previous = snapshot;
    if (event && mode == PadStatusMode::Events) shownUntil = now + Duration;
    return event;
}

void PadStatusDisplay::Request(Clock::time_point now) {
    shownUntil = now + Duration;
}

bool PadStatusDisplay::Visible(Clock::time_point now) const {
    return shownUntil.has_value() && now < *shownUntil;
}

namespace {

std::mutex displayMutex;

PadStatusDisplay& display() {
    static PadStatusDisplay shared(ParsePadStatusMode(std::getenv("ANYPS5_PAD_STATUS")));
    return shared;
}

}

}

extern "C" void AgcDriverPadStatusObserve_nid_postfix(const AgcDriver::PadStatusSnapshot* snapshot) {
    std::lock_guard lock(AgcDriver::displayMutex);
    AgcDriver::display().Observe(AgcDriver::PadStatusDisplay::Clock::now(), *snapshot);
}

extern "C" void AgcDriverPadStatusShow_nid_postfix() {
    std::lock_guard lock(AgcDriver::displayMutex);
    AgcDriver::display().Request(AgcDriver::PadStatusDisplay::Clock::now());
}
