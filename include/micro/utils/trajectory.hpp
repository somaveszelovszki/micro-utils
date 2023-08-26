#pragma once

#include <etl/vector.h>

#include <micro/utils/CarProps.hpp>
#include <micro/utils/ControlData.hpp>

namespace micro {

struct LineInfo;

class Trajectory {
public:

    enum class orientationUpdate_t {
        FIX_ORIENTATION = 1,
        PATH_ORIENTATION = 2
    };

    struct config_t {
        Pose pose; // the desired position and forward angle (independent from the speed sign)
        m_per_sec_t speed;
    };

    typedef etl::vector<config_t, 500> configs_t;

    Trajectory()
        : sectionStartConfig_(this->configs_.end())
        , length_(0)
        , coveredDistanceUntilLastConfig_(0)
        , carDistanceAtLastConfig_(0)
        , carDistanceSinceLastConfig_(0) {}

    meter_t length() const {
        return this->length_;
    }

    meter_t coveredDistance() const {
        return this->coveredDistanceUntilLastConfig_ + carDistanceSinceLastConfig_;
    }

    config_t lastConfig() const {
        return this->configs_.back();
    }

    radian_t lastSpeedAngle() const;

    void setStartConfig(const config_t& start, meter_t currentDist);
    void appendLine(const config_t& dest);
    void appendCircle(const point2m& center, radian_t angle, m_per_sec_t destSpeed);
    void appendSineArc(const config_t& dest, radian_t fwdAngle, orientationUpdate_t orientationUpdate, radian_t sineStart, radian_t sineEnd);

    ControlData update(const CarProps& car);

    bool finished(const CarProps& car, const LineInfo& lineInfo, const meter_t lineDetectedDistanceThreshold) const;

    void clear() {
        this->configs_.clear();
        this->sectionStartConfig_             = this->configs_.begin();
        this->length_                         = meter_t(0);
        this->coveredDistanceUntilLastConfig_ = meter_t(0);
        this->carDistanceAtLastConfig_        = meter_t(0);
        this->carDistanceSinceLastConfig_     = meter_t(0);
    }

private:
    std::pair<configs_t::const_iterator, configs_t::const_iterator> getSectionBoundaries(const CarProps& car) const;

    configs_t::const_iterator getClosestConfig(const point2m& pos) const;

    static radian_t toCarOrientation(const radian_t fwdAngle, const m_per_sec_t speed);

    configs_t configs_;
    configs_t::const_iterator sectionStartConfig_;
    meter_t length_;
    meter_t coveredDistanceUntilLastConfig_;
    meter_t carDistanceAtLastConfig_;
    meter_t carDistanceSinceLastConfig_;
};

} // namespace micro
