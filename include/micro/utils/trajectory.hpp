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

    explicit Trajectory(meter_t optoRowCarCenterDist)
        : carCenterToOptoRowCenter_(optoRowCarCenterDist, meter_t(0))
        , sectionStartConfig_(this->configs_.begin())
        , length_(0)
        , coveredDistanceUntilLastConfig_(-optoRowCarCenterDist)
        , carDistanceAtLastConfig_(0)
        , carDistanceSinceLastConfig_(0) {}

    meter_t length() const {
        return this->length_;
    }

    meter_t coveredDistance() const {
        return min(this->coveredDistanceUntilLastConfig_ + carDistanceSinceLastConfig_, this->length_);
    }

    config_t lastConfig() const {
        return *this->configs_.back();
    }

    void setStartConfig(const config_t& start, meter_t currentDist);
    void appendLine(const config_t& dest);
    void appendCircle(const point2m& center, radian_t angle, m_per_sec_t destSpeed, uint32_t numSections);
    void appendSineArc(const config_t& dest, radian_t fwdAngle, uint32_t numSections);

    ControlData update(const CarProps car);

    void clear() {
        this->configs_.clear();
        this->length_                         = meter_t(0);
        this->coveredDistanceUntilLastConfig_ = meter_t(0);
        this->carDistanceAtLastConfig_        = meter_t(0);
        this->carDistanceSinceLastConfig_     = meter_t(0);
    }

private:
    configs_t::const_iterator getClosestConfig(const point2m& pos) const;

    const vec2m carCenterToOptoRowCenter_; // Vector from the car center to the optical sensor row center
    configs_t configs_;
    configs_t::const_iterator sectionStartConfig_;
    meter_t length_;
    meter_t coveredDistanceUntilLastConfig_;
    meter_t carDistanceAtLastConfig_;
    meter_t carDistanceSinceLastConfig_;
};

} // namespace micro
