#pragma once

#include <micro/utils/numeric.hpp>

namespace micro {
namespace base {

template <typename A, typename B>
class pair {
public:
    /* @brief The left element.
     **/
    A left;

    /* @brief The right element.
     **/
    B right;
};

} // namespace base
} // namespace micro
