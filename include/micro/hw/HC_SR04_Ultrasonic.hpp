#pragma once

#include <micro/port/gpio.hpp>
#include <micro/port/semaphore.hpp>
#include <micro/utils/units.hpp>

namespace micro {
namespace hw {

class HC_SR04_Ultrasonic {
  public:
    HC_SR04_Ultrasonic(const gpio_t& gpio_trigger, const gpio_t& gpio_echo);

    void initialize();

    meter_t readDistance();

    void onEchoReceived();

  private:
    const gpio_t gpio_trigger_;
    const gpio_t gpio_echo_;

    semaphore_t echoSemaphore_;
    microsecond_t echoTime_;
};

} // namespace hw
} // namespace micro
