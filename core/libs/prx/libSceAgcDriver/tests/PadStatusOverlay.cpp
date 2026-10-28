#include "prx/libSceAgcDriver/Execution/include/PadStatusOverlay.hpp"
#include <chrono>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using AgcDriver::PadKind;
using AgcDriver::PadPower;
using AgcDriver::PadSlotStatus;
using AgcDriver::PadStatusDisplay;
using AgcDriver::PadStatusMode;
using AgcDriver::PadStatusSnapshot;

void require(bool condition, const std::string& reason) {
    if (!condition) throw std::runtime_error(reason);
}

PadStatusDisplay::Clock::time_point at(int milliseconds) {
    return PadStatusDisplay::Clock::time_point{} + std::chrono::milliseconds(milliseconds);
}

PadSlotStatus pad(PadKind kind, PadPower power, const std::string& name = {}) {
    return {true, kind, name, power};
}

PadStatusSnapshot snapshot(const std::vector<PadSlotStatus>& slots) {
    PadStatusSnapshot result;
    for (std::size_t index = 0; index < slots.size(); ++index) result.slots[index] = slots[index];
    return result;
}

bool parseRefused(const char* value) {
    try {
        AgcDriver::ParsePadStatusMode(value);
    } catch (const std::runtime_error& error) {
        return std::string(error.what()).find("ANYPS5_PAD_STATUS") != std::string::npos;
    }
    return false;
}

void modeTests() {
    require(AgcDriver::ParsePadStatusMode(nullptr) == PadStatusMode::Off, "an unset ANYPS5_PAD_STATUS is not off");
    require(AgcDriver::ParsePadStatusMode("") == PadStatusMode::Off, "an empty ANYPS5_PAD_STATUS is not off");
    require(AgcDriver::ParsePadStatusMode("0") == PadStatusMode::Off, "ANYPS5_PAD_STATUS=0 is not off");
    require(AgcDriver::ParsePadStatusMode("1") == PadStatusMode::Events, "ANYPS5_PAD_STATUS=1 is not the events mode");
    for (const char* value : {"2", "01", " 1", "1 ", "on", "true", "-1", "events", "00"}) require(parseRefused(value), std::string("ANYPS5_PAD_STATUS='") + value + "' is not refused with the variable's name");
    std::printf("pad status mode: unset, empty and 0 are off, 1 is events, anything else is refused\n");
}

void hiddenByDefaultTests() {
    PadStatusDisplay off(PadStatusMode::Off);
    require(!off.Visible(at(0)), "a new display is visible");
    require(off.Observe(at(0), snapshot({pad(PadKind::DualSense, PadPower::Wired)})), "a connect is not reported as an event");
    require(!off.Visible(at(0)) && !off.Visible(at(1)), "the mode 0 display shows a connect");
    require(off.Observe(at(100), snapshot({})), "a disconnect is not reported as an event");
    require(!off.Visible(at(100)), "the mode 0 display shows a disconnect");
    require(off.Observe(at(200), snapshot({pad(PadKind::Xbox, PadPower::Full)})), "a second connect is not reported");
    require(off.Observe(at(300), snapshot({pad(PadKind::Xbox, PadPower::Low)})) && !off.Visible(at(300)), "the mode 0 display shows a battery drop");
    PadStatusDisplay quiet(PadStatusMode::Events);
    require(!quiet.Observe(at(0), snapshot({})) && !quiet.Visible(at(0)), "an empty first observation shows the panel");
    for (int step = 1; step < 200; ++step) require(!quiet.Observe(at(step * 16), snapshot({})) && !quiet.Visible(at(step * 16)), "an unchanged snapshot shows the panel");
    std::printf("pad status: mode 0 and an idle mode 1 never show the panel\n");
}

