#pragma once

#include <etl/pool.h>

namespace micro {

template <typename T>
union aligned_storage {
    T value;
    typename etl::pool_ext<T>::element aligned;
};

} // namespace micro
