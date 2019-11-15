#pragma once

#include "Panel.hpp"
#include "LineDetectPanelData.h"

namespace micro {

class LineDetectPanel : public Panel<lineDetectPanelDataIn_t, dataSize_lineDetectPanelDataIn, lineDetectPanelDataOut_t, dataSize_lineDetectPanelDataOut> {
public:
    using Panel::Panel;
};


}  // namespace micro
