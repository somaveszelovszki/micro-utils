#include <micro/control/LineController.hpp>
#include <micro/utils/log.hpp>
#include <micro/math/unit_utils.hpp>

namespace micro {

LineController::LineController(meter_t _wheelBase, meter_t _sensorWheelDist)
    : wheelBase(_wheelBase)
    , projectedWheelBase(_sensorWheelDist + _wheelBase) {}

Status LineController::run(m_per_sec_t speed, meter_t baseline, const Line& line) {

    static constexpr float KSI_COEFF = SQRT_2;
    static constexpr m_per_sec_t MIN_SPEED = mm_per_sec_t(5);

    if (micro::abs(speed) < MIN_SPEED) {
        this->output = radian_t::zero();
    } else {

        // The car should correct itself in this distance.
//        millimeter_t adjustDistance(static_cast<mm_per_sec_t>(speed).get() * 0.5f + 500);

        const millimeter_t adjustDist = centimeter_t(50) + speed * second_t(0.5f);
        const second_t adjustTime = adjustDist / speed;
//        float adjustTime = adjustDistance.get() / static_cast<mm_per_sec_t>(speed).get();

        // The time constant that the system needs.
        float T_const = (adjustTime.get() * KSI_COEFF) / 3;
        float w0 = 1.0f / T_const;

        // Update poles = (-)Re +- j*Im
        float s_re = -w0 * KSI_COEFF;
        float s_im = w0 * std::sqrt(1 - KSI_COEFF * KSI_COEFF);

        // Handle for variables.
        float v = speed.get();
        float L = this->projectedWheelBase.get();

        // Gains of the position and orientation.
        float K_location = (-L / (v * v)) * (s_re * s_re + s_im * s_im);
        float K_orientation = (L / v) * ((2 * s_re) - v * K_location);

        // Feedback components. Baseline, position, orientation.
        float ub = K_location * baseline.get();
        float up = K_location * static_cast<meter_t>(line.pos_front).get();
        float ud = -K_orientation * line.angle.get();

        // The wheel turn at the sensor
        const radian_t servoInFi = micro::clamp(radian_t(ub - up - ud), -micro::PI_2, micro::PI_2);

        //result = atan(tan(servoInFi)*(this->wheelbase/(this->projectedWheelBase)));
        this->output = servoInFi * (this->wheelBase / this->projectedWheelBase);   // nearly equal to atan(tan(servoInFi)*(this->wheelbase/(this->projectedWheelBase)))
    }

    return Status::OK;
}

} // namespace micro
