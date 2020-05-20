#pragma once

#include <micro/port/mutex.hpp>

#include <algorithm>
#include <iterator>
#include <mutex>

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
    atomic(Args&&... args) : value_(std::forward<Args>(args)...) {}

    T get() const {
        std::lock_guard<mutex_t> lock(this->mutex_);
        return this->value_;
    }

    void set(const T& value) {
        std::lock_guard<mutex_t> lock(this->mutex_);
        this->value_ = value;
    }

    void operator=(const T& value) {
        this->set(value);
    }

private:
    mutable mutex_t mutex_;
    T value_;
};

template <typename T> struct is_atomic { enum { value = micro::is_base_of_template<atomic, T>::value }; };

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
