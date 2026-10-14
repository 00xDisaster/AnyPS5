#include "prx/libSceAgcDriver/Execution/include/PerformanceOverlay.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using AgcDriver::OverlayContent;
using AgcDriver::OverlayCorner;
using AgcDriver::OverlayMetrics;
using AgcDriver::OverlayRect;
using AgcDriver::OverlayStatistics;
using Clock = OverlayStatistics::Clock;
using namespace std::chrono_literals;

constexpr Clock::time_point Start = Clock::time_point{} + std::chrono::hours(1);

void require(bool condition, const std::string& reason) {
    if (!condition) throw std::runtime_error(reason);
}

bool near(double value, double expected, double tolerance = 1e-9) {
    return std::abs(value - expected) <= tolerance;
}

void fpsWindowTests() {
    OverlayStatistics statistics;
    require(statistics.Fps() == 0 && statistics.LastFrameMs() == 0 && statistics.FrameTimes().empty(), "a new overlay shows statistics");
    statistics.AddPresent(Start, 0, 0);
    require(statistics.Fps() == 0, "one present reads as a frame rate");
    for (int i = 1; i <= 120; ++i) statistics.AddPresent(Start + i * 10ms, 0, 0);
    require(near(statistics.Fps(), 100.0), "a present every 10 ms reads as " + std::to_string(statistics.Fps()) + " FPS, not 100");
    statistics.AddPresent(Start + 1700ms, 0, 0);
    require(near(statistics.Fps(), 51.0), "after a 500 ms stall the last second holds 52 presents, read as " + std::to_string(statistics.Fps()) + " FPS, not 51");
    statistics.AddPresent(Start + 3700ms, 0, 0);
    require(near(statistics.Fps(), 0.5), "after a 2 s stall the last second holds one present, read as " + std::to_string(statistics.Fps()) + " FPS, not the last frame's 0.5");
    statistics.AddPresent(Start + 3750ms, 0, 0);
    require(near(statistics.Fps(), 20.0), "two presents 50 ms apart read as " + std::to_string(statistics.Fps()) + " FPS, not 20");
    bool refused = false;
    try {
        statistics.AddPresent(Start + 3740ms, 0, 0);
    } catch (const std::exception&) {
        refused = true;
    }
    require(refused, "a present earlier than the previous one was accepted");
    statistics.Reset();
    require(statistics.Fps() == 0 && statistics.LastFrameMs() == 0 && statistics.FrameTimes().empty(), "a reset kept statistics");
}

void frameRingTests() {
    OverlayStatistics statistics;
    auto at = Start;
    statistics.AddPresent(at, 0, 0);
    for (int i = 1; i <= 4; ++i) {
        at += std::chrono::milliseconds(i);
        statistics.AddPresent(at, 0, 0);
    }
    const auto partial = statistics.FrameTimes();
    require(partial.size() == 4 && near(partial[0], 1) && near(partial[3], 4), "the frame times of a short run are not in present order");
    for (int i = 5; i <= 150; ++i) {
        at += std::chrono::milliseconds(i);
        statistics.AddPresent(at, 0, 0);
    }
    const auto times = statistics.FrameTimes();
    require(times.size() == OverlayStatistics::FrameHistory, "the ring holds " + std::to_string(times.size()) + " frames, not 120");
    for (std::size_t i = 0; i < times.size(); ++i) require(near(times[i], 31.0 + static_cast<double>(i)), "frame " + std::to_string(i) + " of the ring is " + std::to_string(times[i]) + " ms");
    require(near(statistics.LastFrameMs(), 150), "the last frame is not the newest");
    const auto range = statistics.FrameTimeRange();
    require(near(range.minimum, 31) && near(range.maximum, 150) && near(range.average, 90.5), "the graph range of 31..150 ms reads as " + std::to_string(range.minimum) + "/" + std::to_string(range.average) + "/" + std::to_string(range.maximum));
}

