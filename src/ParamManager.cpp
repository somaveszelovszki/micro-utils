#include <exception>
#include <utility>
#include <variant>

#include <micro/debug/ParamManager.hpp>
#include <micro/math/numeric.hpp>

namespace micro {

bool ParamManager::Param::sync() {
    const auto newValue = std::visit([](const auto& v) -> value_type { return v.get(); }, current);
    return std::exchange(prev, newValue) != newValue;
}

bool ParamManager::Param::setCurrent(const value_type newValue) {
    return std::visit(
        [this](const auto& v) {
            return std::visit([&v](auto& c){
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
            }, current);
        }, newValue);
}

auto ParamManager::update(const Values& newValues) -> Values {
    std::scoped_lock lock{mutex_};

    Values changed;
    for (auto& [name, param] : params_) {
        if (const auto it = newValues.find(name); it != newValues.end()) {
            if (param.setCurrent(it->second) && param.sync()) {
                changed.insert({name, param.prev});
            }
        }
    }
    return changed;
}

auto ParamManager::sync() -> Values {
    std::scoped_lock lock{mutex_};

    Values changed;
    for (auto& [name, param] : params_) {
        if (param.sync()) {
            changed.insert({name, param.prev});
        }
    }
    return changed;
}

auto ParamManager::getAll() const -> Values {
    std::scoped_lock lock{mutex_};

    Values values;
    for (auto& [name, param] : params_) {
        values.insert({name, param.prev});
    }
    return values;
}


} // namespace micro
