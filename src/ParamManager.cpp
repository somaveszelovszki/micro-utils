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

auto ParamManager::getAll() -> Values {
    std::scoped_lock lock{mutex_};
    Values result;
    for (auto& [name, param] : params_) {
        result.insert({name, param.prev});
    }
    return result;
}

auto ParamManager::getChanged() -> Values {
    std::scoped_lock lock{mutex_};
    Values result;
    for (auto& [name, param] : params_) {
        if (param.updatePrev()) {
            result.insert({name, param.prev});
        }
    }
    return result;
}


auto ParamManager::update(const Values& newValues) -> Values {
    std::scoped_lock lock{mutex_};
    Values result;
    for (const auto& [name, value] : newValues) {
        if (auto it = params_.find(name); it != params_.end()) {
            auto& param = it->second;
            if (param.setValue(value) && param.updatePrev()) {
                result.insert({name, param.prev});
            }
        }
    }
    return result;
}

} // namespace micro
