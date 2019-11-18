#pragma once

#include "line2.hpp"
#include "eq_solver.hpp"

#include <utility>
#include <limits>

namespace micro {

template <typename T>
T distance(const line2<T>& line, const point2<T>& point) {
    return abs(line.a * point.X + line.b * point.Y + line.c) / line.normFactor();
}

template <typename T>
T distanceNorm(const line2<T>& lineNorm, const point2<T>& point) {
    return abs(lineNorm.a * point.X + lineNorm.b * point.Y + lineNorm.c);
}

template <typename T>
micro::pair<point2<T>, point2<T>> lineCircle_intersection(const line2<T>& line, const point2<T>& circleCenter, const T& circleRadius) {
    T x_2 = T(0), x_1 = T(0), x_0 = T(0);

    if (isZero(line.b)) { // vertical line
        x_2 = 1;
        x_1 = -2 * circleCenter.Y;
        x_0 = (line.c / line.a + circleCenter.X) * (line.c / line.a + circleCenter.X) + circleCenter.Y * circleCenter.Y - circleRadius * circleRadius;
    } else {
        // applies conversion between (0 = a*x + b*y + c) and (y = A*x + B)
        const T A = -line.a / line.b;
        const T B = -line.c / line.b;

        x_2 = 1 + A * A;
        x_1 = 2 * A * B - 2 * circleCenter.X - 2 * A * circleCenter.Y;
        x_0 = circleCenter.X * circleCenter.X + (B - circleCenter.Y) * (B - circleCenter.Y) - circleRadius * circleRadius;
    }

    const micro::pair<T, T> x1_2 = solve_quadratic(x_2, x_1, x_0);

    micro::pair<point2<T>, point2<T>> result = {
        { std::numeric_limits<T>::quiet_NaN(), std::numeric_limits<T>::quiet_NaN() },
        { std::numeric_limits<T>::quiet_NaN(), std::numeric_limits<T>::quiet_NaN() }
    };

    if (!std::isnan(x1_2.first)) {
        result.first = { x1_2.first, line.getY(x1_2.first) };
    }

    if (!std::isnan(x1_2.second)) {
        result.second = { x1_2.second, line.getY(x1_2.second) };
    }

    return result;
}

template <typename T>
point2<T> lineLine_intersection(const line2<T>& line1, const line2<T>& line2) {
    point2<T> intersection = { std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity() };
    const T det = line2.a * line1.b - line1.a * line2.b;
    if (!isZero(det)) {
        intersection.X = (line1.c * line2.b - line2.c * line1.b) / det;
        intersection.Y = (line2.c * line1.a - line1.c * line2.a) / det;
    }
    return intersection;
}

} // namespace micro
