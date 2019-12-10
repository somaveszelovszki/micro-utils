#pragma once

#include <micro/utils/units.hpp>
#include <micro/container/vec.hpp>
#include <micro/container/infinite_buffer.hpp>
#include <cfg_track.hpp>

namespace micro {

/* @brief Stores data of a detected line.
 **/
struct Line {
    millimeter_t pos_front; // The line position at the front sensor line (relative to car vertical middle axis).
    millimeter_t pos_rear;  // The line position at the rear sensor line (relative to car vertical middle axis).
    radian_t angle;         // The line angle (relative to car forward angle)
    rad_per_sec_t angular_velocity;
};

typedef vec<Line, cfg::MAX_NUM_LINES> Lines;
typedef vec<millimeter_t, cfg::MAX_NUM_LINES> LinePositions;

class LineCalculator {
public:
    void update(LinePositions front, LinePositions rear);

    const Lines& lines() {
        return const_cast<LineCalculator*>(this)->currentLines();
    }

    static Line getMainLine(const Lines& lines, const Line& prevMainLine);

private:

    Lines& currentLines() {
        return prevLines.peek_back(0);
    }

    static Lines::const_iterator getClosestLine(const Lines& lines, millimeter_t pos, bool isFront, millimeter_t& dist);

    void removeUnmatchedLines(LinePositions& positions, bool isFront);

    void removeUnmatchedPositions(LinePositions& positions, uint32_t targetSize, bool isFront);

    infinite_buffer<Lines, 50> prevLines;
};

} // namespace micro
