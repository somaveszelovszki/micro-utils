#pragma once

#include <etl/string.h>
#include <functional>
#include <micro/container/map.hpp>
#include <micro/log/log.hpp>
#include <micro/math/numeric.hpp>
#include <micro/port/mutex.hpp>
#include <mutex>
#include <variant>

namespace micro {

class ParamManager {
  public:
    static constexpr size_t MAX_NUM_PARAMS = 12;

    using value_type =
        std::variant<bool, int8_t, int16_t, int32_t, uint8_t, uint16_t, uint32_t, float>;

    using reference_type =
        std::variant<std::reference_wrapper<bool>, std::reference_wrapper<int8_t>,
                     std::reference_wrapper<int16_t>, std::reference_wrapper<int32_t>,
                     std::reference_wrapper<uint8_t>, std::reference_wrapper<uint16_t>,
                     std::reference_wrapper<uint32_t>, std::reference_wrapper<float>>;

    using Name = etl::string<20>;

  private:
    static constexpr bool PLACEHOLDER = false;
    struct Param {
        value_type prev{PLACEHOLDER};
        reference_type current{std::ref(const_cast<bool&>(PLACEHOLDER))};

        bool sync();
        bool setCurrent(const value_type newValue);
    };

  public:
    using Values     = micro::map<Name, value_type, MAX_NUM_PARAMS>;
    using NamedParam = std::pair<Name, value_type>;

    template <typename T> void registerParam(const char* name, T& value) {
        std::scoped_lock lock{registerMutex_};
        params_.insert({Name{name}, Param{underlying_value(value), underlying_ref(value)}});
    }

    bool update(const Name& name, const value_type& value);
    void sync(Values& OUT changedValues);
    void getAll(Values& OUT values) const;

  private:
    mutex_t registerMutex_;
    micro::map<Name, Param, MAX_NUM_PARAMS> params_;
};

#define REGISTER_PARAM(params, var) params.registerParam(#var, var)

} // namespace micro
