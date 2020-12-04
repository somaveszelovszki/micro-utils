#pragma once

#include <micro/hw/Motor.hpp>

namespace micro {
namespace hw {

/* @brief Controls a DC motor using the standard servo interface.
 **/
class ServoInterfaceDcMotor : ServoInterfaceMotor<float> {
public:
    ServoInterfaceDcMotor(const micro::timer_t& timer, const uint32_t chnl);
};

/* @brief Controls a DC motor using direct full-bridge control.
 **/
class BridgeDcMotor : public Motor<float> {

public:
    BridgeDcMotor(const micro::timer_t& timer, const uint32_t chnl1, const uint32_t chnl2, const float maxDuty);

    BridgeDcMotor(const micro::timer_t& timer, const uint32_t chnl1, const uint32_t chnl2, const gpio_t& shutdown1, const gpio_t& shutdown2,
        const float maxDuty, const bool shutdownPinsEnabled = true);

    void start() override;

    void write(const float& duty) override;

    void stop() override;

private:
    const micro::timer_t timer_;
    const uint32_t chnl1_;
    const uint32_t chnl2_;
    const gpio_t shutdown1_;
    const gpio_t shutdown2_;
    const bool shutdownPinsEnabled_;

    const float maxDuty_;
};

} // namespace hw
} // namespace micro
