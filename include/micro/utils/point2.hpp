#pragma once

#include <micro/math/unit_utils.hpp>

namespace micro {

/* @brief Template implementation for 2-dimensional points.
 * @tparam T Numeric type of the coordinates.
 **/
template <typename T> struct point2;

template <typename T> using vec2 = point2<T>;

template <typename T> struct bbox2;

template <typename T> struct point2 {
    /* @brief The X coordinate.
     **/
    T X;

    /* @brief The Y coordinate.
     **/
    T Y;

    point2(const T& fill = T())
        : X(fill)
        , Y(fill) {}

    point2(const T& x, const T& y)
        : X(x)
        , Y(y) {}

    /* @brief Casts point to another type.
     * @returns point cast to another type.
     **/
    template <typename T2>
    operator point2<T2>() const {
        return point2<T2>{ T2(this->X), T2(this->Y) };
    }

    /* @brief Checks if two points are equal
     * @param other The other point.
     * @returns Boolean value indicating if the two points are equal.
     **/
    template <typename T2>
    bool operator==(const point2<T2>& other) const {
        return this->X == other.X && this->Y == other.Y;
    }

    /* @brief Checks if two points are not equal
     * @param other The other point.
     * @returns Boolean value indicating if the two points are not equal.
     **/
    template <typename T2>
    bool operator!=(const point2<T2>& other) const {
        return this->X != other.X || this->Y != other.Y;
    }

    /* @brief Calculates length of the point vector.
     * @returns The length of the point vector.
     **/
    T length() const {
        return micro::pythag(this->X, this->Y);
    }
    
    /* @brief Calculates distance between the two points.
     * @param other The other point.
     * @returns The distance between the two points.
     **/
    template <typename T2>
    T distance(point2<T2> other) const {
        return micro::pythag(this->X - other.X, this->Y - other.Y);
    }

    /* @brief Calculates angle of the vector.
     **/
    radian_t getAngle() const {
        return micro::atan2(this->Y, this->X);
    }

    /* @brief Calculates angle of given vector using this point as the origo.
     * @param other The vector.
     **/
    template <typename T2>
    radian_t getAngle(const vec2<T2>& other) const {
        return micro::atan2(other.Y - this->Y, other.X - this->X);
    }

    /* @brief Calculates weighted average of the two points.
     * @param other The other point.
     * @param otherWeight The weight of the other point relative to this point - 1 by default.
     * @returns The average of the two points.
     **/
    template <typename T2>
    point2<T> average(const point2<T2>& other, float otherWeight = 1.0f) const {
        float weightSum = 1.0f + otherWeight;
        return point2<T>((this->X + other.X * otherWeight) / weightSum, (this->Y + other.Y * otherWeight) / weightSum);
    }

    point2<T> rotate(const float c, const float s) const {
        return {
            c * this->X - s * this->Y,
            s * this->X + c * this->Y
        };
    }

    point2<T> rotate(const radian_t& angle) const {
        return this->rotate(cos(angle), sin(angle));
    }

    point2<T> rotate90() const {
        return this->rotate(0.0f, 1.0f);
    }

    point2<T> rotate180() const {
        return this->rotate(-1.0f, 0.0f);
    }

    point2<T> rotate270() const {
        return this->rotate(0.0f, -1.0f);
    }

    // @note numpoints must be at least 1
    static void bbox(const point2<T> points[], uint32_t numpoints, bbox2<T> *pResult);

    bool isInside(const point2<T>& a, const point2<T>& b, const point2<T>& c) const;

    bool isInside(const bbox2<T>& bbox) const;
};

/* @brief Adds coordinates of this and the other point.
* @param other The other point.
* @returns The result of the addition.
**/
template <typename T1, typename T2>
point2<T1> operator+(const point2<T1>& p1, const point2<T2>& p2) {
    return point2<T1>{ p1.X + p2.X, p1.Y + p2.Y };
}

/* @brief Subtracts coordinates of the other point from the coordinates of this point.
 * @param other The other point.
 * @returns The result of the subtraction.
 **/
template <typename T1, typename T2>
point2<T1> operator-(const point2<T1>& p1, const point2<T2>& p2) {
    return point2<T1>{ p1.X - p2.X, p1.Y - p2.Y };
}

/* @brief Adds coordinates of this and the other point and stores the result in this point.
 * @param other The other point.
 * @returns This point.
 **/
template <typename T1, typename T2>
point2<T1>& operator+=(point2<T1>& p1, const point2<T2>& p2) {
    p1.X += p2.X;
    p1.Y += p2.Y;
    return p1;
}

/* @brief Subtracts coordinates of the other point from the coordinates of this point and stores the result in this point.
 * @param other The other point.
 * @returns This point.
 **/
template <typename T1, typename T2>
point2<T1>& operator-=(point2<T1>& p1, const point2<T2>& p2) {
    p1.X -= p2.X;
    p1.Y -= p2.Y;
    return p1;
}

/* @brief Multiplies coordinates of the point with the given constant.
 * @param p The point
 * @param c The constant.
 * @returns The result of the multiplication.
 **/
template <typename T1, typename T2, typename R = decltype (std::declval<T1>() * std::declval<T2>())>
typename std::enable_if<!is_base_of_template<point2, T2>::value, point2<R>>::type operator*(const point2<T1>& p, const T2& c) {
    return point2<R>(p.X * c, p.Y * c);
}

/* @brief Multiplies coordinates of the point with the given constant.
 * @param c The constant.
 * @param p The point
 * @returns The result of the multiplication.
 **/
template <typename T1, typename T2, typename R = decltype (std::declval<T1>() * std::declval<T2>())>
typename std::enable_if<!is_base_of_template<point2, T2>::value, point2<R>>::type operator*(const T1& c, const point2<T2>& p) {
    return p * c;
}

/* @brief Divides coordinates of the point by the given constant.
 * @param c The constant.
 * @returns The result of the division.
 **/
template <typename T1, typename T2, typename R = decltype (std::declval<T1>() / std::declval<T2>())>
typename std::enable_if<!is_base_of_template<point2, T2>::value, point2<R>>::type operator/(const point2<T1>& p, const T2& c) {
    return point2<R>(p.X / c, p.Y / c);
}

/* @brief Multiplies coordinates of the point with the given constant and stores the result in the point.
 * @param c The constant.
 * @returns This point.
 **/
template <typename T1, typename T2>
typename std::enable_if<std::is_arithmetic<T2>::value, point2<T1>&>::type operator*=(point2<T1>& p, const T2& c) {
    p.X *= c;
    p.Y *= c;
    return p;
}

/* @brief Divides coordinates of the point by the given constant and stores the result in the point.
 * @param c The constant.
 * @returns This point.
 **/
template <typename T1, typename T2>
typename std::enable_if<std::is_arithmetic<T2>::value, point2<T1>&>::type operator/=(point2<T1>& p, const T2& c) {
    p.X /= c;
    p.Y /= c;
    return p;
}

/* @brief Multiplies coordinates of the point with the given constant.
 * @param c The constant.
 * @param p The point.
 * @returns The result of the multiplication.
 **/
template <typename T1, typename T2>
typename std::enable_if<std::is_arithmetic<T2>::value, point2<T1>&>::type operator*=(const T2& c, const point2<T1>& p) {
    return p * c;
}

/* @brief 2-dimensional bounding box.
 * @tparam T Numeric type of the coordinates.
 **/
template <typename T> struct bbox2 {
    point2<T> bl, tr;   // Bottom left and top right points.
};

template<typename T>
void point2<T>::bbox(const point2<T> points[], uint32_t numpoints, bbox2<T> *pResult) {
    pResult->bl = pResult->tr = points[0];
    for(uint32_t i = 1; i < numpoints; ++i) {
        point2<T> p = points[i];

        if(p.X < pResult->bl->X)
            pResult->bl->X = p.X;
        else if(p.X > pResult->tr->X)
            pResult->tr->X = p.X;

        if(p.Y < pResult->bl->Y)
            pResult->bl->Y = p.Y;
        else if(p.Y > pResult->tr->Y)
            pResult->tr->Y = p.Y;
    }
}

template<typename T>
bool point2<T>::isInside(const bbox2<T>& bbox) const {
    return this->X >= bbox.bl.X && this->X < bbox.tr.X && this->Y >= bbox.bl.Y && this->Y < bbox.tr.Y;
}

template<typename T>
bool point2<T>::isInside(const point2<T>& a, const point2<T>& b, const point2<T>& c) const {
    const vec2<T> ap = *this - a, bp = *this - b, cp = *this - c,
        ab = b - a, bc = c - b, ca = a - c;

    const micro::Direction dir = micro::Direction::LEFT;
    const micro::Sign sA = ab.getAngleSign(ap, dir), sB = bc.getAngleSign(bp, dir), sC = ca.getAngleSign(cp, dir);

    // sum can be +-3 when all values are either POSITIVE or NEGATIVE,
    return abs(static_cast<int8_t>(sA) + static_cast<int8_t>(sB) + static_cast<int8_t>(sC)) == 3;
}

typedef point2<int32_t>      point2i, vec2i;
typedef point2<float>        point2f, vec2f;

typedef point2<meter_t>      point2m, vec2m;
typedef point2<m_per_sec_t>  vec2mps;

} // namespace micro

