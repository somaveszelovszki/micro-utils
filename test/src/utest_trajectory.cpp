#include <micro/utils/trajectory.hpp>

#include "utils.hpp"

using namespace micro;

void test_straight_line_perfect_follow(const Sign speedSign) {
    Trajectory trajectory;

    trajectory.setStartConfig(Trajectory::config_t{
        Pose{ point2m{ meter_t(0), meter_t(0) }, radian_t(0) },
        speedSign * m_per_sec_t(1.0f)
    }, meter_t(100));

    trajectory.appendLine(Trajectory::config_t{
        Pose{
            trajectory.lastConfig().pose.pos + vec2m{ meter_t(10), meter_t(0) },
            radian_t(0)
        },
        speedSign * m_per_sec_t(1.0f)
    });

    CarProps car;
    ControlData controlData;

    for (meter_t dist = { 0 }; dist < trajectory.length(); dist += centimeter_t(20)) {
        car.pose = { point2m{ dist, meter_t(0) }, radian_t(0) };
        car.distance = meter_t(0);
        controlData = trajectory.update(car);
        EXPECT_EQ(speedSign * m_per_sec_t(1.0f), controlData.speed);
        EXPECT_EQ(millisecond_t(0), controlData.rampTime);
        EXPECT_EQ(ControlData::controlType_t::Line, controlData.controlType);
        EXPECT_NEAR_UNIT(millimeter_t(0), controlData.lineControl.actual.pos, millimeter_t(1));
        EXPECT_NEAR_UNIT(radian_t(0), controlData.lineControl.actual.angle, degree_t(1));
        EXPECT_EQ(millimeter_t(0), controlData.lineControl.desired.pos);
        EXPECT_EQ(radian_t(0), controlData.lineControl.desired.angle);
    }
}

void test_straight_line_nonperfect_follow(const Sign speedSign) {
    Trajectory trajectory;

    trajectory.setStartConfig(Trajectory::config_t{
        Pose{ point2m{ meter_t(0), meter_t(0) }, radian_t(0) },
        speedSign * m_per_sec_t(1.0f)
    }, meter_t(100));

    trajectory.appendLine(Trajectory::config_t{
        Pose{
            trajectory.lastConfig().pose.pos + vec2m{ meter_t(10), meter_t(0) },
            radian_t(0)
        },
        speedSign * m_per_sec_t(1.0f)
    });

    CarProps car;
    ControlData controlData;

    for (meter_t dist = { 0 }; dist < trajectory.length(); dist += centimeter_t(20)) {
        const float coverage = dist / trajectory.length();
        const meter_t carOffset = centimeter_t(10) * coverage;
        const radian_t carAngle = degree_t(10) * coverage;

        car.pose = { point2m{ dist, carOffset }, carAngle };
        car.distance = meter_t(0);
        controlData = trajectory.update(car);
        EXPECT_EQ(speedSign * m_per_sec_t(1.0f), controlData.speed);
        EXPECT_EQ(millisecond_t(0), controlData.rampTime);
        EXPECT_EQ(ControlData::controlType_t::Line, controlData.controlType);
        EXPECT_NEAR_UNIT(carOffset, controlData.lineControl.actual.pos, centimeter_t(1));
        EXPECT_NEAR_UNIT(-carAngle, controlData.lineControl.actual.angle, degree_t(1));
        EXPECT_EQ(millimeter_t(0), controlData.lineControl.desired.pos);
        EXPECT_EQ(radian_t(0), controlData.lineControl.desired.angle);
    }
}

void test_sine_arc_fix_orientation_perfect_follow(const Sign speedSign) {
    Trajectory trajectory;

    trajectory.setStartConfig(Trajectory::config_t{
        Pose{ point2m{ meter_t(0), meter_t(0) }, radian_t(0) },
        speedSign * m_per_sec_t(1.0f)
    }, meter_t(100));

    trajectory.appendSineArc(
        Trajectory::config_t{
            Pose{
                trajectory.lastConfig().pose.pos + vec2m{ meter_t(2), meter_t(1) },
                radian_t(0)
            },
            speedSign * m_per_sec_t(1.0f)
        },
        radian_t(0),
        Trajectory::orientationUpdate_t::FIX_ORIENTATION,
        50,
        radian_t(0),
        PI
    );

    CarProps car;
    ControlData controlData;

    const point2m endPos =  trajectory.lastConfig().pose.pos;
    for (meter_t x = { 0 }; x < endPos.X; x += centimeter_t(20)) {
        car.pose = { point2m{ x, endPos.Y * (1 - cos(x / endPos.X * PI)) / 2 }, radian_t(0) };
        car.distance = meter_t(0);
        controlData = trajectory.update(car);
        EXPECT_EQ(speedSign * m_per_sec_t(1.0f), controlData.speed);
        EXPECT_EQ(millisecond_t(0), controlData.rampTime);
        EXPECT_EQ(ControlData::controlType_t::Line, controlData.controlType);
        EXPECT_NEAR_UNIT(millimeter_t(0), controlData.lineControl.actual.pos, millimeter_t(1));
        EXPECT_EQ(millimeter_t(0), controlData.lineControl.desired.pos);
        EXPECT_NEAR_UNIT(controlData.lineControl.actual.angle, controlData.lineControl.desired.angle, degree_t(1));
    }
}

TEST(trajectory, straight_line_perfect_follow) {
    test_straight_line_perfect_follow(Sign::POSITIVE);
    test_straight_line_perfect_follow(Sign::NEGATIVE);
}

TEST(trajectory, straight_line_nonperfect_follow) {
    test_straight_line_nonperfect_follow(Sign::POSITIVE);
    test_straight_line_nonperfect_follow(Sign::NEGATIVE);
}

TEST(trajectory, sine_arc_fix_orientation_perfect_follow) {
    test_sine_arc_fix_orientation_perfect_follow(Sign::POSITIVE);
    test_sine_arc_fix_orientation_perfect_follow(Sign::NEGATIVE);
}
