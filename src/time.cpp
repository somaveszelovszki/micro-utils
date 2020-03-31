#include <micro/utils/time_init.h>
#include <micro/utils/time.hpp>

#ifdef __GNUC__

//#define __ASM            __asm                                      /*!< asm keyword for GNU Compiler          */
//#define __INLINE         inline                                     /*!< inline keyword for GNU Compiler       */
//#define __STATIC_INLINE  static inline

#include "cmsis_gcc.h"

#endif // __GNUC__

namespace micro {

TIM_HandleTypeDef *tim_system = nullptr;

millisecond_t getTime() {
    return millisecond_t(HAL_GetTick());
}

microsecond_t getExactTime() {
    __disable_irq();
    const microsecond_t time = static_cast<microsecond_t>(getTime()) + microsecond_t(__HAL_TIM_GET_COUNTER(tim_system));
    __enable_irq();
    return time;
}

extern "C" void micro_time_init(TIM_HandleTypeDef *htim) {
    tim_system = htim;
}

} // namespace micro
