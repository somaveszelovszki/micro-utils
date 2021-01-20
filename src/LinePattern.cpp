#include <micro/utils/LinePattern.hpp>

namespace micro {

const char* to_string(const LinePattern::type_t& type) {
    const char *str = nullptr;
    switch (type) {
    case LinePattern::NONE:        str = "NONE";        break;
    case LinePattern::SINGLE_LINE: str = "SINGLE_LINE"; break;
    case LinePattern::ACCELERATE:  str = "ACCELERATE";  break;
    case LinePattern::BRAKE:       str = "BRAKE";       break;
    case LinePattern::LANE_CHANGE: str = "LANE_CHANGE"; break;
    case LinePattern::JUNCTION_1:  str = "JUNCTION_1";  break;
    case LinePattern::JUNCTION_2:  str = "JUNCTION_2";  break;
    case LinePattern::JUNCTION_3:  str = "JUNCTION_3";  break;
    default:                       str = "";            break;
    }
    return str;
}

} // namespace micro
