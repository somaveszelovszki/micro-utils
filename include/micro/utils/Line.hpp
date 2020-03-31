#pragma once

#include "units.hpp"

#include <micro/container/vec.hpp>
#include <micro/container/infinite_buffer.hpp>

namespace micro {

/* @brief Stores data of a detected line.
 **/
struct Line {
    millimeter_t pos; // The line position at the front sensor line (relative to car vertical middle axis).
    uint8_t id = 0;

    bool operator==(const Line& other) const {
        return this->pos == other.pos && this->id == other.id;
    }

    bool operator!=(const Line& other) const {
        return !(*this == other);
    }
};

typedef vec<Line, 3> Lines;

void updateMainLine(const Lines& lines, Line& mainLine);

Lines::const_iterator findClosestLine(const Lines& lines, millimeter_t pos);

Lines::const_iterator findLine(const Lines& lines, const uint32_t id);

} // namespace micro
