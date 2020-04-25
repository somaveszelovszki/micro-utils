#pragma once

#include "types.hpp"

#include <FreeRTOS.h>
#include <semphr.h>

#include <algorithm>

namespace micro {

/* @brief Helper class for concurrent getting and setting of large data set.
 * Usage:   1. call set() to get setter reference to underlying data set
 *          2. set data
 *          3. call swap() to exchange references above underlying data sets
 *          4. call get() to get getter reference to underlying data set (this will point to the data set updated in 2.)
 */
template <typename T>
class swap_exchange {
public:
    /* @brief Constructor - initializes getter and setter pointers.
     */
    swap_exchange()
        : value_get(this->value1_)
        , value_set(this->value2_) {}

    const T& get() const { return this->value_get; }
    T& set() { return this->value_set; }

    /* @brief Swaps getter and setter pointers.
     * @note This function solves concurrency problems by swapping pointers in a critical section.
     */
    void swap() {
        std::swap(this->value_get, this->value_set);
    }
private:
    T value1_;
    T value2_;
    T& value_get;
    T& value_set;
};

template <typename T>
class atomic {
public:
    typedef T underlying_type;

    template<typename ...Args>
    atomic(SemaphoreHandle_t hmutex_, Args&&... args)
        : hmutex_(hmutex_)
        , value_(std::forward<Args>(args)...) {}

    T get() const {
        xSemaphoreTake(this->hmutex_, portMAX_DELAY);
        const T result = this->value_;
        xSemaphoreGive(this->hmutex_);
        return result;
    }

    void set(const T& value) {
        xSemaphoreTake(this->hmutex_, portMAX_DELAY);
        this->value_ = value;
        xSemaphoreGive(this->hmutex_);
    }

    void operator=(const T& value) {
        this->set(value);
    }

private:
    SemaphoreHandle_t hmutex_;
    T value_;
};

template <typename T> struct is_atomic { enum { value = micro::is_base_of_template<atomic, T>::value }; };

template <typename T>
T valueOf(const atomic<T>& value) {
    return value.get();
}

template <typename ForwardIt>
void shift_left(ForwardIt begin, ForwardIt end) {
    for (ForwardIt it = begin; it < end; ++it) {
        *std::prev(it) = *it;
    }
}

template <typename ForwardIt>
void shift_right(ForwardIt begin, ForwardIt end) {
    for (ForwardIt it = end; it > begin; --it) {
        *it = *std::prev(it);
    }
}

template< class Iter, class T>
T accumulate(Iter begin, Iter end, T init) {
    for (Iter it = begin; it < end; ++it) {
        init += *it;
    }
    return init;
}

template< class Iter, class T, class Operation>
T accumulate(Iter begin, Iter end, T init, Operation op) {
    for (Iter it = begin; it < end; ++it) {
        init = op(init, *it);
    }
    return init;
}

template <typename Iter>
void bubble_sort(Iter begin, Iter end) {
    for (Iter it1 = begin; it1 != end; ++it1) {
        for (Iter it2 = std::next(it1); it2 != end; ++it2) {
            if (*it1 > *it2) {
                std::swap(*it1, *it2);
            }
        }
    }
}

} // namespace micro
