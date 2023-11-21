#pragma once

namespace micro {

template <typename T, size_t Alignment>
struct aligned_storage {
    struct type {
        alignas (Alignment) T value;
    };
};

template <typename T, size_t Alignment>
using aligned_storage_t = typename aligned_storage<T, Alignment>::type;

} // namespace micro
