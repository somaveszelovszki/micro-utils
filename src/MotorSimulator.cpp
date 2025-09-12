#include <cmath>

#include <iterator>

#include <micro/sim/MotorSimulator.hpp>

namespace micro {

MotorSimulator::MotorSimulator(const m_per_sec2_t accelerationFwdRatio,
                               const millisecond_t accelerationBwdRatio,
                               const millisecond_t deadTime, const microsecond_t simStep)
    : accelerationFwdRatio_(accelerationFwdRatio), accelerationBwdRatio_(accelerationBwdRatio),
      deadTime_(deadTime), simStep_(simStep) {
    for (uint32_t i = 0; i < prevDuties_.capacity(); ++i) {
        prevDuties_.push(0.0f);
    }
}

m_per_sec_t MotorSimulator::speed() const {
    return this->speed_;
}

void MotorSimulator::update(const float duty) {
    prevDuties_.pop();
    prevDuties_.push(duty);

    const float outDuty = *std::next(prevDuties_.rbegin(), std::lround(deadTime_ / simStep_));

    const m_per_sec2_t accelerationFwd = outDuty * this->accelerationFwdRatio_;
    const m_per_sec2_t accelerationBwd = this->speed_ / this->accelerationBwdRatio_;

    const m_per_sec2_t acceleration = accelerationFwd - accelerationBwd;

    this->speed_ += acceleration * this->simStep_;
}

} // namespace micro
