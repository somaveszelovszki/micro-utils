#pragma once

#include <micro/utils/runnable.hpp>
#include <micro/math/unit_utils.hpp>

namespace micro {
/* @brief PD controller implementation.
 **/
class PD_Controller : public runnable_t {
public:
    /* @brief Constructor - sets period time and term weights.
     **/
    PD_Controller(float P, float D, float outMin, float outMax)
        : runnable_t(period)
        , desired(0.0f)
        , P(P)
        , D(D)
        , ek1(0.0f)
        , outMin(outMin)
        , outMax(outMax)
        , output(0.0f) {}

    void setParams(float P, float D) {
        this->P = P;
        this->D = D;
    }

    /* @brief Gets output.
     * @returns The output.
     **/
    float getOutput() const {
        return this->output;
    }

    /* @brief Updates output according to current and previous errors.
     * @note If an error occurs (see result status), output will not be updated!
     * @note This is an internal method, use MEASURE(meas) macro instead!
     * @param measured The current measured speed.
     * @returns Status indicating operation success.
     **/
    Status run(const float measured) {
        this->updateTimeDiff();

        float ek = this->desired - measured;

        this->output = P * ek + D * (ek - ek1);
        this->output = micro::clamp(this->output, this->outMin, this->outMax);

        this->ek1 = ek;

        return Status::OK;
    }

    float desired;     // The desired value.

private:
    float P, D;
    float ek1;
    float outMin;
    float outMax;
    float output;       // The output - updated in every cycle, holds the output value until the next update.
};
} // namespace micro
