#pragma once

#include <optional>

#include <etl/map.h>

#include <micro/math/numeric.hpp>

namespace micro {

template <typename K, typename V, const size_t N, typename Compare = etl::less<K>>
auto bounds(const etl::map<K, V, N, Compare>& m, const K& key) {
    auto lower = m.end();
    auto upper = m.upper_bound(key);

    if (upper == m.begin()) {
        lower = upper;
    } else if (upper == m.end()) {
        upper = std::next(m.begin(), m.size() - 1);
        lower = upper;
    } else {
        lower = std::prev(upper);
    }

    return std::make_pair(lower, upper);
}

template <typename K, typename V, const size_t N, typename Compare = etl::less<K>>
std::optional<V> lerp(const etl::map<K, V, N, Compare>& m, const K& key) {
    if (m.empty()) {
        return std::nullopt;
    }
    
    const auto [lower, upper] = bounds(m, key);
    return map(key, lower->first, upper->first, lower->second, upper->second);
}

} // namespace micro
