#pragma once

#include "PanelLink.hpp"

namespace micro {

struct DistSensorPanelInData : public PanelLinkData {
    static constexpr millisecond_t period() { return millisecond_t(100); }
    static constexpr millisecond_t timeout() { return millisecond_t(250); }

} __attribute__((packed));

struct DistSensorPanelOutData : public PanelLinkData {
    static constexpr millisecond_t period() { return millisecond_t(20); }
    static constexpr millisecond_t timeout() { return millisecond_t(50); }

    uint16_t distance_mm;

} __attribute__((packed));

} // namespace micro
