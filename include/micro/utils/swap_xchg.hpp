#pragma once

#include <micro/utils/types.hpp>

#include <FreeRTOS.h>
#include <task.h>

#include <algorithm>

namespace micro {

template <typename T>
class swap_xchg {
public:
    /* @brief Constructor - initializes getter and setter pointers.
     */
    swap_xchg(void)
        : value_GET(&this->value1)
        , value_SET(&this->value2) {}

    /* @brief Gets getter pointer.
     * @returns The getter pointer.
     */
    const T* get() const { return this->value_GET; }

    /* @brief Gets setter pointer.
     * @returns The setter pointer.
     */
    T* set() { return this->value_SET; }

    /* @brief Swaps getter and setter pointers.
     * @note This function solves concurrency problems by swapping pointers in a critical section.
     */
    void swap() {
        taskENTER_CRITICAL();
        std::swap(this->value_GET, this->value_SET);
        taskEXIT_CRITICAL();
    }

private:
    T value1, value2;
    T *value_GET, *value_SET;    // Getter and setter pointers to the underlying data sets.
};

} // namespace micro
