#pragma once

#include "line2.hpp"
#include "eq_solver.hpp"

#include <utility>
#include <limits>

namespace micro {

template <typename T>
T distance(const line2<T>& line, const point2<T>& point) {
    return T(abs(valueOf(line.a) * valueOf(point.X) + valueOf(line.b) * valueOf(point.Y) + valueOf(line.c)) / line.normFactor());
}

template <typename T>
T distanceNorm(const line2<T>& lineNorm, const point2<T>& point) {
    return T(abs(valueOf(lineNorm.a) * valueOf(point.X) + valueOf(lineNorm.b) * valueOf(point.Y) + valueOf(lineNorm.c)));
}

template <typename T>
std::pair<point2<T>, point2<T>> lineCircle_intersection(const line2<T>& line, const point2<T>& circleCenter, const T& circleRadius) {
    float x_2 = 0, x_1 = 0, x_0 = 0;

    const point2<float> circleCenterRaw = { valueOf(circleCenter.X), valueOf(circleCenter.Y) };
    const float circleRadiusRaw = valueOf(circleRadius);

    if (isZero(line.b)) { // vertical line
        x_2 = 1;
        x_1 = -2 * circleCenterRaw.Y;
        x_0 = (line.c / line.a + circleCenterRaw.X) * (line.c / line.a + circleCenterRaw.X) + circleCenterRaw.Y * circleCenterRaw.Y - circleRadius * circleRadius;
    } else {
        // applies conversion between (0 = a*x + b*y + c) and (y = A*x + B)
        const float A = -line.a / line.b;
        const float B = -line.c / line.b;

        x_2 = 1 + A * A;
        x_1 = 2 * A * B - 2 * circleCenterRaw.X - 2 * A * circleCenterRaw.Y;
        x_0 = circleCenterRaw.X * circleCenterRaw.X + (B - circleCenterRaw.Y) * (B - circleCenterRaw.Y) - circleRadiusRaw * circleRadiusRaw;
    }

    const std::pair<float, float> x1_2 = solve_quadratic(x_2, x_1, x_0);

    std::pair<point2<T>, point2<T>> result = {
        { micro::numeric_limits<T>::quiet_NaN(), micro::numeric_limits<T>::quiet_NaN() },
        { micro::numeric_limits<T>::quiet_NaN(), micro::numeric_limits<T>::quiet_NaN() }
    };

    if (!micro::isnan(x1_2.first)) {
        result.first = { x1_2.first, line.getY(x1_2.first) };
    }

    if (!micro::isnan(x1_2.second)) {
        result.second = { x1_2.second, line.getY(x1_2.second) };
    }

    return result;
}

template <typename T>
point2<T> lineLine_intersection(const line2<T>& line1, const line2<T>& line2) {
    point2<T> intersection = { micro::numeric_limits<T>::infinity(), micro::numeric_limits<T>::infinity() };

    const line2f line1Raw(valueOf(line1.a), valueOf(line1.b), valueOf(line1.c));
    const line2f line2Raw(valueOf(line2.a), valueOf(line2.b), valueOf(line2.c));
    const float det = line2Raw.a * line1Raw.b - line1Raw.a * line2Raw.b;

    if (!isZero(det)) {
        intersection.X = T(line1Raw.c * line2Raw.b - line2Raw.c * line1Raw.b) / det;
        intersection.Y = T(line2Raw.c * line1Raw.a - line1Raw.c * line2Raw.a) / det;
    }
    return intersection;
}

} // namespace micro

