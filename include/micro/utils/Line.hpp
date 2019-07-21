#pragma once

#include <micro/utils/units.hpp>
#include <micro/container/vec.hpp>
#include <cfg_track.hpp>

namespace micro {

/* @brief Stores data of a detected line.
 **/
struct Line {
    millimeter_t pos_front; // The line position at the front sensor line (relative to car vertical middle axis).
    millimeter_t pos_rear;  // The line position at the rear sensor line (relative to car vertical middle axis).
    radian_t angle;         // The line angle (relative to car forward angle)
};

typedef vec<Line, cfg::MAX_NUM_LINES> Lines;
typedef vec<millimeter_t, cfg::MAX_NUM_LINES> LinePositions;

void calculateLines(LinePositions front, LinePositions rear, Lines& lines, Line& mainLine);

} // namespace micro
