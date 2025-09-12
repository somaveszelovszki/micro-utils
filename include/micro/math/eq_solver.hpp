#pragma once

#include <limits>
#include <utility>

#include <micro/math/numeric.hpp>

namespace micro {

/**
 * @brief Solves quadratic equation which is in form: A*x^2 + B*x + C
 */
template <typename T> std::pair<T, T> solve_quadratic(const T& A, const T& B, const T& C) {
    const T det = B * B - 4 * A * C;

    std::pair<T, T> result = {std::numeric_limits<T>::quiet_NaN(),
                              std::numeric_limits<T>::quiet_NaN()};

    if (isZero(det)) {
        result.first = -B / (2 * A);
    } else if (det > 0) {
        const T sq = sqrt(det);

        result.first  = (-B + sq) / (2 * A);
        result.second = (-B - sq) / (2 * A);
    }

    return result;
}

} // namespace micro
