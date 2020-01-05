#pragma once

#include <type_traits>

namespace micro {

/* @brief Rational number structure with operators.
 **/
class rational_t {
private:
    /* @brief Simplifies two rational number.
     * @note Simplifies only if numerator is a multiple of the denominator or vica versa.
     * @returns The simplified rational number.
     **/
    static constexpr rational_t simplify(const rational_t& r) {
        return r.num == r.den ? (rational_t){ 1, 1 }
        : r.num % r.den == 0 ? (rational_t){ r.num / r.den, 1 }
        : r.den % r.num == 0 ? (rational_t){ 1, r.den / r.num }
        : (rational_t){ r.num, r.den };
    }

public:
    int64_t num;    // The numerator.
    int64_t den;    // The denominator.

    /* @brief Multiplies two rational numbers.
     * @note Does not simplify result.
     * @param other The other rational number.
     * @returns The unsimplified result of the multiplication.
     **/
    constexpr rational_t operator*(const rational_t& other) const {
        return rational_t::simplify({ this->num * other.num, this->den * other.den });
    }

    /* @brief Divides two rational numbers.
     * @note Does not simplify result.
     * @param other The other rational number.
     * @returns The unsimplified result of the division.
     **/
    constexpr rational_t operator/(const rational_t& other) const {
        return rational_t::simplify({ this->num * other.den, this->den * other.num });
    }

    template <typename T, class = typename std::enable_if<std::is_arithmetic<T>::value>::type>
    constexpr rational_t operator*(const T& c) const {
        return { this->num * c, this->den };
    }

    template <typename T, class = typename std::enable_if<std::is_arithmetic<T>::value>::type>
    constexpr rational_t operator/(const T& c) const {
        return { this->num / c, this->den };
    }

    template <typename T, class = typename std::enable_if<std::is_arithmetic<T>::value>::type>
    constexpr friend rational_t operator*(const T& c, const rational_t& r) {
        return r * c;
    }

    template <typename T, class = typename std::enable_if<std::is_arithmetic<T>::value>::type>
    constexpr friend rational_t operator/(const T& c, const rational_t& r) {
        return { c * r.den, r.num };
    }

    /* @brief Calculates reciprocal of the rational number.
     * @returns The reciprocal of the rational number.
     **/
    constexpr rational_t reciprocal() const {
        return { this->den, this->num };
    }
};
} // namespace micro
