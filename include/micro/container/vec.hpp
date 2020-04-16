#pragma once

#include <micro/utils/algorithm.hpp>
#include <micro/utils/storage.hpp>
#include <micro/math/numeric.hpp>

#include <initializer_list>
#include <functional>

namespace micro {

template <typename T, uint32_t capacity_>
class vec_base {

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

    /* @brief Casts vector to a C array.
     * @returns The vector as a C array.
     **/
    T* data() {
        return this->data_;
    }

    /* @brief Casts vector to a C array.
     * @returns The vector as a C array.
     **/
    T* data() const {
        return this->data_;
    }

    uint32_t size() const {
        return this->size_;
    }

    uint32_t capacity() const {
        return capacity_;
    }

    bool empty() const {
        return !this->size_;
    }

    iterator erase(iterator iter) {
        if (iter >= this->begin() && iter < this->end()) {
            micro::shift_left(std::next(iter), this->back());
            this->back()->~T();
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
        return std::next(this->begin(), this->size_);
    }

    const_iterator end() const {
        return std::next(this->begin(), this->size_);
    }

    iterator back() {
        return this->begin() + this->size_ - 1;
    }

    const_iterator back() const {
        return this->begin() + this->size_ - 1;
    }

protected:
    void construct(const vec_base<T, capacity_>& other) {
        this->clear();
        for (const_iterator it = other.begin(); it != other.end(); ++it) {
            this->data_[this->size_++] = *it;
        }
    }

protected:
    T data_[capacity_];
    uint32_t size_;
};

template <typename T, uint32_t capacity_>
class vec : public vec_base<T, capacity_> {
private:
    typedef vec_base<T, capacity_> base_type;

public:
    typedef typename base_type::iterator iterator;
    typedef typename base_type::const_iterator const_iterator;
    typedef typename base_type::entry_type entry_type;

    /* @brief Default constructor - Sets size to 0.
     **/
    vec() {}

    /* @brief Copy constructor - copies elements.
     * @param other The other vector.
     **/
    vec(const vec<T, capacity_>& other) {
        this->construct(other);
    }

    vec(const std::initializer_list<T>& values) {
        this->push_back(values.begin(), values.end());
    }

    /* @brief Copies data from the other vector.
     * @param other The other vector.
     * @returns This vector.
     **/
    vec<T, capacity_>& operator=(const vec<T, capacity_>& other) {
        this->construct(other);
        return *this;
    }

    vec<T, capacity_>& operator=(const std::initializer_list<T>& values) {
        this->clear();
        this->push_back(values.begin(), values.end());
        return *this;
    }

    /* @brief Appends one element to the end of the vector.
     * @param value The element to append.
     * @returns The number of elements that have been appended successfully.
     **/
    uint32_t push_back(const T& value) {
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
    uint32_t push_back(Iter begin_, Iter end_) {
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
            micro::shift_right(iter, this->back());
            *iter = value;
            ++this->size_;
        }
        return this->size_ - prev_size;
    }

    /* @brief Emplaces one element at a given position of the vector.
     * @param iter The iterator.
     * @params args The constructor arguments.
     * @returns The number of elements that have been emplaced successfully.
     **/
    template<typename ...Args>
    uint32_t emplace(iterator iter, Args&&... args) {
        const uint32_t prev_size = this->size_;
        if (this->size() < this->capacity()) {
            micro::shift_right(iter, this->back());
            *iter = T(std::forward<Args>(args)...);
            ++this->size_;
        }
        return this->size_ - prev_size;
    }

    void pop_back() {
        this->erase(this->back());
    }
};

template <typename T, uint32_t capacity_>
class sorted_vec : public vec_base<T, capacity_> {
private:
    typedef vec_base<T, capacity_> base_type;

public:
    typedef typename base_type::iterator iterator;
    typedef typename base_type::const_iterator const_iterator;
    typedef typename base_type::entry_type entry_type;

    /* @brief Default constructor - Sets size to 0.
     **/
    sorted_vec() {}

    sorted_vec(const sorted_vec<T, capacity_>& other) {
        this->construct(other);
    }

    sorted_vec<T, capacity_>& operator=(const sorted_vec<T, capacity_>& other) {
        this->construct(other);
        return *this;
    }

    uint32_t insert(const T& value) {
        const uint32_t prev_size = this->size_;
        if (this->size() < this->capacity()) {
            const iterator iter = std::find_if_not(this->begin(), this->end(), [value](const T& other) { return value > other; });
            micro::shift_right(iter, this->back());
            *iter = value;
            ++this->size_;
        }
        return this->size_ - prev_size;
    }

    template <typename Iter>
    uint32_t insert(Iter begin_, Iter end_) {
        const uint32_t prev_size = this->size_;
        for (Iter it = begin_; it != end_; ++it) {
            if (this->size() < this->capacity()) {
                this->insert(*it);
            } else {
                break;
            }
        }
        return this->size_ - prev_size;
    }
};

} // namespace micro
