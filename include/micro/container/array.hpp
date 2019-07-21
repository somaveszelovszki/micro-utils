#pragma once

#include <micro/utils/arrays.hpp>

namespace micro {

/* @brief Stores array of given size and provides helper functions to manipulate elements.
 * @tparam T The type of the stored elements.
 * @tparam size The size of the array.
 **/
template <typename T, uint32_t size>
class array {

private:
    T data_[size];    // array of the elements.

    /* @brief Copies data from source array.
     * @param values The source array.
     **/
    void setValues(const T values[]) {
        micro::copy<size>(values, this->data_);
    }

public:

    /* @brief Default constructor - does not initialize array values.
     **/
    array() {}

    /* @brief Constructor - copies elements.
     * @param values The source array.
     **/
    explicit array(const T values[]) {
        this->setValues(values);
    }

    /* @brief Copy constructor - copies elements.
     * @param other The other array.
     **/
    array(const array<T, size>& other) {
        this->setValues(other.data);
    }

    /* @brief Copies data from the other array.
     * @param other The other array.
     * @returns This array.
     **/
    array<T, size>& operator=(const array<T, size>& other) {
        this->setValues(other.data);
        return *this;
    }

    /* @brief Casts array object to a C/C++ array.
     * @returns The array object as a C/C++ array.
     **/
    explicit operator T*() {
        return this->data_;
    }

    /* @brief Casts array object to a C/C++ const array.
     * @returns The array object as a C/C++ const array.
     **/
    explicit operator const T*() const {
        return this->data_;
    }

    /* @brief Gets element at given position.
     * @returns The element at the given position.
     **/
    T& operator[](uint32_t pos) {
        return this->data_[pos];
    }

    /* @brief Gets element at given position.
     * @returns The element at the given position.
     **/
    const T& operator[](uint32_t pos) const {
        return this->data_[pos];
    }

    /* @brief Checks if two array objects have equal values.
     * @param other The other array object.
     * @returns Boolean data indicating if the two array objects have equal values.
     **/
    bool operator==(const array<T, size>& other) const {
        bool eq = true;
        for (uint32_t i = 0; eq && i < size; ++i)
            eq = (*this)[i] == other[i];
        return eq;
    }

    /* @brief Checks if two array objects do not have equal values.
     * @param other The other array object.
     * @returns Boolean data indicating if the two array objects do not have equal values.
     **/
    bool operator!=(const array<T, size>& other) const {
        return !(*this == other);
    }

    /* @brief Concatenates two array objects.
     * @tparam size2 Size of the other array object.
     * @param other The other array object.
     * @param dest The destination array object.
     **/
    template <uint32_t size2>
    void concat(const array<T, size2>& other, array<T, size + size2>& dest) const {
        micro::concat<size, size2>(static_cast<const T*>(*this), static_cast<const T*>(other), static_cast<T*>(dest));
    }

    /* @brief Gets subarray of given length starting at given position.
     * @tparam size2 Size of the subarray.
     * @param pos Start index of subarray.
     * @returns The subarray.
     **/
    template <uint32_t size2>
    array<T, size2> subarray(uint32_t pos) {
        return array<T, size2>(this->data_ + pos);
    }

    /* @brief Appends element at the end of this array, and stores the result in the destination array object.
     * @tparam c The element to append.
     * @param dest The destination array object.
     **/
    void append(const T& c, array<T, size + 1>& dest) const {
        micro::copy<size>(this->data_, static_cast<T*>(dest));
        dest[size] = c;
    }

    /* @brief Concats two array objects.
     * @tparam size2 Size of the other array object.
     * @param other The other array object.
     * @returns The result array object.
     **/
    template <uint32_t size2>
    array<T, size + size2> operator+(const array<T, size2>& other) const {
        array<T, size + size2> result;
        concat(other, result);
        return result;
    }

    /* @brief Appends element at the end of this array, and returns the result array object.
     * @tparam c The element to append.
     * @returns The result array object.
     **/
    array<T, size + 1> operator+(const T& c) const {
        array<T, size + 1> result;
        this->append(c, result);
        return result;
    }

    /* @brief Appends element at the beginning of the byte array, and returns the result array object.
     * @tparam c The element to append.
     * @param bytes The source array object.
     * @returns The result array object.
     **/
    friend array<T, 1 + size> operator+(const T& c, const array<T, size>& bytes) {
        return array<T, 1>(&c) + bytes;
    }
};
} // namespace micro
