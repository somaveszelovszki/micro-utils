#pragma once

#include <micro/math/unit_utils.hpp>

namespace micro {

class PID_Controller {
public:

    PID_Controller(const float P, const float I, const float D, const float integralMax, const float outMin, const float outMax, const float deadband);

    void tune(const float P, const float I, const float D, const float integralMax);

    float output() const {
        return this->output_;
    }

    void update(const float measured);

    float desired;

private:
    float P_, I_, D_;
    float integralMax_;
    const float outMin_, outMax_;
    const float deadband_;
    float prevErr_;
    float integral_;
    float output_;
    millisecond_t prevUpdateTime_;
};
} // namespace micro
