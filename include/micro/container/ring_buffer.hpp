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
private:
    T data_[capacity_];          // The buffer.
    volatile uint32_t head_;     // The head - writing starts from this point.
    volatile uint32_t tail_;     // The tail - reading starts from this point.

public:
    /* @brief Default constructor - initializes head and tail indexes.
     **/
    ring_buffer()
        : head_(0)
        , tail_(0) {}

    /* @brief Gets writable buffer pointer.
     * @note Do not write to any elements other than this one!
     * @returns Pointer to the writable buffer.
     **/
    T* getWritableBuffer() {
        return this->data_ + this->head_;
    }

    /* @brief Gets readable buffer pointer.
     * @returns Pointer to the readable buffer.
     **/
    const T* getReadableBuffer() const {
        return this->data_ + this->tail_;
    }

    /* @brief Updates head position.
     * @param _count The number of elements that have been read.
     **/
    void updateHead(uint32_t _count) {
        this->head_ = (this->head_ + _count) % capacity_;
    }

    /* @brief Updates tail position.
     * @param _count The number of elements that have been written.
     **/
    void updateTail(uint32_t _count) {
        this->tail_ = (this->tail_ + _count) % capacity_;
    }

    /* @brief Puts new element into the buffer.
     * @param value The element to add.
     * @returns Status indicating if element has been added.
     **/
    Status put(const T& value);

    /* @brief Puts new elements into the buffer.
     * @note Either all or none of the elements will be added, depending on the buffer's free space.
     * @param values The array of elements to add.
     * @param _count The number of elements to add.
     * @returns Status indicating if element has been added.
     **/
    Status put(const T * const values, uint32_t _count);

    /* @brief Gets oldest element from the buffer.
     * @param pDest Pointer to the object that will store the result.
     * @returns Status indicating if element has been read.
     **/
    Status get(T *pDest);

    Status peek(T *pDest) {
        bool _get = this->head_ != this->tail_;
        if (_get) {
            *pDest = this->data_[this->tail_];
        }
        return _get ? Status::OK : Status::BUFFER_EMPTY;
    }

    /* @brief Gets oldest elements from the buffer.
     * @note Either all or none of the elements will be read, depending on the given count parameter.
     * @param buffer Buffer that will store the result elements.
     * @param _count The number of elements to get.
     * @returns Status indicating if element has been read.
     **/
    Status get(T *buffer, uint32_t _count);

    /* @brief Gets size of the buffer.
     * @returns The current number of elements stored in the buffer.
     **/
    uint32_t size() const {
        const uint32_t head = this->head_;
        const uint32_t tail = this->tail_;
        return head >= tail ? head - tail : capacity_ + head - tail;
    }

    /* @brief Gets capacity of the buffer.
     * @returns The capacity of the buffer.
     **/
    uint32_t capacity() const {
        return capacity_;
    }

    /* @brief Gets size of the buffer that is safe to read without addressing out of the buffer (may not be equal to the buffer size).
     * @returns The buffer's safe size.
     **/
    uint32_t safeReadSize() const {
        const uint32_t head = this->head_;
        const uint32_t tail = this->tail_;
        uint32_t safe_size = min(this->size(), capacity_ - this->tail_);
        return safe_size;
    }

    /* @brief Gets index of given value in buffer.
     * @param value The value to search for.
     * @param start The start position (relative to the tail).
     * @returns Index of the value or -1 if not found.
     **/
    int32_t indexOf(const T& value, uint32_t start = 0);
};

template <typename T, uint32_t capacity_>
Status ring_buffer<T, capacity_>::put(const T& value) {
    bool _put = this->size() + 1 < capacity_;
    if (_put) {
        this->data_[this->head_] = value;
        this->updateHead(1);
    }

    Status result = _put ? Status::OK : Status::BUFFER_FULL;
    return result;
}

template <typename T, uint32_t capacity_>
Status ring_buffer<T, capacity_>::put(const T *const values, uint32_t _count) {
    bool _put = this->size() + _count < capacity_;

    if (_put) {
        uint32_t sizeAfterHead = min(_count, capacity_ - this->head_),
            sizeBeforeTail = _count > sizeAfterHead ? _count - sizeAfterHead : 0;

        micro::copy(values, this->data_ + this->head_, sizeAfterHead);
        if (sizeBeforeTail > 0) {
            micro::copy(values + sizeAfterHead, this->data_, sizeBeforeTail);
        }

        this->updateHead(_count);
    }

    Status result = _put ? Status::OK : Status::BUFFER_FULL;
    return result;
}

template <typename T, uint32_t capacity_>
Status ring_buffer<T, capacity_>::get(T *pDest) {
    bool _get = !!this->size();
    if (_get) {
        *pDest = this->data_[this->tail_];
        this->updateTail(1);
    }

    Status result = _get ? Status::OK : Status::NO_NEW_DATA;
    return result;
}

template <typename T, uint32_t capacity_>
Status ring_buffer<T, capacity_>::get(T * const buffer, uint32_t _count) {
    bool _get = this->size() >= _count;
    if (_get) {
        uint32_t sizeAfterTail = min(_count, capacity_ - this->tail_),
            sizeBeforeHead = _count > sizeAfterTail ? _count - sizeAfterTail : 0;

        micro::copy(this->data_ + this->tail_, buffer, sizeAfterTail);
        if (sizeBeforeHead > 0) {
            micro::copy(this->data_, buffer, sizeBeforeHead);
        }

        this->updateTail(_count);
    }

    Status result = _get ? Status::OK : Status::NO_NEW_DATA;
    return result;
}

template<typename T, uint32_t capacity_>
int32_t ring_buffer<T, capacity_>::indexOf(const T& value, uint32_t start) {
    uint32_t i;
    int32_t idx = -1;

    // searches after tail
    uint32_t sizeAfterTail = this->safeReadSize();
    for (i = start; i < sizeAfterTail; ++i) {
        if (this->data_[this->tail_ + i] == value) {
            idx = i;
            break;
        }
    }

    // if not found, continues search before head
    if (idx == -1) {
        uint32_t sizeBeforeHead = this->size() - sizeAfterTail;
        for (i = 0; i < sizeBeforeHead; ++i) {
            if (this->data_[i] == value) {
                idx = sizeAfterTail + i;
                break;
            }
        }
    }

    return idx;
}
} // namespace micro
