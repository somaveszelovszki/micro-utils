#pragma once

#include "Panel.hpp"
#include "MotorPanelData.h"

namespace micro {

class MotorPanel : public Panel<motorPanelDataIn_t, motorPanelDataOut_t> {
public:
    using Panel<motorPanelDataIn_t, motorPanelDataOut_t>::Panel;
};

}  // namespace micro
