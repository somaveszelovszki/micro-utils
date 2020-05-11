//=====================================================================================================
// MadgwickAHRS.h
//=====================================================================================================
//
// Implementation of Madgwick's IMU and AHRS algorithms.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
//=====================================================================================================

#pragma once

#include <micro/math/numeric.hpp>
#include <micro/utils/units.hpp>
#include <micro/utils/point3.hpp>

namespace micro {

class MadgwickAHRS {
public:
    explicit MadgwickAHRS(const float beta);

    void update(const millisecond_t sampleTime, const point3<rad_per_sec_t>& gyro, const point3<m_per_sec2_t>& accel, const point3<gauss_t>& mag = point3<gauss_t>());

    radian_t roll() const;
    radian_t pitch() const;
    radian_t yaw() const;
    point3<radian_t> angles() const;

    void reset(const point3<radian_t>& angles);
private:
    const float beta;       // algorithm gain
    millisecond_t prevSampleTime;
    quaternion_t q;   // quaternion of sensor frame relative to auxiliary frame
};

} // namespace micro
