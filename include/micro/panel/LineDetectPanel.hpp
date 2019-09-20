#pragma once

#include "Panel.hpp"
#include "LineDetectPanelData.h"

namespace micro {

class LineDetectPanel : public Panel<lineDetectPanelDataIn_t, lineDetectPanelDataOut_t> {
public:
    using Panel<lineDetectPanelDataIn_t, lineDetectPanelDataOut_t>::Panel;
};


}  // namespace micro
