#pragma once

#include <micro/math/unit_utils.hpp>
#include <micro/utils/Line.hpp>

namespace micro {

class LineController {
public:
	LineController(meter_t _wheelBase, meter_t _sensorWheelDist);

	Status run(m_per_sec_t speed, meter_t baseline, const Line& line);

	radian_t getOutput() const {
	    return this->output;
	}
private:
	const meter_t wheelBase;          // Distance between the two axles.
	const meter_t projectedWheelBase; // The original wheel base + the sensor distance.
	radian_t output;
};

class LineController2 {

public:

    LineController2(meter_t _wheelBase, meter_t _sensorWheelDist)
        : wheelBase(_wheelBase)
        , projectedWheelBase(_sensorWheelDist + _wheelBase) {}

    radian_t GetControlSignal(m_per_sec_t currentVelocity, const Line& line) {
        const m_per_sec_t MIN_SPEED = mm_per_sec_t(5);
        radian_t result;
        if (abs(currentVelocity) < MIN_SPEED) {
            result = radian_t(0.0f);
        } else {

            // The car should correct itself in this distance.
            const meter_t adjustDistance = centimeter_t(50) + currentVelocity * second_t(0.5);

            // The car should correct itself in this time.
            const second_t adjustTime = adjustDistance / currentVelocity;

            // The time constant that the system needs.
            float T_const = (adjustTime.get() * xi_coeff) / 3;
            float w0 = 1.0f / T_const;

            // Update poles = (-)Re +- j*Im
            float s_re = -w0*xi_coeff;
            float s_im = w0*std::sqrt(1-xi_coeff*xi_coeff);

            // Handle for variables.
            float v = static_cast<m_per_sec_t>(currentVelocity).get();
            float L = static_cast<meter_t>(this->projectedWheelBase).get();

            // Gains of the position and orientation.
            float K_location = (-L / (v*v)) * (s_re*s_re + s_im*s_im);
            float K_orientation = (L / v) * ((2*s_re) - v*K_location);

            // Feedback components. Baseline, position, orientation.
            float ub = K_location * this->baseline.get();
            float up = K_location * static_cast<millimeter_t>(line.pos_front).get();
            float ud = -K_orientation * static_cast<radian_t>(line.angle).get();

            // The wheel turn at the sensor.
            radian_t servoInFi = micro::normalize360(radian_t(ub - up - ud));
            if (servoInFi > PI) {
                servoInFi -= 2 * PI;
            }

            servoInFi = micro::clamp(servoInFi, -PI_2, PI_2);

            result = atan(tan(servoInFi) * (this->wheelBase / this->projectedWheelBase));
        }

        return result;
    }

private:

    static constexpr float xi_coeff = 0.7f; // TODO 0.9?

    // From config.
    meter_t wheelBase;                       // Distance between the two axles.
    meter_t projectedWheelBase ;             // The original wheel base + the sensor distance.

    meter_t baseline;                    // The controller wants to keep the line at this point of the sensor.

};
} // namespace micro
