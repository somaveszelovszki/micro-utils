#include <micro/sensor/DistanceSensor.hpp>

namespace micro {

constexpr float COMPLIANCE_RATE = 0.3f;             // Compliance for new measurement - relative to previous measurement.
constexpr meter_t DEADBAND = centimeter_t(10.0f);   // Deadband for new measurement.

DistanceSensor::DistanceSensor(millisecond_t timeout, GPIO_TypeDef *gpio_trigger, uint16_t gpioPin_trigger, GPIO_TypeDef *gpio_echo, uint16_t gpioPin_echo)
    : Sensor(timeout)
    , sonar(gpio_trigger, gpioPin_trigger, gpio_echo, gpioPin_echo)
    , filter(COMPLIANCE_RATE, DEADBAND) {}

Status DistanceSensor::initialize() {
    this->sonar.initialize();
    return Status::OK;
}

Status DistanceSensor::run() {
    this->meas = this->filter.update(this->sonar.getDistance());
    this->sonar.startMeasurement();
    return Status::OK;
}

} // namespace micro
