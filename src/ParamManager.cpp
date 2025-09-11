#include <micro/debug/ParamManager.hpp>
#include <micro/math/numeric.hpp>
#include <utility>
#include <variant>

namespace micro {

bool ParamManager::Param::sync() {
    const auto newValue = std::visit([](const auto& v) -> value_type { return v.get(); }, current);
    return std::exchange(prev, newValue) != newValue;
}

bool ParamManager::Param::setCurrent(const value_type newValue) {
    return std::visit(
        [this](const auto& v) {
            return std::visit(
                [&v](auto& c) {
                    using V = std::decay_t<decltype(v)>;
                    using C = std::decay_t<decltype(c.get())>;

                    if constexpr (!std::is_constructible_v<C, V>) {
                        return false;
                    }

                    if (const auto converted = numeric_cast<C>(v)) {
                        c.get() = *converted;
                        return true;
                    }

                    return false;
                },
                current);
        },
        newValue);
}

bool ParamManager::update(const Name& name, const value_type& value) {
    const auto it = params_.find(name);
    return it != params_.end() && it->second.setCurrent(value) && it->second.sync();
}

void ParamManager::sync(Values& OUT changedValues) {
    changedValues.clear();

    for (auto& [name, param] : params_) {
        if (param.sync()) {
            changedValues.insert({name, param.prev});
        }
    }
}

void ParamManager::getAll(Values& OUT values) const {
    values.clear();

    for (auto& [name, param] : params_) {
        values.insert({name, param.prev});
    }
}

} // namespace micro
