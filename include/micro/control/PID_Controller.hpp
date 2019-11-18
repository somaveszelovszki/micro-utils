#pragma once

#include <micro/utils/runnable.hpp>
#include <micro/math/unit_utils.hpp>

namespace micro {
/* @brief PID controller implementation.
 **/
class PID_Controller : public runnable_t {
public:
    /* @brief Constructor - sets period time and term weights.
     **/
    PID_Controller(millisecond_t Ts, millisecond_t Ti, millisecond_t Td, float Kc, float outMin, float outMax)
        : runnable_t(period)
        , desired(0.0f)
        , b0(Kc * (1 + period / Ti + Td / period))
        , b1(-Kc * (1 + 2 * Td / Ts))
        , b2(Kc * Td / Ts)
        , ek1(0.0f)
        , ek2(0.0f)
        , outMin(outMin)
        , outMax(outMax)
        , output(0.0f) {}

    void setParams(const PID_Controller& other) {
        this->b0 = other.b0;
        this->b1 = other.b1;
        this->b2 = other.b2;
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

        this->output = this->output + this->b0 * ek + this->b1 * this->ek1 + this->b2 * this->ek2;
        this->output = micro::clamp(this->output, this->outMin, this->outMax);
        this->ek2 = this->ek1;
        this->ek1 = ek;

        return Status::OK;
    }

    float desired;     // The desired value.

private:
    float b0, b1, b2;
    float ek1, ek2;
    float outMin;
    float outMax;
    float output;       // The output - updated in every cycle, holds the output value until the next update.
};
} // namespace micro
