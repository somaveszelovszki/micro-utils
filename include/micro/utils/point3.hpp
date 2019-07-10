#pragma once

#include <micro/utils/numeric.hpp>

namespace micro {

/* @brief Template implementation for 3-dimensional points.
 * @tparam T Numeric type of the coordinates.
 **/
template <typename T> struct point3;

template <typename T> using Vec3 = point3<T>;

template <typename T> struct bbox3;

template <typename T> struct point3 {
    /* @brief The X coordinate.
     **/
    T X;

    /* @brief The Y coordinate.
     **/
    T Y;

    /* @brief The Z coordinate.
     **/
    T Z;

    /* @brief Constructor - does not initializes coordinates.
     **/
    point3() {}

    /* @brief Constructor - initializes X and Y coordinates.
     * @param _X The X coordinate.
     * @param _Y The Y coordinate.
     * @param _Z The Z coordinate.
     **/
    point3(const T& _X, const T& _Y, const T& _Z)
        : X(_X)
        , Y(_Y)
        , Z(_Z) {}

    /* @brief Adds coordinates of this and the other point.
    * @param other The other point.
    * @returns The result of the addition.
    **/
    point3<T> operator+(const point3<T>& other) const {
        return point3<T>(this->X + other.X, this->Y + other.Y, this->Z + other.Z);
    }

    /* @brief Subtracts coordinates of the other point from the coordinates of this point.
     * @param other The other point.
     * @returns The result of the subtraction.
     **/
    point3<T> operator-(const point3<T>& other) const {
        return point3<T>(this->X - other.X, this->Y - other.Y, this->Z - other.Z);
    }

    /* @brief Adds coordinates of this and the other point and stores the result in this point.
     * @param other The other point.
     * @returns This point.
     **/
    point3<T>& operator+=(const point3<T>& other) {
        this->X += other.X;
        this->Y += other.Y;
        this->Z += other.Z;
        return *this;
    }

    /* @brief Subtracts coordinates of the other point from the coordinates of this point and stores the result in this point.
     * @param other The other point.
     * @returns This point.
     **/
    point3<T>& operator-=(const point3<T>& other) {
        this->X -= other.X;
        this->Y -= other.Y;
        this->Z -= other.Z;
        return *this;
    }

    /* @brief Multiplies coordinates of the point with the given constant.
     * @param c The constant.
     * @returns The result of the multiplication.
     **/
    template <typename T2, class = typename std::enable_if<std::is_arithmetic<T2>::value>::type>
    point3<T> operator*(const T2& c) const {
        return point3<T>(this->X * c, this->Y * c, this->Z * c);
    }

    /* @brief Divides coordinates of the point by the given constant.
     * @param c The constant.
     * @returns The result of the division.
     **/
    template <typename T2, class = typename std::enable_if<std::is_arithmetic<T2>::value>::type>
    point3<T> operator/(const T2& c) const {
        return point3<T>(this->X / c, this->Y / c, this->Z / c);
    }

    /* @brief Multiplies coordinates of the point with the given constant and stores the result in the point.
     * @param c The constant.
     * @returns This point.
     **/
    template <typename T2, class = typename std::enable_if<std::is_arithmetic<T2>::value>::type>
    point3<T>& operator*=(const T2& c) {
        this->X *= c;
        this->Y *= c;
        this->Z *= c;
        return *this;
    }

    /* @brief Divides coordinates of the point by the given constant and stores the result in the point.
     * @param c The constant.
     * @returns This point.
     **/
    template <typename T2, class = typename std::enable_if<std::is_arithmetic<T2>::value>::type>
    point3<T>& operator/=(const T2& c) {
        this->X /= c;
        this->Y /= c;
        this->Z /= c;
        return *this;
    }

    /* @brief Multiplies coordinates of the point with the given constant.
     * @param c The constant.
     * @param p The point.
     * @returns The result of the multiplication.
     **/
    template <typename T2, class = typename std::enable_if<std::is_arithmetic<T2>::value>::type>
    friend point3<T> operator*(const T2& c, const point3<T>& p) {
        return p * c;
    }

    /* @brief Casts point to another type.
     * @returns Point cast to another type.
     **/
    template <typename T2>
    operator point3<T2>() const {
        return point3<T2>(static_cast<T2>(this->X), static_cast<T2>(this->Y), static_cast<T2>(this->Z));
    }

    /* @brief Checks if two points are equal.
     * @param other The other point.
     * @returns Boolean value indicating if the two points are equal.
     **/
    bool operator==(const point3<T>& other) const {
        return this->X == other.X && this->Y == other.Y && this->Z == other.Z;
    }

    /* @brief Checks if two points are not equal.
     * @param other The other point.
     * @returns Boolean value indicating if the two points are not equal.
     **/
    bool operator!=(const point3<T>& other) const {
        return this->X != other.X || this->Y != other.Y || this->Z != other.Z;
    }

    /* @brief Calculates length of the point vector.
     * @returns The length of the point vector.
     **/
    T length() const {
        return micro::pythag(this->X, this->Y, this->Z);
    }

    /* @brief Calculates distance between the two points.
     * @param other The other point.
     * @returns The distance between the two points.
     **/
    T distance(point3<T> other) const {
        return micro::pythag(this->X - other.X, this->Y - other.Y, this->Z - other.Z);
    }

    /* @brief Calculates weighted average of the two points.
     * @param other The other point.
     * @param otherWeight The weight of the other point relative to this point - 1 by default.
     * @returns The average of the two points.
     **/
    point3<T> average(const point3<T>& other, float32_t otherWeight = 1.0f) const {
        float32_t weightSum = 1.0f + otherWeight;
        return point3<T>((this->X + other.X * otherWeight) / weightSum, (this->Y + other.Y + otherWeight) / weightSum, (this->Z + other.Z + otherWeight) / weightSum);
    }

    // @note numPoints must be at least 1
    static void bbox(const point3<T> points[], uint32_t numPoints, bbox3<T> *pResult);
};

typedef point3<float32_t>   point3f, Vec3f;          // 32-bit floating point types.
typedef point3<uint8_t>     point3ui8, Vec3ui8;      // 8-bit microigned integer point types.

/* @brief 3-dimensional bounding box.
 * @tparam T Numeric type of the coordinates.
 **/
template <typename T> struct bbox3 {
    point3<T> bl, tr;
};

template<typename T>
void point3<T>::bbox(const point3<T> points[], uint32_t numPoints, bbox3<T> *pResult) {
    pResult->bl = pResult->tr = points[0];
    for(uint32_t i = 1; i < numPoints; ++i) {
        point3<T> p = points[i];

        if (p.X < pResult->bl.X) {
            pResult->bl.X = p.X;
        } else if (p.X > pResult->tr.X) {
            pResult->tr.X = p.X;
        }

        if (p.Y < pResult->bl.Y) {
            pResult->bl.Y = p.Y;
        } else if (p.Y > pResult->tr.Y) {
            pResult->tr.Y = p.Y;
        }

        if (p.Z < pResult->bl.Z) {
            pResult->bl.Z = p.Z;
        } else if (p.Z > pResult->tr.Z) {
            pResult->tr.Z = p.Z;
        }
    }
}

} // namespace micro
