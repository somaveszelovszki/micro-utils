#pragma once

#include "CarProps.hpp"
#include "ControlData.hpp"

#include <micro/container/vec.hpp>

namespace micro {

class Trajectory {
public:

    enum class orientationUpdate_t {
        FIX_ORIENTATION = 1,
        PATH_ORIENTATION = 2
    };

    struct config_t {
        Pose pose;
        m_per_sec_t speed;
    };

    typedef vec<config_t, 500> configs_t;

    Trajectory()
        : sectionStartConfig_(this->configs_.begin())
        , length_(0)
        , coveredDistanceUntilLastConfig_(0)
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
    void appendSineArc(const config_t& dest, radian_t fwdAngle, orientationUpdate_t orientationUpdate, uint32_t numSections, radian_t sineStart, radian_t sineEnd);

    ControlData update(const CarProps car);

    void clear() {
        this->configs_.clear();
        this->sectionStartConfig_             = this->configs_.begin();
        this->length_                         = meter_t(0);
        this->coveredDistanceUntilLastConfig_ = meter_t(0);
        this->carDistanceAtLastConfig_        = meter_t(0);
        this->carDistanceSinceLastConfig_     = meter_t(0);
    }

private:
    configs_t::const_iterator getClosestConfig(const point2m& pos) const;

    configs_t configs_;
    configs_t::const_iterator sectionStartConfig_;
    meter_t length_;
    meter_t coveredDistanceUntilLastConfig_;
    meter_t carDistanceAtLastConfig_;
    meter_t carDistanceSinceLastConfig_;
};

} // namespace micro
