#include <fstream>
#include <micro/control/PID_Controller.hpp>
#include <micro/sim/MotorSimulator.hpp>
#include <micro/test/utils.hpp>

using namespace micro;

namespace {

constexpr m_per_sec2_t MOTOR_ACCELERATION_FWD_RATIO  = m_per_sec2_t(4);
constexpr millisecond_t MOTOR_ACCELERATION_BWD_RATIO = millisecond_t(1500);
constexpr millisecond_t MOTOR_DEAD_TIME              = millisecond_t(50);

constexpr microsecond_t SIMULATION_STEP   = microsecond_t(500);
constexpr microsecond_t SIMULATION_LENGTH = second_t(5);

void step(const m_per_sec_t target) {
    MotorSimulator motor(MOTOR_ACCELERATION_FWD_RATIO, MOTOR_ACCELERATION_BWD_RATIO,
                         MOTOR_DEAD_TIME, SIMULATION_STEP);

    const PID_Params params = {1.0f, 0.0005f, 0.0f};
    PID_Controller speedController(params, 1.0f, 0.2f, 0.0f);

    speedController.target = target.get();

    microsecond_t now = microsecond_t(0);

    std::ofstream out("/home/soma/work/temp/controller.csv");

    while (now < SIMULATION_LENGTH) {
        const float actual = motor.speed().get();
        speedController.update(actual);
        motor.update(speedController.output());

        out << actual << "," << speedController.output() << "\n";

        if (now > second_t(2)) {
            EXPECT_NEAR(speedController.target, actual, 0.1f);
        }

        now += SIMULATION_STEP;
    }
}

void ramp(const m_per_sec_t target) {
    MotorSimulator motor(MOTOR_ACCELERATION_FWD_RATIO, MOTOR_ACCELERATION_BWD_RATIO,
                         MOTOR_DEAD_TIME, SIMULATION_STEP);

    const PID_Params params = {1.0f, 0.0005f, 0.0f};
    PID_Controller speedController(params, 1.0f, 0.2f, 0.0f);

    microsecond_t now = microsecond_t(0);

    while (now < SIMULATION_LENGTH) {
        speedController.target = micro::lerp<microsecond_t, m_per_sec_t>(
                                     now, second_t(0), second_t(2), m_per_sec_t(0), target)
                                     .get();

        const float actual = motor.speed().get();
        speedController.update(actual);
        motor.update(speedController.output());

        EXPECT_NEAR(speedController.target, actual, 0.2f);

        now += SIMULATION_STEP;
    }
}

} // namespace

TEST(PID_Controller, step_positive) {
    step(m_per_sec_t(1));
}

TEST(PID_Controller, step_negative) {
    step(m_per_sec_t(-1));
}

TEST(PID_Controller, ramp_positive) {
    ramp(m_per_sec_t(1));
}

TEST(PID_Controller, ramp_negative) {
    ramp(m_per_sec_t(-1));
}
