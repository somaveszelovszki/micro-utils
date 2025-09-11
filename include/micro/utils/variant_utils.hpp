#pragma once

#include <variant>

namespace micro {

template <typename... Ts> struct variant_visitor : Ts... {
    using Ts::operator()...;
};

template <typename... Ts> variant_visitor(Ts...) -> variant_visitor<Ts...>;

} // namespace micro
