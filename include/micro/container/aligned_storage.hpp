#pragma once

#include <etl/pool.h>

namespace micro {
namespace detail {

template <typename T>
using aligned_element = typename etl::pool_ext<T>::element;

template <typename T>
union aligned_storage {
    T value;
    aligned_element<T> aligned;
};

} // namespace detail

template <typename T>
using aligned_storage = std::conditional<sizeof(T) < sizeof(detail::aligned_element<uint8_t>), detail::aligned_storage<T>, T>;

template <typename T>
using aligned_storage_t = typename aligned_storage<T>::type;

} // namespace micro
