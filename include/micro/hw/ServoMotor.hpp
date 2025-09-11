#pragma once

#include <micro/hw/Motor.hpp>
#include <micro/math/unit_utils.hpp>

namespace micro {
namespace hw {

/* @brief Controls a servo motor using the standard servo interface.
 **/
class ServoMotor : public ServoInterfaceMotor<radian_t> {
  public:
    ServoMotor(const micro::timer_t& timer, uint32_t chnl, uint32_t pwmCenter,
               const radian_t& positiveTransferRate, const radian_t& negativeTransferRate,
               radian_t maxAngle, rad_per_sec_t maxAngVel);

    radian_t angle() const;

    void write(const radian_t& angle) override;

  private:
    const rad_per_sec_t maxAngVel_; // The maximum angular velocity

    radian_t actualAngle_;
    microsecond_t prevAngleUpdateTime_;
};

} // namespace hw
} // namespace micro
