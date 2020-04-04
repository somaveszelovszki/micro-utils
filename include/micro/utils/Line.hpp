#pragma once

#include <micro/container/vec.hpp>

#include "units.hpp"

#include <cfg_micro.hpp>

namespace micro {

/* @brief Stores data of a detected line.
 **/
struct Line {
    millimeter_t pos; // The line position at the front sensor line (relative to car vertical middle axis).
    uint8_t id : 3;

    bool operator==(const Line& other) const { return this->pos == other.pos && this->id == other.id; }
    bool operator!=(const Line& other) const { return !(*this == other); }
    bool operator<(const Line& other) const  { return this->pos < other.pos; }
    bool operator>(const Line& other) const  { return this->pos > other.pos; }
};

typedef sorted_vec<Line, cfg::MAX_NUM_LINES> Lines;

void updateMainLine(const Lines& lines, Line& mainLine);

Lines::const_iterator findClosestLine(const Lines& lines, millimeter_t pos);

Lines::const_iterator findLine(const Lines& lines, const uint32_t id);

bool areClose(const Lines& lines);

bool areFar(const Lines& lines);

} // namespace micro
