#pragma once

#include <micro/utils/Line.hpp>

namespace micro {

/* @brief Stores data of a detected line pattern.
 **/
struct LinePattern {
public:
    /* @brief Defines line pattern type.
     **/
    enum type_t : uint8_t {
        NONE        = 0, ///< No lines detected
        SINGLE_LINE = 1, ///< 1 solid 1.9mm wide line.
        ACCELERATE  = 2, ///< 1 solid line in the middle, 2 dashed lines on both sides
        BRAKE       = 3, ///< 1 solid line in the middle, 2 solid lines on both sides
        LANE_CHANGE = 4, ///< 1 solid line in the middle, 1 dashed line on one (UNKNOWN!) side (dash and space lengths decreasing)
        JUNCTION_1  = 5, ///< Labyrinth junction of 1 segment
        JUNCTION_2  = 6, ///< Labyrinth junction of 2 segments
        JUNCTION_3  = 7, ///< Labyrinth junction of 3 segments
        DEAD_END    = 8  ///< Labyrinth dead-end
    };

    type_t type;        // Line pattern type.
    Sign dir;         // The pattern direction (POSITIVE means it is the same as the car direction).
    Direction side;   // The side of the line the pattern is on. In case of symmetrical pattern: CENTER.
    meter_t startDist;

    LinePattern()
        : type(type_t::NONE)
        , dir(Sign::NEUTRAL)
        , side(Direction::CENTER)
        , startDist(0) {}

    LinePattern(type_t type, Sign dir, Direction side, meter_t startDist = meter_t(0))
        : type(type)
        , dir(dir)
        , side(side)
        , startDist(startDist) {}

    bool operator==(const LinePattern& other) const {
        return this->type == other.type && this->dir == other.dir && this->side == other.side;
    }

    bool operator!=(const LinePattern& other) const {
        return !(*this == other);
    }
};

enum class linePatternDomain_t : uint8_t {
    Race      = 0,
    Labyrinth = 1
};

const char* to_string(const LinePattern::type_t& type);

} // namespace micro
