#pragma once

#include <etl/set.h>

#include "units.hpp"

namespace micro {

/* @brief Stores data of a detected line.
 **/
struct Line {
    static constexpr uint8_t MAX_NUM_LINES = 3;

    millimeter_t pos; ///< The line position at the front sensor line (relative to car vertical middle axis).
    uint8_t id = 0;   ///< The line id @note 0 means unknown line id

    bool operator==(const Line& other) const { return this->pos == other.pos && this->id == other.id; }
    bool operator!=(const Line& other) const { return !(*this == other); }
    bool operator<(const Line& other) const  { return this->pos < other.pos; }
    bool operator>(const Line& other) const  { return this->pos > other.pos; }
};

using Lines = etl::set<Line, Line::MAX_NUM_LINES>;

struct OrientedLine {
    millimeter_t pos;
    radian_t angle;

    bool operator==(const OrientedLine& other) const { return this->pos == other.pos && this->angle == other.angle; }
    bool operator!=(const OrientedLine& other) const { return !(*this == other); }
};

struct MainLine {
    const meter_t carFrontRearSensorRowDist;
    Line frontLine;             ///< The position of the line at the front sensor row - in the front sensor row's coordinate system
    Line rearLine;              ///< The position of the line at the rear sensor row - in the rear sensor row's coordinate system
    OrientedLine centerLine;    ///< The followed line - in the car's current direction's coordinate system. @note This will be the control input.

    explicit MainLine(const meter_t carFrontRearSensorRowDist);

    void updateFrontRearLines();
    void updateCenterLine();
};

void updateMainLine(const Lines& lines, Line& mainLine);

void updateMainLine(const Lines& frontLines, const Lines& rearLines, MainLine& mainLine);

Lines::const_iterator findClosestLine(const Lines& lines, millimeter_t pos);

Lines::const_iterator findLine(const Lines& lines, const uint32_t id);

bool areClose(const Lines& lines);

bool areFar(const Lines& lines);

#define EXPECT_EQ_MICRO_ORIENTED_LINE(expected, result) \
    EXPECT_NEAR_UNIT_DEFAULT(expected.pos, result.pos); \
    EXPECT_NEAR_UNIT_DEFAULT(expected.angle, result.angle)

} // namespace micro
