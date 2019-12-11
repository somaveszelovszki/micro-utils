#pragma once

#include <micro/utils/units.hpp>
#include <micro/container/vec.hpp>
#include <micro/container/infinite_buffer.hpp>
#include <cfg_track.hpp>

namespace micro {

/* @brief Stores data of a detected line.
 **/
struct Line {
    uint32_t id;
    millimeter_t pos_front; // The line position at the front sensor line (relative to car vertical middle axis).
    millimeter_t pos_rear;  // The line position at the rear sensor line (relative to car vertical middle axis).
    radian_t angle;         // The line angle (relative to car forward angle)
    rad_per_sec_t angular_velocity;
};

typedef vec<Line, cfg::MAX_NUM_LINES> Lines;
typedef vec<millimeter_t, cfg::MAX_NUM_LINES> LinePositions;

class LineCalculator {
public:
    LineCalculator() : currentMaxLineId(0) {}

    void update(LinePositions front, LinePositions rear);

    const Lines& lines() {
        return const_cast<LineCalculator*>(this)->currentLines().lines;
    }

    static void updateMainLine(const Lines& lines, Line& mainLine);

private:
    struct StampedLines {
        Lines lines;
        microsecond_t time;
    };

    StampedLines& currentLines() {
        return prevLines.peek_back(0);
    }

    static Lines::const_iterator getClosestLine(const Lines& lines, millimeter_t pos, bool isFront, millimeter_t& dist);

    static Lines::const_iterator findClosestLine(const Lines& lines, Lines::const_iterator line);

    static Lines::const_iterator findLine(const Lines& lines, const uint32_t id);

    static void removeUnmatchedLines(Lines& lines, LinePositions& positions, bool isFront);

    static void removeUnmatchedPositions(const Lines& lines, LinePositions& positions, uint32_t targetSize, bool isFront);

    infinite_buffer<StampedLines, 50> prevLines;
    uint32_t currentMaxLineId;
};

} // namespace micro
