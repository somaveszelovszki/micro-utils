#include <micro/utils/CarProps.hpp>

#include <micro/test/utils.hpp>

using namespace micro;

namespace {

constexpr meter_t CAR_FRONT_REAR_PIVOT_DIST = centimeter_t(30);

} // namespace

TEST(CarProps, getSpeedAngle) {
    CarProps car;

    for (car.pose.angle = radian_t(0); car.pose.angle < 2 * PI; car.pose.angle += degree_t(20)) {
        car.frontWheelAngle = radian_t(0);
        car.rearWheelAngle  = radian_t(0);
        EXPECT_NEAR_UNIT(car.pose.angle, car.getSpeedAngle(CAR_FRONT_REAR_PIVOT_DIST), degree_t(1));

        car.frontWheelAngle = degree_t(15);
        car.rearWheelAngle  = degree_t(15);
        EXPECT_NEAR_UNIT(normalize360(car.pose.angle + degree_t(15)), car.getSpeedAngle(CAR_FRONT_REAR_PIVOT_DIST), degree_t(1));

        car.frontWheelAngle = degree_t(-15);
        car.rearWheelAngle  = degree_t(-15);
        EXPECT_NEAR_UNIT(normalize360(car.pose.angle - degree_t(15)), car.getSpeedAngle(CAR_FRONT_REAR_PIVOT_DIST), degree_t(1));

        car.frontWheelAngle = degree_t(15);
        car.rearWheelAngle  = degree_t(-15);
        EXPECT_NEAR_UNIT(car.pose.angle, car.getSpeedAngle(CAR_FRONT_REAR_PIVOT_DIST), degree_t(1));
    }
}