void counterTests() {
    OverlayStatistics statistics;
    constexpr std::uint64_t Earlier = 1'000'000'000u;
    for (std::uint64_t i = 0; i <= 60; ++i) statistics.AddPresent(Start + i * 16ms, Earlier + i * 4'000'000u, Earlier + i * 1'500'000u);
    require(near(statistics.GpuMsPerFrame(), 4.0) && near(statistics.DriverMsPerFrame(), 1.5), "4 ms of GPU and 1.5 ms of driver time per frame read as " + std::to_string(statistics.GpuMsPerFrame()) + " and " + std::to_string(statistics.DriverMsPerFrame()));
    statistics.AddPresent(Start + 61 * 16ms, 5, 5);
    require(statistics.GpuMsPerFrame() == 0 && statistics.DriverMsPerFrame() == 0, "a counter that went back is read as time");
}

void formatTests() {
    const std::pair<double, const char*> cases[] = {{59.94, "59.9"}, {0, "0.0"}, {99.94, "99.9"}, {99.96, "100"}, {144.4, "144"}, {12345, "9999"}, {-3, "0.0"}, {std::nan(""), "0.0"}};
    for (const auto& [value, expected] : cases) require(AgcDriver::FormatOverlayValue(value) == expected, std::to_string(value) + " is shown as '" + AgcDriver::FormatOverlayValue(value) + "', not '" + expected + "'");
    const std::pair<double, const char*> rates[] = {{0.5, "0.50"}, {1000.0 / 12000.0, "0.08"}, {0.05, "0.05"}, {0.004, "0.00"}, {0.994, "0.99"}, {0.996, "1.0"}, {1.5, "1.5"}, {59.94, "59.9"}, {144.4, "144"}, {12345, "9999"}, {0, "0.0"}, {-1, "0.0"}, {std::nan(""), "0.0"}};
    for (const auto& [value, expected] : rates) require(AgcDriver::FormatOverlayRate(value) == expected, "a rate of " + std::to_string(value) + " is shown as '" + AgcDriver::FormatOverlayRate(value) + "', not '" + expected + "'");
}

