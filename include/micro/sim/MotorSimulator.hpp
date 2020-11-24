#pragma once

#include <micro/container/infinite_buffer.hpp>
#include <micro/utils/units.hpp>

namespace micro {

class MotorSimulator {
public:
    MotorSimulator(const m_per_sec2_t accelerationFwdRatio, const millisecond_t accelerationBwdRatio, const millisecond_t deadTime,
        const microsecond_t simStep);

    m_per_sec_t speed() const;

    void update(const float duty);

private:
    const m_per_sec2_t accelerationFwdRatio_;
    const millisecond_t accelerationBwdRatio_;
    const millisecond_t deadTime_;

    const microsecond_t simStep_;

    infinite_buffer<float, 200> prevDuties_;
    m_per_sec_t speed_;
};

} // namespace micro
