#include <micro/hw/HC_SR04_Ultrasonic.hpp>
#include <micro/utils/numeric.hpp>
#include <micro/utils/time.hpp>

namespace micro {
namespace hw {

static constexpr uint32_t US_ROUNDTRIP_CM = 57; // Microseconds it takes sound to travel round-trip 1cm (2cm total), uses integer to save compiled code space.
static constexpr centimeter_t MAX_DIST(200);

centimeter_t echoTimeToDist(microsecond_t echoTime) {
    return centimeter_t(micro::max(echoTime.get() / US_ROUNDTRIP_CM, (echoTime.get() ? 1.0f : 0.0f)));
}

microsecond_t distToEchoTime(centimeter_t dist) {
    return microsecond_t(dist.get() * US_ROUNDTRIP_CM);
}

static constexpr microsecond_t ECHO_OVERHEAD(450);    // Overhead before echo.

HC_SR04_Ultrasonic::HC_SR04_Ultrasonic(GPIO_TypeDef *gpio_trigger, uint16_t gpioPin_trigger, GPIO_TypeDef *gpio_echo, uint16_t gpioPin_echo)
    : gpio_trigger(gpio_trigger)
    , gpioPin_trigger(gpioPin_trigger)
    , gpio_echo(gpio_echo)
    , gpioPin_echo(gpioPin_echo)
    , busy(false) {}

void HC_SR04_Ultrasonic::initialize() {
    HAL_GPIO_WritePin(this->gpio_trigger, this->gpioPin_trigger, GPIO_PIN_RESET);
    this->busy = false;
}

void HC_SR04_Ultrasonic::startMeasurement() {
    // if no echo signal has been received, sets measured value to zero and clears busy flag
    if (this->busy && (getExactTime() - this->lastStartTime > distToEchoTime(MAX_DIST))) {
        this->distance = MAX_DIST;
        this->busy = false;
    }

    // if sensor is not busy, starts next measurement by sending out the trigger impulse
    if (!this->busy) {
        this->lastStartTime = getExactTime();
        HAL_GPIO_WritePin(this->gpio_trigger, this->gpioPin_trigger, GPIO_PIN_RESET);
        while(getExactTime() - this->lastStartTime < microsecond_t(4)) {}
        HAL_GPIO_WritePin(this->gpio_trigger, this->gpioPin_trigger, GPIO_PIN_SET);
        while(getExactTime() - this->lastStartTime < microsecond_t(14)) {}
        HAL_GPIO_WritePin(this->gpio_trigger, this->gpioPin_trigger, GPIO_PIN_RESET);
        this->busy = true;
    }

    // TODO this is blocking now
    while(HAL_GPIO_ReadPin(this->gpio_echo, this->gpioPin_echo) != GPIO_PIN_SET &&
        micro::getExactTime() - this->lastStartTime <= millisecond_t(500)) {}

    const microsecond_t echoTime = getExactTime();
    this->distance = echoTime > this->lastStartTime ? echoTimeToDist(echoTime - this->lastStartTime) : centimeter_t::ZERO();
    this->busy = false;
}

void HC_SR04_Ultrasonic::onEchoReceived() {
    const microsecond_t echoTime = micro::getExactTime();
    this->distance = echoTime > this->lastStartTime - ECHO_OVERHEAD ? echoTimeToDist(echoTime - this->lastStartTime - ECHO_OVERHEAD) : MAX_DIST;
    this->busy = false;
}

} // namespace hw
} // namespace micro
