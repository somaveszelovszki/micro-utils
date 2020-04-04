#pragma once

#include <micro/utils/LinePattern.hpp>
#include "PanelLink.hpp"

namespace micro {

struct LineDetectInPanelLinkData : public PanelLinkData {

    bool     indicatorLedsEnabled : 1;
    uint8_t  scanRangeRadius      : 5; // Radius of the line scan (number of sensors to each direction) - 0 means all sensors
    uint8_t  domain               : 1;
    uint8_t  reserved             : 1;
    uint32_t distance_mm          : 24;

    static constexpr microsecond_t period()  { return millisecond_t(5);  }
    static constexpr microsecond_t timeout() { return millisecond_t(12); }

} __attribute__((packed));

struct LineDetectOutPanelLinkData : public micro::PanelLinkData {

    struct {
        int16_t pos_mm_per16 : 13;
        uint8_t id           : 3;
    } __attribute__((packed)) lines[cfg::MAX_NUM_LINES];

    struct {
        uint8_t  type         : 4;
        int8_t  dir           : 2;
        int8_t  side          : 2;
        uint32_t startDist_mm : 24;
    } __attribute__((packed)) pattern;

    static constexpr microsecond_t period()  { return millisecond_t(0); }
    static constexpr microsecond_t timeout() { return millisecond_t(5); }

} __attribute__((packed));

} // namespace micro
