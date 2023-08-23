#include <utility>
#include <variant>

#include <micro/debug/ParamManager.hpp>

namespace micro {

bool ParamManager::Param::updatePrev() {
    const auto newValue = std::visit([](const auto& v) -> value_type { return v.get(); }, current);
    return std::exchange(prev, newValue) != newValue;
}

bool ParamManager::Param::setValue(const value_type newValue) {
    return std::visit(
        [this](const auto& v) {
            if (auto *p = std::get_if<std::reference_wrapper<std::decay_t<decltype(v)>>>(&current)) {
                p->get() = v;
                return true;
            };
            return false;
        }, newValue);
}

auto ParamManager::update(const bool notifyAllParams, const Values& newValues) -> Values {
    std::scoped_lock lock{mutex_};
    Values result;
    for (auto& [name, param] : params_) {
        if (const auto it = newValues.find(name); it != newValues.end()) {
            if (!param.setValue(it->second)) {
                continue;
            }
        }

        if (param.updatePrev() || notifyAllParams) {
            result.insert({name, param.prev});
        }
    }
    return result;
}

} // namespace micro