void expiryTests() {
    PadStatusDisplay display(PadStatusMode::Events);
    display.Observe(at(0), snapshot({}));
    require(display.Observe(at(1000), snapshot({pad(PadKind::DualSense, PadPower::Wired)})), "a connect is not an event");
    require(display.Visible(at(1000)), "the panel is hidden when the controller connects");
    require(display.Visible(at(1001)) && display.Visible(at(4999)), "the panel hides before its 4 seconds are over");
    require(!display.Visible(at(5000)), "the panel is still shown 4 seconds after the event");
    require(!display.Visible(at(60000)), "the panel comes back by itself");
    require(PadStatusDisplay::Duration == std::chrono::seconds(4), "the panel's duration is not 4 seconds");
    std::printf("pad status: shown for 4 s after a connect, then gone\n");
}

void initialConnectTests() {
    PadStatusDisplay display(PadStatusMode::Events);
    require(display.Observe(at(0), snapshot({pad(PadKind::Xbox, PadPower::Medium)})) && display.Visible(at(0)), "a controller present at start does not show the panel");
    require(!display.Visible(at(4000)), "the start panel stays");
    std::printf("pad status: a controller present at the first observation counts as a connect\n");
}

void extensionTests() {
    PadStatusDisplay display(PadStatusMode::Events);
    display.Observe(at(0), snapshot({}));
    display.Observe(at(0), snapshot({pad(PadKind::DualSense, PadPower::Wired)}));
    require(!display.Observe(at(3000), snapshot({pad(PadKind::DualSense, PadPower::Wired)})), "an unchanged snapshot is an event");
    require(!display.Visible(at(4000)), "an observation without an event extended the panel");
    PadStatusDisplay extended(PadStatusMode::Events);
    extended.Observe(at(0), snapshot({}));
    extended.Observe(at(0), snapshot({pad(PadKind::DualSense, PadPower::Wired)}));
    require(extended.Observe(at(3000), snapshot({pad(PadKind::DualSense, PadPower::Wired), pad(PadKind::Xbox, PadPower::Full)})), "a second connect is not an event");
    require(extended.Visible(at(6999)), "a new event did not restart the 4 seconds");
    require(!extended.Visible(at(7000)), "the restarted panel outlives its 4 seconds");
    extended.Observe(at(6000), snapshot({pad(PadKind::DualSense, PadPower::Wired)}));
    require(extended.Visible(at(9999)) && !extended.Visible(at(10000)), "a disconnect did not restart the 4 seconds");
    std::printf("pad status: a new event restarts the 4 s, a quiet observation does not\n");
}

void batteryTests() {
    struct Case {
        PadPower before;
        PadPower after;
        bool event;
    };
    const Case cases[] = {
        {PadPower::Full, PadPower::Medium, true},
        {PadPower::Medium, PadPower::Low, true},
        {PadPower::Low, PadPower::Empty, true},
        {PadPower::Full, PadPower::Empty, true},
        {PadPower::Full, PadPower::Low, true},
        {PadPower::Medium, PadPower::Medium, false},
        {PadPower::Low, PadPower::Medium, false},
        {PadPower::Empty, PadPower::Full, false},
        {PadPower::Medium, PadPower::Full, false},
        {PadPower::Full, PadPower::Wired, false},
        {PadPower::Wired, PadPower::Low, false},
        {PadPower::Wired, PadPower::Wired, false},
        {PadPower::Unknown, PadPower::Low, false},
        {PadPower::Medium, PadPower::Unknown, false},
        {PadPower::Unknown, PadPower::Unknown, false},
    };
    for (const auto& item : cases) {
        PadStatusDisplay display(PadStatusMode::Events);
        display.Observe(at(0), snapshot({pad(PadKind::Xbox, item.before)}));
        const bool shownAtConnect = display.Visible(at(1000));
        require(shownAtConnect, "the connect did not show the panel");
        const bool event = display.Observe(at(10000), snapshot({pad(PadKind::Xbox, item.after)}));
        require(event == item.event, "a battery change was classified wrongly");
        require(display.Visible(at(10001)) == item.event, "a battery change shows the panel wrongly");
    }
    PadStatusDisplay second(PadStatusMode::Events);
    second.Observe(at(0), snapshot({pad(PadKind::Xbox, PadPower::Full), pad(PadKind::DualSense, PadPower::Full)}));
    require(second.Observe(at(10000), snapshot({pad(PadKind::Xbox, PadPower::Full), pad(PadKind::DualSense, PadPower::Low)})) && second.Visible(at(10001)), "a drop on the second slot is not an event");
    std::printf("pad status: only a drop to a lower battery class is an event\n");
}

