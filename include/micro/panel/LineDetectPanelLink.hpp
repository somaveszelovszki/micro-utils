#pragma once

#include "PanelLink.hpp"
#include "LineDetectPanelData.h"

namespace micro {

class LineDetectPanelLink : public PanelLink<lineDetectPanelDataOut_t, lineDetectPanelDataIn_t, PanelLinkRole_Passive> {
public:
    using PanelLink::PanelLink;
};

}  // namespace micro