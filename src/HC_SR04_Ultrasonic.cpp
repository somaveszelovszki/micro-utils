#include <micro/hw/HC_SR04_Ultrasonic.hpp>
#include <micro/bsp/tim.hpp>
#include <micro/bsp/gpio.hpp>
#include <micro/utils/numeric.hpp>
#include <cfg_board.hpp>
#include <micro/utils/debug.hpp>

namespace micro {

static constexpr uint32_t US_ROUNDTRIP_CM = 57; // Microseconds it takes sound to travel round-trip 1cm (2cm total), uses integer to save compiled code space.

static constexpr centimeter_t MAX_DIST = centimeter_t(200);

centimeter_t echoTimeToDist(microsecond_t echoTime) {
    return centimeter_t(micro::max(echoTime.get() / US_ROUNDTRIP_CM, (echoTime.get() ? 1.0f : 0.0f)));
}

microsecond_t distToEchoTime(centimeter_t dist) {
    return microsecond_t(dist.get() * US_ROUNDTRIP_CM);
}

static constexpr microsecond_t ECHO_OVERHEAD = microsecond_t(450);    // Overhead before echo.

hw::HC_SR04_Ultrasonic::HC_SR04_Ultrasonic(const gpio_pin_struct& _trigger, const gpio_pin_struct& _echo)
    : trigger(_trigger)
    , echo(_echo)
    , busy(false) {}

void hw::HC_SR04_Ultrasonic::initialize() {
    GPIO_WritePin(this->trigger, PinState::RESET);
    this->busy = false;
}

void hw::HC_SR04_Ultrasonic::startMeasurement() {
    // if no echo signal has been received, sets measured value to zero and clears busy flag
//    if (this->busy && (micro::getTimerCounter(cfg::tim_System) - this->lastStartTime > distToEchoTime(MAX_DIST))) {
    if (this->busy && (micro::getExactTime() - this->lastStartTime > distToEchoTime(MAX_DIST))) {
        this->distance = MAX_DIST;
        this->busy = false;
    }

    // if sensor is not busy, starts next measurement by sending out the trigger impulse
    if (!this->busy) {
        this->lastStartTime = micro::getExactTime();
        //this->lastStartTime_us = micro::getTime().get<microseconds>() + micro::getTimerCounter(cfg::tim_System);
        GPIO_WritePin(this->trigger, PinState::RESET);
        while(micro::getExactTime() - this->lastStartTime < microsecond_t(4)) {}
        GPIO_WritePin(this->trigger, PinState::SET);
        while(micro::getExactTime() - this->lastStartTime < microsecond_t(14)) {}
        GPIO_WritePin(this->trigger, PinState::RESET);
        this->busy = true;
    }

    while(GPIO_ReadPin(this->echo) != PinState::SET) {
        if (micro::getExactTime() - this->lastStartTime > millisecond_t(500)){
            break;
        }
    }
    const microsecond_t echoTime = micro::getExactTime();
    this->distance = echoTime > this->lastStartTime ? echoTimeToDist(echoTime - this->lastStartTime) : centimeter_t::ZERO();
    this->busy = false;
}

void hw::HC_SR04_Ultrasonic::onEchoReceived() {
    const microsecond_t echoTime = micro::getExactTime();
    this->distance = echoTime > this->lastStartTime - ECHO_OVERHEAD ? echoTimeToDist(echoTime - this->lastStartTime - ECHO_OVERHEAD) : MAX_DIST;
    this->busy = false;
}

} // namespace micro
