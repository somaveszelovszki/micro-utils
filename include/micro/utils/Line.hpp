#pragma once

#include "units.hpp"

#include <micro/container/vec.hpp>
#include <micro/container/infinite_buffer.hpp>
#include <micro/panel/line.h>

#include <cfg_track.hpp>

namespace micro {

/* @brief Stores data of a detected line.
 **/
struct Line {
    millimeter_t pos; // The line position at the front sensor line (relative to car vertical middle axis).
    uint8_t id;

    bool operator==(const Line& other) const {
        return this->pos == other.pos && this->id == other.id;
    }

    bool operator!=(const Line& other) const {
        return !(*this == other);
    }
};

typedef vec<Line, MAX_NUM_LINES> Lines;

class LineCalculator {
public:
    void update(const Lines& lines);

    const Lines& lines() {
        return const_cast<LineCalculator*>(this)->currentLines().lines;
    }

    static void updateMainLine(const Lines& lines, Line& mainLine);

    static Lines::const_iterator findClosestLine(const Lines& lines, millimeter_t pos);

    static Lines::const_iterator findLine(const Lines& lines, const uint32_t id);

private:
    struct StampedLines {
        Lines lines;
        microsecond_t time;
    };

    StampedLines& currentLines() {
        return prevLines.peek_back(0);
    }

    infinite_buffer<StampedLines, 50> prevLines;
};

} // namespace micro
