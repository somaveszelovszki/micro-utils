#pragma once

#include <micro/utils/storage.hpp>
#include <micro/math/numeric.hpp>

#include <initializer_list>

namespace micro {

template <typename T, uint32_t capacity_>
class vec {

public:
    typedef T* iterator;
    typedef const T* const_iterator;
    typedef T entry_type;

    T& operator[](uint32_t pos) {
        return this->data_[pos];
    }

    const T& operator[](uint32_t pos) const {
        return this->data_[pos];
    }

    /* @brief Default constructor - Sets size to 0.
     **/
    vec() : size_(0) {}

    /* @brief Copy constructor - copies elements.
     * @param other The other vector.
     **/
    vec(const vec<T, capacity_>& other)
        : size_(0) {
        this->append(other.begin(), other.end());
    }

    vec(const std::initializer_list<T>& values)
        : size_(0) {
        this->append(values.begin(), values.end());
    }

    /* @brief Copies data from the other vector.
     * @param other The other vector.
     * @returns This Vec.
     **/
    vec<T, capacity_>& operator=(const vec<T, capacity_>& other) {
        this->clear();
        this->append(other.begin(), other.end());
        return *this;
    }

    vec<T, capacity_>& operator=(const std::initializer_list<T>& values) {
        this->clear();
        this->append(values.begin(), values.end());
        return *this;
    }

    /* @brief Casts vector to a C array.
     * @returns The vector as a C array.
     **/
    T* data() {
        return reinterpret_cast<T*>(this->data_);
    }

    /* @brief Casts vector to a C array.
     * @returns The vector as a C array.
     **/
    T* data() const {
        return reinterpret_cast<const T*>(this->data_);
    }

    uint32_t size() const {
        return this->size_;
    }

    uint32_t capacity() const {
        return capacity_;
    }

    /* @brief Appends one element to the end of the vector.
     * @param value The element to append.
     * @returns The number of elements that have been appended successfully.
     **/
    uint32_t append(const T& value) {
        const uint32_t prev_size = this->size_;
        if (this->size() < this->capacity()) {
            this->data_[this->size_++] = value;
        }
        return this->size_ - prev_size;
    }

    /* @brief Emplaces one element to the end of the vector.
     * @params args The constructor arguments.
     * @returns The number of elements that have been emplaced successfully.
     **/
    template<typename ...Args>
    uint32_t emplace_back(Args&&... args) {
        const uint32_t prev_size = this->size_;
        if (this->size() < this->capacity()) {
            this->data_[this->size_++] = T(std::forward<Args>(args)...);
        }
        return this->size_ - prev_size;
    }

    /* @brief Appends elements to the end of the vector.
     * @param _data The elements to append.
     * @param _size Number of elements to append.
     * @returns The number of elements that have been appended successfully.
     **/
    template <typename Iter>
    uint32_t append(Iter begin_, Iter end_) {
        const uint32_t prev_size = this->size_;
        for (Iter it = begin_; it != end_; ++it) {
            if (this->size() < this->capacity()) {
                this->data_[this->size_++] = *it;
            } else {
                break;
            }
        }
        return this->size_ - prev_size;
    }

    uint32_t insert(iterator iter, const T& value) {
        const uint32_t prev_size = this->size_;
        if (iter > this->begin() && iter <= this->end() && this->size() < this->capacity()) {
            this->shiftRight(iter);
            *iter = value;
            this->size_++;
        }
        return this->size_ - prev_size;
    }

    /* @brief Emplaces one element at a given position of the vector.
     * @param iter The iterator.
     * @params args The constructor arguments.
     * @returns The number of elements that have been emplaced succesfully.
     **/
    template<typename ...Args>
    uint32_t emplace(iterator iter, Args&&... args) {
        const uint32_t prev_size = this->size_;
        if (this->size() < this->capacity()) {
            this->shiftRight(iter);
            *iter = T(std::forward<Args>(args)...);
            this->size_++;
        }
        return this->size_ - prev_size;
    }

    const_iterator find(const T& item) const {
        const_iterator it = this->begin();
        for (; it != this->end(); ++it) {
            if (*it == item) {
                break;
            }
        }
        return it;
    }

    iterator find(const T& item) {
        return const_cast<iterator>(const_cast<const vec*>(this)->find(item));
    }

    iterator erase(const T& item) {
        iterator iter = this->find(item);
        this->erase(iter);
        return iter;
    }

    iterator erase(iterator iter) {
        if (iter >= this->begin() && iter < this->end()) {
            iter->~T();
            for(iterator it = iter; it != this->end() - 1; ++it) {
                *it = *(it + 1);
            }
            this->size_--;
        }
        return iter;
    }

    const_iterator erase(const_iterator iter) {
        return this->erase(const_cast<iterator>(iter));
    }

    /* @brief Clears vector.
     **/
    void clear() {
        for (iterator it = this->begin(); it != this->end(); ++it) {
            it->~T();
        }
        this->size_ = 0;
    }

    iterator begin() {
        return reinterpret_cast<iterator>(this->data_);
    }

    const_iterator begin() const {
        return reinterpret_cast<const_iterator>(this->data_);
    }

    iterator end() {
        return reinterpret_cast<iterator>(this->data_ + this->size_);
    }

    const_iterator end() const {
        return reinterpret_cast<const_iterator>(this->data_ + this->size_);
    }

    iterator front() {
        return this->begin();
    }

    const_iterator front() const {
        return this->begin();
    }

    iterator back() {
        return this->begin() + this->size_ - 1;
    }

    const_iterator back() const {
        return this->begin() + this->size_ - 1;
    }

    void removeDuplicates() {
        if (this->size_ > 1) {
            for (const_iterator it1 = this->begin(); it1 != this->back() && it1 != this->end(); ++it1) {
                for (const_iterator it2 = it1 + 1; it2 != this->end();) {
                    if (*it1 == *it2) {
                        it2 = this->erase(it2);
                    } else {
                        ++it2;
                    }
                }
            }
        }
    }

private:
    void shiftLeft(iterator until) {
        for (iterator it = this->begin(); it != until; ++it) {
            *it = *(it + 1);
        }
    }

    void shiftRight(iterator from) {
        for (iterator it = this->end(); it != from; --it) {
            *it = *(it - 1);
        }
    }

    T data_[capacity_];
    uint32_t size_;
};
} // namespace micro
