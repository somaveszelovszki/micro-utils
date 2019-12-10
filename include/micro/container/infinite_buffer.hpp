#pragma once

#include <micro/utils/arrays.hpp>
#include <micro/math/numeric.hpp>

namespace micro {

/* @brief Infinite buffer implementation.
 * @note This class is not concurrent.
 * @tparam T Type of the stored elements.
 * @tparam capacity The capacity of the buffer.
 **/
template <typename T, uint32_t capacity_>
class infinite_buffer {
private:
    T data_[capacity_]; // The buffer.
    uint32_t idx_;      // The index of the last inserted element.

public:
    /* @brief Default constructor - initializes last inserted index.
     **/
    infinite_buffer() : idx_(capacity_ - 1) {}

    /* @brief Puts new element into the buffer.
     * @param value The element to add.
     * @returns Status indicating if element has been added.
     **/
    void push_back(const T& value) {
        ++this->idx_;
        if (capacity_ == this->idx_) this->idx_ = 0;
        this->data_[this->idx_] = value;
    }

    T& peek_back(uint32_t step) {
        return this->data_[this->idx_ >= step ? this->idx_ - step : capacity_ + this->idx_ - step];
    }

    const T& peek_back(uint32_t step) const {
        return const_cast<infinite_buffer*>(this)->peek_back(step);
    }

    /* @brief Gets capacity of the buffer.
     * @returns The capacity of the buffer.
     **/
    uint32_t capacity() const {
        return capacity_;
    }
};

} // namespace micro
