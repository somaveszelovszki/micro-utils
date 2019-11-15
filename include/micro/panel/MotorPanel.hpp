#pragma once

#include "Panel.hpp"
#include "MotorPanelData.h"

namespace micro {

class MotorPanel : public Panel<motorPanelDataIn_t, dataSize_motorPanelDataIn, motorPanelDataOut_t, dataSize_motorPanelDataOut> {
public:
    using Panel::Panel;
};

}  // namespace micro
