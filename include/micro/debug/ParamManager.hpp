#pragma once

#include <functional>
#include <mutex>
#include <variant>

#include <etl/map.h>
#include <etl/string.h>

#include <micro/log/log.hpp>
#include <micro/port/mutex.hpp>
#include <micro/math/numeric.hpp>

#include <iostream>


namespace micro {

class ParamManager {
public:
    static constexpr size_t MAX_NUM_PARAMS = 32;

    using value_type = std::variant<
        bool,
        int8_t,
        int16_t,
        int32_t,
        uint8_t,
        uint16_t,
        uint32_t,
        float>;

    using reference_type = std::variant<
        std::reference_wrapper<bool>,
        std::reference_wrapper<int8_t>,
        std::reference_wrapper<int16_t>,
        std::reference_wrapper<int32_t>,
        std::reference_wrapper<uint8_t>,
        std::reference_wrapper<uint16_t>,
        std::reference_wrapper<uint32_t>,
        std::reference_wrapper<float>>;

    using Name = etl::string<32>;

    struct Param {
        value_type prev;
        reference_type current;

        bool updatePrev();
        bool setValue(const value_type newValue);
    };

    using Values = etl::map<Name, value_type, MAX_NUM_PARAMS>;

    template <typename T>
    void registerParam(const char *name, T& value) {
        std::scoped_lock lock{mutex_};
        params_.insert({Name{name}, Param{underlying_value(value), underlying_ref(value)}});
    }

    Values update(const bool notifyAllParams = false, const Values& newValues = {});

private:
    mutex_t mutex_;
    etl::map<Name, Param, MAX_NUM_PARAMS> params_;
};

#define REGISTER_PARAM(params, var) params.registerParam(#var, var)

} // namespace micro
