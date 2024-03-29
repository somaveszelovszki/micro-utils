#include <micro/utils/trajectory.hpp>

#include <micro/test/utils.hpp>

using namespace micro;

void test_straight_line_perfect_follow(const Sign speedSign) {
    Trajectory trajectory;

    trajectory.setStartConfig(Trajectory::config_t{
        Pose{ point2m{ meter_t(0), meter_t(0) }, radian_t(0) },
        speedSign * m_per_sec_t(1.0f)
    }, meter_t(0));

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
        car.distance = dist;
        controlData = trajectory.update(car);
        EXPECT_EQ(speedSign * m_per_sec_t(1.0f), controlData.speed);
        EXPECT_EQ(millisecond_t(0), controlData.rampTime);
        EXPECT_TRUE(controlData.rearSteerEnabled);
        EXPECT_NEAR_UNIT(millimeter_t(0), controlData.lineControl.actual.pos, millimeter_t(1));
        EXPECT_NEAR_UNIT(radian_t(0), controlData.lineControl.actual.angle, degree_t(1));
        EXPECT_EQ(millimeter_t(0), controlData.lineControl.target.pos);
        EXPECT_EQ(radian_t(0), controlData.lineControl.target.angle);
    }
}

void test_straight_line_nonperfect_follow(const Sign speedSign) {
    Trajectory trajectory;

    trajectory.setStartConfig(Trajectory::config_t{
        Pose{ point2m{ meter_t(0), meter_t(0) }, radian_t(0) },
        speedSign * m_per_sec_t(1.0f)
    }, meter_t(0));

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
        car.distance = dist;
        controlData = trajectory.update(car);
        EXPECT_EQ(speedSign * m_per_sec_t(1.0f), controlData.speed);
        EXPECT_EQ(millisecond_t(0), controlData.rampTime);
        EXPECT_TRUE(controlData.rearSteerEnabled);
        EXPECT_NEAR_UNIT(carOffset, controlData.lineControl.actual.pos, centimeter_t(1));
        EXPECT_NEAR_UNIT(-carAngle, controlData.lineControl.actual.angle, degree_t(1));
        EXPECT_EQ(millimeter_t(0), controlData.lineControl.target.pos);
        EXPECT_EQ(radian_t(0), controlData.lineControl.target.angle);
    }
}

void test_sine_arc_fix_orientation_perfect_follow(const Sign speedSign) {
    Trajectory trajectory;

    trajectory.setStartConfig(Trajectory::config_t{
        Pose{ point2m{ meter_t(0), meter_t(0) }, radian_t(0) },
        speedSign * m_per_sec_t(1.0f)
    }, meter_t(0));

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
        radian_t(0),
        PI
    );

    CarProps car;
    ControlData controlData;

    const point2m endPos = trajectory.lastConfig().pose.pos;
    for (meter_t x = { 0 }; x < endPos.X; x += centimeter_t(20)) {
        const point2m prevPos = car.pose.pos;
        car.pose = { point2m{ x, endPos.Y * (1 - cos(x / endPos.X * PI)) / 2 }, radian_t(0) };
        car.distance += (car.pose.pos - prevPos).length();
        controlData = trajectory.update(car);
        EXPECT_EQ(speedSign * m_per_sec_t(1.0f), controlData.speed);
        EXPECT_EQ(millisecond_t(0), controlData.rampTime);
        EXPECT_TRUE(controlData.rearSteerEnabled);
        EXPECT_NEAR_UNIT(millimeter_t(0), controlData.lineControl.actual.pos, millimeter_t(1));
        EXPECT_EQ(millimeter_t(0), controlData.lineControl.target.pos);
        EXPECT_NEAR_UNIT(controlData.lineControl.actual.angle, controlData.lineControl.target.angle, degree_t(1));
    }
}

void test_straight_line_multiple_segments_perfect_follow(const Sign speedSign) {
    Trajectory trajectory;

    trajectory.setStartConfig(Trajectory::config_t{
        Pose{ point2m{ meter_t(0), meter_t(0) }, radian_t(0) },
        speedSign * m_per_sec_t(1.0f)
    }, meter_t(0));

    trajectory.appendLine(Trajectory::config_t{
        Pose{
            trajectory.lastConfig().pose.pos + vec2m{ meter_t(1), meter_t(0) },
            radian_t(0)
        },
        speedSign * m_per_sec_t(1.5f)
    });

    trajectory.appendLine(Trajectory::config_t{
        Pose{
            trajectory.lastConfig().pose.pos + vec2m{ meter_t(1), meter_t(0) },
            radian_t(0)
        },
        speedSign * m_per_sec_t(3.0f)
    });

    trajectory.appendLine(Trajectory::config_t{
        Pose{
            trajectory.lastConfig().pose.pos + vec2m{ meter_t(5), meter_t(0) },
            radian_t(0)
        },
        speedSign * m_per_sec_t(3.0f)
    });

    CarProps car;
    ControlData controlData;

    for (meter_t dist = { 0 }; dist < trajectory.length(); dist += centimeter_t(20)) {
        car.pose = { point2m{ dist, meter_t(0) }, radian_t(0) };
        car.distance = dist;
        controlData = trajectory.update(car);

        m_per_sec_t expectedTargetSpeed;
        if (dist < meter_t(1)) {
            expectedTargetSpeed = micro::lerp(dist, meter_t(0), meter_t(1), m_per_sec_t(1.0f), m_per_sec_t(1.5f));
        } else if (dist < meter_t(2)) {
            expectedTargetSpeed = micro::lerp(dist, meter_t(1), meter_t(2), m_per_sec_t(1.5f), m_per_sec_t(3.0f));
        } else {
            expectedTargetSpeed = m_per_sec_t(3.0f);
        }

        EXPECT_NEAR_UNIT(speedSign * expectedTargetSpeed, controlData.speed, m_per_sec_t(0.1f));
        EXPECT_EQ(millisecond_t(0), controlData.rampTime);
        EXPECT_TRUE(controlData.rearSteerEnabled);
        EXPECT_NEAR_UNIT(millimeter_t(0), controlData.lineControl.actual.pos, millimeter_t(1));
        EXPECT_NEAR_UNIT(radian_t(0), controlData.lineControl.actual.angle, degree_t(1));
        EXPECT_EQ(millimeter_t(0), controlData.lineControl.target.pos);
        EXPECT_EQ(radian_t(0), controlData.lineControl.target.angle);
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

TEST(trajectory, straight_line_multiple_segments_perfect_follow) {
    test_straight_line_multiple_segments_perfect_follow(Sign::POSITIVE);
    test_straight_line_multiple_segments_perfect_follow(Sign::NEGATIVE);
}