#include <micro/port/task.hpp>
#include <micro/utils/timer.hpp>

namespace micro {

TIM_HandleTypeDef *tim_system = nullptr;

void time_init(TIM_HandleTypeDef *htim) {
    tim_system = htim;
}

millisecond_t getTime() {
    return millisecond_t(HAL_GetTick());
}

microsecond_t getExactTime() {
    os_enterCritical();
    const microsecond_t time = static_cast<microsecond_t>(getTime()) + microsecond_t(__HAL_TIM_GET_COUNTER(tim_system));
    os_exitCritical();
    return time;
}

} // namespace micro
