#include <micro/control/PID_Controller.hpp>
#include <micro/utils/timer.hpp>

namespace micro {

PID_Controller::PID_Controller(const PID_Params& params, const float outMax, const float maxRate, const float deadband)
    : target(0.0f)
    , params_(params)
    , outMax_(outMax)
    , maxRate_(maxRate)
    , deadband_(deadband)
    , prevErr_(0.0f)
    , integral_(0.0f)
    , output_(0.0f) {}

void PID_Controller::tune(const PID_Params& params) {
    this->params_ = params;
}

void PID_Controller::update(const float measured) {

    const float error = this->target - measured;
    this->update(error, error - this->prevErr_);
}

void PID_Controller::update(const float error, const float diff) {

    if (0.0f == this->target && abs(error) < this->deadband_) {
        this->prevErr_  = 0.0f;
        this->output_   = 0.0f;
        this->integral_ = 0.0f;
    } else {
        const float output = error * this->params_.P + this->integral_ * this->params_.I + diff * this->params_.D;

        this->output_ = clamp(output, this->output_ - this->maxRate_, this->output_ + this->maxRate_);

        if (isBtw(this->output_, -this->outMax_, this->outMax_)) {
            this->integral_ += error;
        } else {
            this->output_ = clamp(this->output_, -this->outMax_, this->outMax_);
        }
    }

    this->prevErr_ = error;
}

}  // namespace micro
