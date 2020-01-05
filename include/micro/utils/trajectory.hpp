#pragma once

#include "CarProps.hpp"
#include "ControlData.hpp"

#include <micro/container/vec.hpp>

namespace micro {

class Trajectory {
public:
    struct Config {
        Pose pose;
        m_per_sec_t speed;
    };

    typedef vec<Config, 200> configs_t;

    Trajectory(meter_t optoCarCenterDist, const configs_t& configs)
        : optoCarCenterDist(optoCarCenterDist)
        , configs(configs)
        , sectionStartConfig(this->configs.begin()) {}

    ControlData getControl(const CarProps& car);

    configs_t::const_iterator getClosestConfig(const point2m& pos) const;

private:
    const meter_t optoCarCenterDist; // Distance between optical sensor row and car center
    configs_t configs;
    configs_t::const_iterator sectionStartConfig;
};

} // namespace micro