void replacementTests() {
    PadStatusDisplay display(PadStatusMode::Events);
    display.Observe(at(0), snapshot({pad(PadKind::Xbox, PadPower::Full)}));
    require(display.Observe(at(10000), snapshot({pad(PadKind::DualSense, PadPower::Full)})) && display.Visible(at(10001)), "swapping the controller in a slot is not an event");
    require(display.Observe(at(20000), snapshot({pad(PadKind::Other, PadPower::Full, "ONE")})) && display.Visible(at(20001)), "a controller of another kind is not an event");
    require(display.Observe(at(30000), snapshot({pad(PadKind::Other, PadPower::Full, "TWO")})) && display.Visible(at(30001)), "a controller of the same kind and another name is not an event");
    require(!display.Observe(at(40000), snapshot({pad(PadKind::Other, PadPower::Full, "TWO")})) && !display.Visible(at(40000)), "the same controller is an event");
    std::printf("pad status: swapping the controller in a slot is an event\n");
}

void requestTests() {
    PadStatusDisplay off(PadStatusMode::Off);
    off.Request(at(500));
    require(off.Visible(at(500)) && off.Visible(at(4499)), "a request does not show the panel in mode 0");
    require(!off.Visible(at(4500)), "a request in mode 0 stays past 4 seconds");
    off.Request(at(3000));
    require(off.Visible(at(6999)) && !off.Visible(at(7000)), "a second request does not restart the 4 seconds");
    PadStatusDisplay events(PadStatusMode::Events);
    events.Observe(at(0), snapshot({pad(PadKind::Xbox, PadPower::Full)}));
    events.Request(at(3000));
    require(events.Visible(at(6999)) && !events.Visible(at(7000)), "a request on a panel that is shown does not restart the 4 seconds");
    events.Request(at(20000));
    require(events.Visible(at(20001)) && !events.Visible(at(24000)), "a request after the panel expired does not show it");
    std::printf("pad status: a request shows the panel for 4 s in both modes\n");
}

void providerTests() {
    std::vector<PadStatusSnapshot> script{snapshot({}), snapshot({pad(PadKind::DualSense, PadPower::Wired)}), snapshot({pad(PadKind::DualSense, PadPower::Wired)}), snapshot({})};
    std::size_t step = 0;
    AgcDriver::PadStatusProvider provider{[&] { return script[step]; }};
    PadStatusDisplay display(PadStatusMode::Events);
    std::vector<bool> shown;
    for (step = 0; step < script.size(); ++step) {
        display.Observe(at(static_cast<int>(step) * 1000), provider.read());
        shown.push_back(display.Visible(at(static_cast<int>(step) * 1000)));
    }
    require((shown == std::vector<bool>{false, true, true, true}), "the injected provider did not drive the display");
    require(display.Visible(at(6999)) && !display.Visible(at(7000)), "the last event of the script did not set the expiry");
    std::printf("pad status: an injected provider drives the display without SDL\n");
}

}

int main() try {
    modeTests();
    hiddenByDefaultTests();
    expiryTests();
    initialConnectTests();
    extensionTests();
    batteryTests();
    replacementTests();
    requestTests();
    providerTests();
    std::printf("pad status overlay tests passed\n");
    return 0;
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
}
