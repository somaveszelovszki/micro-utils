#pragma once

#include "point2.hpp"

namespace micro {

struct Pose {

    /* @brief Position of the car relative to its start position.
    **/
    point2m pos;

    /* @brief Car's orientation.
    @note Orientation is relative to the X axis!
     **/
    radian_t angle;

    bool operator==(const Pose& other) { return this->pos == other.pos && this->angle == other.angle; }
    bool operator!=(const Pose& other) { return !(*this == other); };
};

/* @brief Car properties.
 **/
struct CarProps  {
    /* @brief Default constructor - initializes fields.
     **/
    CarProps() {}

    bool operator==(const CarProps& other) {
        return this->pose            == other.pose            &&
            this->speed              == other.speed           &&
            this->distance           == other.distance        &&
            this->orientedDistance   == other.distance        &&
            this->frontWheelAngle    == other.frontWheelAngle &&
            this->rearWheelAngle     == other.rearWheelAngle  &&
            this->yawRate            == other.yawRate  &&
            this->isRemoteControlled == other.isRemoteControlled;
    }

    bool operator!=(const CarProps& other) { return !(*this == other); }

    Pose pose;                // The current pose (position and orientation) of the car.
    m_per_sec_t speed;        // The current speed of the car.
    meter_t distance;         // The distance that the car has driven since startup.
    meter_t orientedDistance; // The distance that the has driven with no major orientation change (in a straight line).
    radian_t frontWheelAngle;
    radian_t rearWheelAngle;
    rad_per_sec_t yawRate;
    bool isRemoteControlled = false;

    radian_t getSpeedAngle(const meter_t frontRearPivotDist) const;

    vec2mps getSpeedVector(const meter_t frontRearPivotDist) const;
};

#define EXPECT_EQ_MICRO_POSE(expected, result)              \
    EXPECT_NEAR_UNIT_DEFAULT(expected.pos.X, result.pos.X); \
    EXPECT_NEAR_UNIT_DEFAULT(expected.pos.Y, result.pos.Y); \
    EXPECT_NEAR_UNIT_DEFAULT(expected.angle, result.angle)

#define EXPECT_EQ_MICRO_CAR_PROPS(expected, result)                               \
    EXPECT_EQ_MICRO_POSE(expected.pose, result.pose);                             \
    EXPECT_NEAR_UNIT_DEFAULT(expected.speed, result.speed);                       \
    EXPECT_NEAR_UNIT_DEFAULT(expected.distance, result.distance);                 \
    EXPECT_NEAR_UNIT_DEFAULT(expected.orientedDistance, result.orientedDistance); \
    EXPECT_NEAR_UNIT_DEFAULT(expected.frontWheelAngle, result.frontWheelAngle);   \
    EXPECT_NEAR_UNIT_DEFAULT(expected.rearWheelAngle, result.rearWheelAngle);     \
    EXPECT_NEAR_UNIT_DEFAULT(expected.yawRate, result.yawRate);                   \
    EXPECT_EQ(expected.isRemoteControlled, result.isRemoteControlled)

} // namespace micro
