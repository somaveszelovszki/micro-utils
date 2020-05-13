#include <micro/control/PID_Controller.hpp>
#include <micro/utils/timer.hpp>

namespace micro {

PID_Controller::PID_Controller(const PID_Params& params, const float outMax, const float deadband)
    : desired(0.0f)
    , params_(params)
    , outMax_(outMax)
    , deadband_(deadband)
    , prevErr_(0.0f)
    , integral_(0.0f)
    , output_(0.0f) {}

void PID_Controller::tune(const PID_Params& params) {
    this->params_ = params;
}

void PID_Controller::update(const float measured) {

    const millisecond_t now = getExactTime();
    const float error = this->desired - measured;

    if (0.0f == this->desired && abs(error) < this->deadband_) {
        this->prevErr_ = 0.0f;
        this->output_  = 0.0f;
    } else {
        const millisecond_t d_time = now - this->prevUpdateTime_;
        const float integral = this->integral_ + error;
        const float output   = error * this->params_.P + this->integral_ * this->params_.I + (error - this->prevErr_) * d_time.get() * this->params_.D;

        if (isBtw(output, -this->outMax_, this->outMax_)) {
            this->output_   = output;
            this->integral_ = integral;
        } else {
            this->output_ = clamp(output, -this->outMax_, this->outMax_);
        }
    }

    this->prevUpdateTime_ = now;
    this->prevErr_        = error;
}

}  // namespace micro
