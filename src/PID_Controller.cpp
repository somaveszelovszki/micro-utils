#include <micro/control/PID_Controller.hpp>
#include <micro/utils/time.hpp>

namespace micro {

PID_Controller::PID_Controller(const float P, const float I, const float D, const float integralMax, const float outMin, const float outMax, const float deadband)
    : desired(0.0f)
    , P_(P)
    , I_(I)
    , D_(D)
    , integralMax_(integralMax)
    , outMin_(outMin)
    , outMax_(outMax)
    , deadband_(deadband)
    , prevErr_(0.0f)
    , integral_(0.0f)
    , output_(0.0f) {}

void PID_Controller::tune(const float P, const float I, const float D, const float integralMax) {
    this->P_ = P;
    this->I_ = I;
    this->D_ = D;
    this->integralMax_ = integralMax;
}

void PID_Controller::update(const float measured) {

    const millisecond_t now = getExactTime();
    const float error = this->desired - measured;

    if (0.0f == this->desired && abs(error) < this->deadband_) {
        this->prevErr_ = 0.0f;
        this->output_  = 0.0f;
    } else {
        const millisecond_t d_time = now - this->prevUpdateTime_;
        this->integral_ = clamp(this->integral_ + error, -this->integralMax_, this->integralMax_);
        this->output_   = clamp(error * this->P_ + this->integral_ * this->I_ + (error - this->prevErr_) * d_time.get() * this->D_, this->outMin_, this->outMax_);
    }

    this->prevUpdateTime_ = now;
    this->prevErr_        = error;
}

}  // namespace micro
