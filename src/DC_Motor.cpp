#include <micro/hw/DC_Motor.hpp>
#include <micro/math/numeric.hpp>

namespace micro {
namespace hw {

DC_Motor::DC_Motor(TIM_HandleTypeDef *htim, const uint32_t chnl1, const uint32_t chnl2, const float maxDuty)
    : htim_(htim)
    , chnl1_(chnl1)
    , chnl2_(chnl2)
    , maxDuty_(maxDuty) {

    this->write(0.0f);
}

void DC_Motor::write(float duty) {

    duty = clamp(duty, -this->maxDuty_, this->maxDuty_);

#ifdef BSP_LIB_HAL
    const uint32_t pwm1 = map<float, uint32_t>(duty, -1.0f, 1.0f, 0, static_cast<uint32_t>(this->htim_->Instance->ARR));
    const uint32_t pwm2 = this->htim_->Instance->ARR - pwm1;
#endif // BSP_LIB_HAL

    __HAL_TIM_SET_COMPARE(this->htim_, this->chnl1_, pwm1);
    __HAL_TIM_SET_COMPARE(this->htim_, this->chnl2_, pwm2);
}

} // namespace hw
} // namespace micro
