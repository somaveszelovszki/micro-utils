#include <micro/math/linalg.hpp>
#include <micro/math/unit_utils.hpp>
#include <micro/sensor/MadgwickAHRS.hpp>
#include <micro/utils/timer.hpp>

namespace micro {

MadgwickAHRS::MadgwickAHRS(const float beta)
    : beta(beta)
    , prevSampleTime(getExactTime())
    , q{ 1.0f, 0.0f, 0.0f, 0.0f } {}

void MadgwickAHRS::update(const millisecond_t sampleTime, const point3<rad_per_sec_t>& gyro, const point3<m_per_sec2_t>& accel, const point3<gauss_t>& mag) {
    float gx = gyro.X.get(),  gy = gyro.Y.get(),  gz = gyro.Z.get();
    float ax = accel.X.get(), ay = accel.Y.get(), az = accel.Z.get();
    float mx = mag.X.get(),   my = mag.Y.get(),   mz = mag.Z.get();

    float s0, s1, s2, s3;
    float qDot1, qDot2, qDot3, qDot4;
    float hx, hy;
    float _2q0mx, _2q0my, _2q0mz, _2q1mx, _2bx, _2bz, _4bx, _4bz, _2q0, _2q1, _2q2, _2q3, _2q0q2, _2q2q3, q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;

    // Rate of change of quaternion from gyroscope
    qDot1 = 0.5f * (-q.q1 * gx - q.q2 * gy - q.q3 * gz);
    qDot2 = 0.5f * ( q.q0 * gx + q.q2 * gz - q.q3 * gy);
    qDot3 = 0.5f * ( q.q0 * gy - q.q1 * gz + q.q3 * gx);
    qDot4 = 0.5f * ( q.q0 * gz + q.q1 * gy - q.q2 * gx);

    micro::normalize(ax, ay, az);
    micro::normalize(mx, my, mz);

    // Auxiliary variables to avoid repeated arithmetic
    _2q0mx = 2.0f * q.q0 * mx;
    _2q0my = 2.0f * q.q0 * my;
    _2q0mz = 2.0f * q.q0 * mz;
    _2q1mx = 2.0f * q.q1 * mx;
    _2q0 = 2.0f * q.q0;
    _2q1 = 2.0f * q.q1;
    _2q2 = 2.0f * q.q2;
    _2q3 = 2.0f * q.q3;
    _2q0q2 = 2.0f * q.q0 * q.q2;
    _2q2q3 = 2.0f * q.q2 * q.q3;
    q0q0 = q.q0 * q.q0;
    q0q1 = q.q0 * q.q1;
    q0q2 = q.q0 * q.q2;
    q0q3 = q.q0 * q.q3;
    q1q1 = q.q1 * q.q1;
    q1q2 = q.q1 * q.q2;
    q1q3 = q.q1 * q.q3;
    q2q2 = q.q2 * q.q2;
    q2q3 = q.q2 * q.q3;
    q3q3 = q.q3 * q.q3;

    // Reference direction of Earth's magnetic field
    hx = mx * q0q0 - _2q0my * q.q3 + _2q0mz * q.q2 + mx * q1q1 + _2q1 * my * q.q2 + _2q1 * mz * q.q3 - mx * q2q2 - mx * q3q3;
    hy = _2q0mx * q.q3 + my * q0q0 - _2q0mz * q.q1 + _2q1mx * q.q2 - my * q1q1 + my * q2q2 + _2q2 * mz * q.q3 - my * q3q3;
    _2bx = sqrt(hx * hx + hy * hy);
    _2bz = -_2q0mx * q.q2 + _2q0my * q.q1 + mz * q0q0 + _2q1mx * q.q3 - mz * q1q1 + _2q2 * my * q.q3 - mz * q2q2 + mz * q3q3;
    _4bx = 2.0f * _2bx;
    _4bz = 2.0f * _2bz;

    // Gradient decent algorithm corrective step
    s0 = -_2q2 * (2.0f * q1q3 - _2q0q2 - ax) + _2q1 * (2.0f * q0q1 + _2q2q3 - ay) - _2bz * q.q2 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * q.q3 + _2bz * q.q1) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * q.q2 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
    s1 = _2q3 * (2.0f * q1q3 - _2q0q2 - ax) + _2q0 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * q.q1 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + _2bz * q.q3 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * q.q2 + _2bz * q.q0) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * q.q3 - _4bz * q.q1) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
    s2 = -_2q0 * (2.0f * q1q3 - _2q0q2 - ax) + _2q3 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * q.q2 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + (-_4bx * q.q2 - _2bz * q.q0) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * q.q1 + _2bz * q.q3) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * q.q0 - _4bz * q.q2) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
    s3 = _2q1 * (2.0f * q1q3 - _2q0q2 - ax) + _2q2 * (2.0f * q0q1 + _2q2q3 - ay) + (-_4bx * q.q3 + _2bz * q.q1) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * q.q0 + _2bz * q.q2) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * q.q1 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);

    micro::normalize(s0, s1, s2, s3);

    // Apply feedback step
    qDot1 -= beta * s0;
    qDot2 -= beta * s1;
    qDot3 -= beta * s2;
    qDot4 -= beta * s3;

    const float d_time = static_cast<second_t>(sampleTime - prevSampleTime).get();

    // Integrate rate of change of quaternion to yield quaternion
    q.q0 += qDot1 * d_time;
    q.q1 += qDot2 * d_time;
    q.q2 += qDot3 * d_time;
    q.q3 += qDot4 * d_time;

    micro::normalize(q.q0, q.q1, q.q2, q.q3);

    prevSampleTime = sampleTime;
}

radian_t MadgwickAHRS::roll() const {
    return micro::roll(q);
}

radian_t MadgwickAHRS::pitch() const {
    return micro::pitch(q);
}

radian_t MadgwickAHRS::yaw() const {
    return micro::yaw(q);
}

point3<radian_t> MadgwickAHRS::angles() const {
    return micro::toEuler(q);
}

void MadgwickAHRS::reset(const point3<radian_t>& angles) {
    q = micro::toQuaternion(angles);
}

} // namespace micro