void lowRateTests() {
    OverlayStatistics slow;
    constexpr std::uint64_t Gpu[] = {30'000'000u, 45'000'000u, 20'000'000u};
    constexpr std::uint64_t Driver[] = {12'000'000u, 20'000'000u, 8'000'000u};
    std::uint64_t gpuTotal = 0;
    std::uint64_t driverTotal = 0;
    slow.AddPresent(Start, 0, 0);
    require(slow.Fps() == 0 && slow.GpuMsPerFrame() == 0 && slow.DriverMsPerFrame() == 0, "a single present reads as a frame");
    for (int i = 0; i < 3; ++i) {
        gpuTotal += Gpu[i];
        driverTotal += Driver[i];
        slow.AddPresent(Start + (i + 1) * 2s, gpuTotal, driverTotal);
        require(near(slow.Fps(), 0.5) && near(slow.LastFrameMs(), 2000), "a present every 2 s reads as " + std::to_string(slow.Fps()) + " FPS and " + std::to_string(slow.LastFrameMs()) + " ms, not 0.5 and 2000");
        require(near(slow.GpuMsPerFrame(), static_cast<double>(Gpu[i]) / 1e6) && near(slow.DriverMsPerFrame(), static_cast<double>(Driver[i]) / 1e6), "frame " + std::to_string(i + 1) + " every 2 s reads " + std::to_string(slow.GpuMsPerFrame()) + " ms of GPU and " + std::to_string(slow.DriverMsPerFrame()) + " ms of driver time, not its own");
    }
    const auto everyTwoSeconds = AgcDriver::ComposeOverlay(slow, true, true);
    require(everyTwoSeconds.above[0] == "FPS: 0.50" && everyTwoSeconds.above[1] == "FRAME: 2000 MS", "a present every 2 s shows '" + everyTwoSeconds.above[0] + "' and '" + everyTwoSeconds.above[1] + "'");
    require(everyTwoSeconds.below[0] == "MIN/AVG/MAX: 2000/2000/2000" && everyTwoSeconds.below[1] == "GPU: 20.0 MS" && everyTwoSeconds.below[2] == "DRIVER: 8.0 MS", "a present every 2 s shows '" + everyTwoSeconds.below[0] + "', '" + everyTwoSeconds.below[1] + "' and '" + everyTwoSeconds.below[2] + "'");

    OverlayStatistics glacial;
    glacial.AddPresent(Start, 0, 0);
    for (int i = 1; i <= 5; ++i) glacial.AddPresent(Start + i * 12s, static_cast<std::uint64_t>(i) * 250'000'000u, static_cast<std::uint64_t>(i) * 12'500'000'000u);
    require(near(glacial.Fps(), 1000.0 / 12000.0) && near(glacial.LastFrameMs(), 12000) && near(glacial.GpuMsPerFrame(), 250) && near(glacial.DriverMsPerFrame(), 12500), "a present every 12 s reads as " + std::to_string(glacial.Fps()) + " FPS, " + std::to_string(glacial.LastFrameMs()) + " ms, " + std::to_string(glacial.GpuMsPerFrame()) + " and " + std::to_string(glacial.DriverMsPerFrame()) + " ms");
    const auto everyTwelveSeconds = AgcDriver::ComposeOverlay(glacial, true, true);
    require(everyTwelveSeconds.above[0] == "FPS: 0.08" && everyTwelveSeconds.above[1] == "FRAME: 9999 MS", "a present every 12 s shows '" + everyTwelveSeconds.above[0] + "' and '" + everyTwelveSeconds.above[1] + "'");
    require(everyTwelveSeconds.below[0] == "MIN/AVG/MAX: 9999/9999/9999" && everyTwelveSeconds.below[1] == "GPU: 250 MS" && everyTwelveSeconds.below[2] == "DRIVER: 9999 MS", "a present every 12 s shows '" + everyTwelveSeconds.below[0] + "', '" + everyTwelveSeconds.below[1] + "' and '" + everyTwelveSeconds.below[2] + "'");
    require(everyTwelveSeconds.frameTimes == std::vector<double>(5, 12000.0), "the graph does not hold the five 12 s frames");
    const auto layout = AgcDriver::LayOutOverlay(everyTwelveSeconds, {1920, 1080}, OverlayCorner::TopLeft);
    std::size_t bars = 0;
    for (const auto& rect : layout.rects) {
        if (rect.color != static_cast<std::uint32_t>(AgcDriver::OverlayColor::Graph)) continue;
        ++bars;
        require(rect.height == OverlayMetrics::GraphHeight * layout.scale / 2, "steady 12 s frames do not reach half the graph");
    }
    require(bars == 5, "the graph has " + std::to_string(bars) + " bars for five 12 s frames");

    OverlayStatistics rising;
    auto at = Start;
    std::uint64_t total = 0;
    rising.AddPresent(at, 0, 0);
    const auto step = [&](Clock::duration interval, std::uint64_t gpuNanoseconds) {
        at += interval;
        total += gpuNanoseconds;
        rising.AddPresent(at, total, 0);
    };
    const auto check = [&](double fps, double gpuMs, const char* shown, const std::string& when) {
        require(near(rising.Fps(), fps) && near(rising.GpuMsPerFrame(), gpuMs) && AgcDriver::FormatOverlayRate(rising.Fps()) == shown, when + " reads as " + std::to_string(rising.Fps()) + " FPS ('" + AgcDriver::FormatOverlayRate(rising.Fps()) + "') and " + std::to_string(rising.GpuMsPerFrame()) + " ms of GPU time, not " + std::to_string(fps) + " ('" + shown + "') and " + std::to_string(gpuMs));
    };
    step(2s, 40'000'000u);
    check(0.5, 40, "0.50", "a 2 s frame");
    step(1250ms, 30'000'000u);
    check(0.8, 30, "0.80", "a 1.25 s frame");
    step(1001ms, 20'000'000u);
    check(1000.0 / 1001.0, 20, "1.0", "a 1.001 s frame, the last second holding one present");
    step(1000ms, 10'000'000u);
    check(1.0, 10, "1.0", "a 1 s frame, the last second holding two presents");
    step(750ms, 6'000'000u);
    check(1.0 / 0.75, 6, "1.3", "a 750 ms frame");
    for (int i = 0; i < 8; ++i) step(250ms, i % 2 == 0 ? 2'000'000u : 6'000'000u);
    check(4.0, 4.0, "4.0", "four presents a second, alternating 2 and 6 ms of GPU time");
    step(3s, 9'000'000u);
    check(1.0 / 3.0, 9, "0.33", "a 3 s frame after the fast ones");
}

void settingsTests() {
    const auto hidden = AgcDriver::ParseOverlaySettings(nullptr, nullptr);
    require(!hidden.visible && hidden.corner == OverlayCorner::TopLeft, "the overlay is not off and top-left by default");
    require(!AgcDriver::ParseOverlaySettings("", "").visible && !AgcDriver::ParseOverlaySettings("0", nullptr).visible, "an empty or 0 ANYPS5_OVERLAY shows the overlay");
    require(AgcDriver::ParseOverlaySettings("1", nullptr).visible, "ANYPS5_OVERLAY=1 does not show the overlay");
    const std::pair<const char*, OverlayCorner> corners[] = {{"tl", OverlayCorner::TopLeft}, {"tr", OverlayCorner::TopRight}, {"bl", OverlayCorner::BottomLeft}, {"br", OverlayCorner::BottomRight}};
    for (const auto& [name, corner] : corners) require(AgcDriver::ParseOverlaySettings("1", name).corner == corner, std::string("ANYPS5_OVERLAY_POS=") + name + " picks another corner");
    const auto refused = [](const char* enabled, const char* position, const char* variable) {
        try {
            static_cast<void>(AgcDriver::ParseOverlaySettings(enabled, position));
        } catch (const std::exception& error) {
            return std::string(error.what()).find(variable) != std::string::npos;
        }
        return false;
    };
    require(refused("yes", nullptr, "ANYPS5_OVERLAY") && refused("2", nullptr, "ANYPS5_OVERLAY"), "an ANYPS5_OVERLAY value other than 1 or 0 was accepted or not named");
    require(refused("1", "center", "ANYPS5_OVERLAY_POS") && refused(nullptr, "TL", "ANYPS5_OVERLAY_POS"), "an unknown ANYPS5_OVERLAY_POS was accepted or not named");
}

void composeTests() {
    OverlayStatistics statistics;
    for (std::uint64_t i = 0; i <= 100; ++i) statistics.AddPresent(Start + i * 10ms, i * 2'000'000u, i * 500'000u);
    const auto plain = AgcDriver::ComposeOverlay(statistics, false, false);
    require(plain.above.size() == 2 && plain.above[0] == "FPS: 100" && plain.above[1] == "FRAME: 10.0 MS", "the frame lines read '" + plain.above[0] + "' and '" + plain.above[1] + "'");
    require(plain.below.size() == 1 && plain.below[0] == "MIN/AVG/MAX: 10.0/10.0/10.0", "the graph line reads '" + plain.below[0] + "'");
    require(plain.frameTimes.size() == 100, "the graph does not hold every frame time");
    const auto profiled = AgcDriver::ComposeOverlay(statistics, true, true);
    require(profiled.below.size() == 3 && profiled.below[1] == "GPU: 2.0 MS" && profiled.below[2] == "DRIVER: 0.5 MS", "the profiled lines read '" + profiled.below[1] + "' and '" + profiled.below[2] + "'");
    require(AgcDriver::ComposeOverlay(statistics, true, false).below.size() == 2 && AgcDriver::ComposeOverlay(statistics, false, true).below[1].rfind("DRIVER", 0) == 0, "a line is shown without its source");
}

std::vector<std::uint8_t> coverage(const AgcDriver::OverlayLayout& layout, std::uint32_t color) {
    const auto& bounds = layout.bounds;
    std::vector<std::uint8_t> covered(static_cast<std::size_t>(bounds.width) * static_cast<std::size_t>(bounds.height), 0);
    for (const auto& rect : layout.rects) {
        if (rect.color != color) continue;
        for (std::int32_t y = rect.y; y < rect.y + rect.height; ++y) {
            for (std::int32_t x = rect.x; x < rect.x + rect.width; ++x) ++covered[static_cast<std::size_t>(y - bounds.y) * static_cast<std::size_t>(bounds.width) + static_cast<std::size_t>(x - bounds.x)];
        }
    }
    return covered;
}

void layoutTests() {
    OverlayContent content{{"FPS: 60.0", "FRAME: 16.7 MS"}, {}, {"MIN/AVG/MAX: 16.7/16.7/16.7"}};
    for (int i = 0; i < 40; ++i) content.frameTimes.push_back(i == 39 ? 50.0 : 10.0);
    const std::pair<VkExtent2D, std::int32_t> scales[] = {{{1280, 720}, 2}, {{1920, 1080}, 2}, {{2560, 1440}, 3}, {{3840, 2160}, 5}, {{320, 180}, 1}, {{2560, 720}, 2}};
    for (const auto& [extent, scale] : scales) require(AgcDriver::OverlayScale(extent, content) == scale, std::to_string(extent.width) + "x" + std::to_string(extent.height) + " draws the overlay at scale " + std::to_string(AgcDriver::OverlayScale(extent, content)) + ", not " + std::to_string(scale));

    const VkExtent2D extent{1920, 1080};
    const auto layout = AgcDriver::LayOutOverlay(content, extent, OverlayCorner::TopLeft);
    const auto s = layout.scale;
    const auto margin = OverlayMetrics::Margin * s;
    require(layout.bounds.x == margin && layout.bounds.y == margin && layout.bounds.width == (OverlayMetrics::InnerWidth + 2 * OverlayMetrics::Padding) * s, "the top-left panel is not at the margin");
    require(layout.rects.front().x == layout.bounds.x && layout.rects.front().width == layout.bounds.width && layout.rects.front().color == static_cast<std::uint32_t>(AgcDriver::OverlayColor::Background), "the background is not drawn first over the whole panel");
    for (const auto& rect : layout.rects) {
        require(rect.width > 0 && rect.height > 0 && rect.x >= layout.bounds.x && rect.y >= layout.bounds.y && rect.x + rect.width <= layout.bounds.x + layout.bounds.width && rect.y + rect.height <= layout.bounds.y + layout.bounds.height, "a rectangle leaves the panel");
    }

    const auto text = coverage(layout, static_cast<std::uint32_t>(AgcDriver::OverlayColor::Text));
    const auto lineTop = [&](std::size_t line) { return OverlayMetrics::Padding + static_cast<std::int32_t>(line) * OverlayMetrics::LinePitch; };
    const auto checkLine = [&](const std::string& line, std::int32_t top) {
        for (std::size_t column = 0; column < line.size(); ++column) {
            const auto& glyph = AgcDriver::OverlayGlyph(line[column]);
            for (std::int32_t gy = 0; gy < OverlayMetrics::GlyphHeight; ++gy) {
                for (std::int32_t gx = 0; gx < OverlayMetrics::Advance; ++gx) {
                    const bool set = gx < OverlayMetrics::GlyphWidth && (glyph[static_cast<std::size_t>(gy)] & (0x10u >> gx)) != 0;
                    for (std::int32_t py = 0; py < s; ++py) {
                        for (std::int32_t px = 0; px < s; ++px) {
                            const auto x = (OverlayMetrics::Padding + static_cast<std::int32_t>(column) * OverlayMetrics::Advance + gx) * s + px;
                            const auto y = (top + gy) * s + py;
                            require(text[static_cast<std::size_t>(y) * static_cast<std::size_t>(layout.bounds.width) + static_cast<std::size_t>(x)] == (set ? 1 : 0), "'" + std::string(1, line[column]) + "' of '" + line + "' is drawn wrong at glyph pixel " + std::to_string(gx) + "," + std::to_string(gy));
                        }
                    }
                }
            }
        }
    };
    checkLine(content.above[0], lineTop(0));
    checkLine(content.above[1], lineTop(1));
    checkLine(content.below[0], lineTop(2) + OverlayMetrics::GraphHeight + OverlayMetrics::GraphGap);

    std::vector<OverlayRect> bars;
    for (const auto& rect : layout.rects) {
        if (rect.color == static_cast<std::uint32_t>(AgcDriver::OverlayColor::Graph)) bars.push_back(rect);
    }
    require(bars.size() == content.frameTimes.size(), "the graph has " + std::to_string(bars.size()) + " bars for 40 frames");
    const auto graphBottom = layout.bounds.y + (lineTop(2) + OverlayMetrics::GraphHeight) * s;
    const auto full = OverlayMetrics::GraphHeight * s;
    require(bars.back().x == layout.bounds.x + (OverlayMetrics::Padding + static_cast<std::int32_t>(OverlayStatistics::FrameHistory) - 1) * s, "the newest frame is not at the right end of the graph");
    for (std::size_t i = 0; i + 1 < bars.size(); ++i) require(bars[i + 1].x == bars[i].x + s && bars[i].width == s, "the graph bars are not side by side");
    require(bars.back().height == full && bars.back().y + bars.back().height == graphBottom, "the slowest frame does not fill the graph");
    require(bars.front().height == static_cast<std::int32_t>(std::lround(10.0 / 50.0 * full)) && bars.front().y + bars.front().height == graphBottom, "a 10 ms frame is not a fifth of a 50 ms one");
    OverlayContent steady = content;
    std::fill(steady.frameTimes.begin(), steady.frameTimes.end(), 16.0);
    for (const auto& rect : AgcDriver::LayOutOverlay(steady, extent, OverlayCorner::TopLeft).rects) {
        if (rect.color == static_cast<std::uint32_t>(AgcDriver::OverlayColor::Graph)) require(rect.height == full / 2, "steady frames do not reach half the graph");
    }

    const std::pair<OverlayCorner, std::pair<bool, bool>> corners[] = {{OverlayCorner::TopRight, {true, false}}, {OverlayCorner::BottomLeft, {false, true}}, {OverlayCorner::BottomRight, {true, true}}};
    for (const auto& [corner, sides] : corners) {
        const auto placed = AgcDriver::LayOutOverlay(content, extent, corner).bounds;
        const auto x = sides.first ? static_cast<std::int32_t>(extent.width) - margin - placed.width : margin;
        const auto y = sides.second ? static_cast<std::int32_t>(extent.height) - margin - placed.height : margin;
        require(placed.x == x && placed.y == y && placed.width == layout.bounds.width && placed.height == layout.bounds.height, "a corner places the panel elsewhere");
    }
    OverlayContent profiled = content;
    profiled.below.push_back("GPU: 2.0 MS");
    require(AgcDriver::LayOutOverlay(profiled, extent, OverlayCorner::TopLeft).bounds.height == layout.bounds.height + OverlayMetrics::LinePitch * s, "a profiled line does not add one line to the panel");

    bool refused = false;
    try {
        static_cast<void>(AgcDriver::OverlayGlyph('q'));
    } catch (const std::exception&) {
        refused = true;
    }
    require(refused, "a character without a glyph was drawn");
}

void paletteTests() {
    const auto sdr = AgcDriver::OverlayPalette(VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
    require(sdr[1] == std::array<float, 4>{1.0f, 1.0f, 1.0f, 1.0f} && sdr[0][3] > 0.0f && sdr[0][3] < 1.0f, "the SDR palette is not opaque white text over a translucent panel");
    const auto srgb = AgcDriver::OverlayPalette(VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
    require(near(srgb[2][1], std::pow((0.8 + 0.055) / 1.055, 2.4), 1e-5) && srgb[1][0] == 1.0f, "an sRGB swapchain gets encoded colors");
    const auto hdr = AgcDriver::OverlayPalette(VK_FORMAT_A2B10G10R10_UNORM_PACK32, VK_COLOR_SPACE_HDR10_ST2084_EXT);
    for (std::size_t channel = 0; channel < 3; ++channel) require(near(hdr[1][channel], 0.5807, 0.0015), "HDR10 white text is not at 203 nits (PQ " + std::to_string(hdr[1][channel]) + ")");
    require(std::lround(hdr[0][0] * 1023.0f) == 0 && hdr[0][3] == sdr[0][3], "the HDR10 panel is not black at the same opacity");
}

}

int main() {
    try {
        fpsWindowTests();
        frameRingTests();
        counterTests();
        formatTests();
        lowRateTests();
        settingsTests();
        composeTests();
        layoutTests();
        paletteTests();
        std::cout << "performance overlay statistics, layout and palette tests passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
