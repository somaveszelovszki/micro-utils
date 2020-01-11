#pragma once

#include "PanelLink.hpp"
#include "MotorPanelData.h"

namespace micro {

class MotorPanelLink : public PanelLink<motorPanelDataOut_t, motorPanelDataIn_t, PanelLinkRole_Master> {
public:
    using PanelLink::PanelLink;
};

}  // namespace micro
