#pragma once

#include "CarProps.hpp"
#include "ControlData.hpp"

#include <micro/container/vec.hpp>

namespace micro {

class Trajectory {
public:
    struct config_t {
        point2m pos;
        m_per_sec_t speed;
    };

    typedef vec<config_t, 200> configs_t;

    Trajectory(meter_t optoRowCarCenterDist, const config_t& startConfig)
        : optoRowCarCenterDist_(optoRowCarCenterDist)
        , configs_({ startConfig })
        , sectionStartConfig_(this->configs_.begin())
        , length_(0)
        , coveredDistanceUntilLastConfig_(-optoRowCarCenterDist)
        , carDistanceAtLastConfig_(0)
        , carDistanceSinceLastConfig_(0) {}

    meter_t length() const {
        return this->length_;
    }

    meter_t coveredDistance() const {
        return this->coveredDistanceUntilLastConfig_ + carDistanceSinceLastConfig_;
    }

    config_t lastConfig() const {
        return *this->configs_.back();
    }

    void appendLine(const config_t& dest);
    void appendSineArc(const config_t& dest, radian_t fwdAngle, uint32_t numSections);

    ControlData update(const CarProps car);

private:
    configs_t::const_iterator getClosestConfig(const point2m& pos) const;

    const meter_t optoRowCarCenterDist_; // Distance between optical sensor row and car center
    configs_t configs_;
    configs_t::const_iterator sectionStartConfig_;
    meter_t length_;
    meter_t coveredDistanceUntilLastConfig_;
    meter_t carDistanceAtLastConfig_;
    meter_t carDistanceSinceLastConfig_;
};

} // namespace micro
