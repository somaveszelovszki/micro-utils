#pragma once

#include "PanelLink.hpp"

namespace micro {

struct DistSensorPanelInData : public PanelLinkData {
    static constexpr millisecond_t period()  { return millisecond_t(100); }
    static constexpr millisecond_t timeout() { return millisecond_t(250); }

    uint8_t filterSize;

} __attribute__((packed));

struct DistSensorPanelOutData : public PanelLinkData {
    static constexpr millisecond_t period()  { return millisecond_t(10); }
    static constexpr millisecond_t timeout() { return millisecond_t(25); }

    uint16_t distance_mm;

} __attribute__((packed));

}  // namespace micro
