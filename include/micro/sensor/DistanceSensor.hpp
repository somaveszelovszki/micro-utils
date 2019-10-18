#pragma once

#include <micro/hw/HC_SR04_Ultrasonic.hpp>
#include <micro/sensor/Sensor.hpp>
#include <micro/sensor/Filter.hpp>

namespace micro {

class DistanceSensor : public Sensor<centimeter_t> {
public:
    DistanceSensor(millisecond_t timeout, GPIO_TypeDef *gpio_trigger, uint16_t gpioPin_trigger, GPIO_TypeDef *gpio_echo, uint16_t gpioPin_echo);

    /* @brief Initializes sensor.
     * @returns Status indicating operation success.
     **/
    Status initialize();

    /* @brief Measures, stores and filters distance.
     * @returns Status indicating operation success.
     **/
    Status run();

    /* @brief Called when ECHO signal has been received. Calculates distance by the difference of the TRIGGER and the ECHO signals.
     **/
    void onEchoReceived() {
        this->sonar.onEchoReceived();
    }

private:
    hw::HC_SR04_Ultrasonic sonar;   // The ultrasonic distance sensor.
    BounceFilter<centimeter_t, 2> filter; // Filter for the measured distances. Drops positive and negative peaks.
};

} // namespace micro
