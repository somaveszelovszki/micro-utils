#pragma once

#include <micro/utils/point2.hpp>

namespace micro {
/** @brief Template implementation for 2-dimensional lines.
 * @tparam T Numeric type of the coordinates.
 */
template <typename T> struct line2 {
    /** @brief a*x + b*y + c = 0
    */
    T a, b, c;

    /* @brief Constructor - does not initializes coordinates.
     **/
    line2() {}

    line2(const T& a, const T& b, const T& c)
        : a(a)
        , b(b)
        , c(c) {
        this->normalize();
    }

    line2(const point2<T>& p1, const point2<T>& p2) {
        this->fromPoints(p1, p2);
    }

    line2(const point2<T>& p, radian_t angle) {
        this->fromPointAndAngle(p, angle);
    }

    /* @brief Casts line to another type.
     * @returns Line cast to another type.
     **/
    template <typename T2>
    operator line2<T2>() const {
        return line2<T2>(T2(a), T2(b), T2(c));
    }

    T normFactor() const {
        return std::sqrt(a * a + b * b);
    }

    void normalize() {
        const T factor = this->normFactor();
        a /= factor;
        b /= factor;
        c /= factor;
    }

    T getY(const T& x) const {
        return isZero(b) ? -(c / a) : -(a / b) * x - (c / b);
    }

    T getX(const T& y) const {
        return isZero(a) ? -(c / b) : -(b / a) * y - (c / a);
    }

    void fromPoints(const point2<T>& p1, const point2<T>& p2) {
        a = p1.Y - p2.Y;
        b = p2.X - p1.X;
        c = p1.X * p2.Y - p2.X * p1.Y;

        this->normalize();
    }

    void fromPointAndAngle(const point2<T>& p, radian_t angle) {
        const radian_t a = normalize360(angle);
        if (isZero(a - PI_2)) {
            this->fromPoints(p, { p.X, p.Y + T(1) });
        } else if (isZero(a + PI_2)) {
            this->fromPoints(p, { p.X, p.Y - T(1) });
        } else {
            this->fromPoints(p, { p.X + T(1), p.Y + T(tan(angle)) });
        }
    }
};

typedef line2<float>  line2f; // 32-bit floating point line.
typedef line2<double> line2d; // 64-bit floating point line.

} // namespace bcr

