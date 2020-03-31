#pragma once

#include "PanelLink.hpp"

namespace micro {

struct LineDetectInPanelLinkData : public PanelLinkData {

    bool indicatorLedsEnabled : 1;
    uint8_t scanRangeRadius : 5; // Radius of the line scan (number of sensors to each direction) - 0 means all sensors
    uint8_t reserved : 2;

    static constexpr microsecond_t period() { return millisecond_t(5); }
    static constexpr microsecond_t timeout() { return millisecond_t(12); }

} __attribute__((packed));

struct LineDetectOutPanelLinkData : public micro::PanelLinkData {

    struct {
        int16_t pos_mm_per16 : 13;
        uint8_t idx : 3;
    } __attribute__((packed)) lines[3];

    static constexpr microsecond_t period() { return millisecond_t(0); }
    static constexpr microsecond_t timeout() { return millisecond_t(5); }

} __attribute__((packed));

} // namespace micro
