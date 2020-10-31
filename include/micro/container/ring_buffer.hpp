#pragma once

#include <micro/utils/arrays.hpp>
#include <micro/math/numeric.hpp>

namespace micro {

/* @brief Ring buffer implementation.
 * @note This class is not concurrent.
 * @tparam T Type of the stored elements.
 * @tparam capacity The capacity of the buffer. Usable capacity is (capacity - 1), because the plus one element is used to determine if the buffer is empty or full.
 **/
template <typename T, uint32_t capacity_>
class ring_buffer {
public:
    /* @brief Default constructor - initializes head and tail indexes.
     **/
    ring_buffer()
        : head_(0)
        , tail_(0)
        , isFull_(false) {}

    /* @brief Gets size of the buffer.
     * @returns The current number of elements stored in the buffer.
     **/
    uint32_t size() const {
        return this->isFull_ ?
            capacity_ :
            this->head_ >= this->tail_ ?
                this->head_ - this->tail_ :
                capacity_ - this->tail_ + this->head_;
    }

    /* @brief Gets capacity of the buffer.
     * @returns The capacity of the buffer.
     **/
    uint32_t capacity() const {
        return capacity_;
    }

    const T* startRead() const {
        return this->size() > 0 ? &this->data_[this->tail_] : nullptr;
    }

    T* startWrite() {
        return !this->isFull_ ? &this->data_[this->head_] : nullptr;
    }

    void finishRead() {
        if (this->size() > 0) {
            this->tail_   = micro::incr_overflow(this->tail_, capacity_);
            this->isFull_ = false;
        }
    }

    void finishWrite() {
        if (!this->isFull_) {
            this->head_   = micro::incr_overflow(this->head_, capacity_);
            this->isFull_ = this->head_ == this->tail_;
        }
    }

    bool read(T& value) {
        const T * const readPtr = this->startRead();
        if (readPtr) {
            value = *readPtr;
            this->finishRead();
        }
        return !!readPtr;
    }

    bool write(const T& value) {
        T * const writePtr = this->startWrite();
        if (writePtr) {
            *writePtr = value;
            this->finishWrite();
        }
        return !!writePtr;
    }

private:
    T data_[capacity_]; // The buffer.
    uint32_t head_;     // The head - writing starts from this point.
    uint32_t tail_;     // The tail - reading starts from this point.
    bool isFull_;       // Indicates if the buffer is full.
};

} // namespace micro
