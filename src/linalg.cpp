#include <micro/math/linalg.hpp>
#include <micro/math/unit_utils.hpp>

namespace micro {

radian_t roll(const quaternion_t& q) {
    const float sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
    const float cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    return std::atan2(sinr_cosp, cosr_cosp);
}

radian_t pitch(const quaternion_t& q) {
    const float sinp = 2 * (q.w * q.y - q.z * q.x);
    return abs(sinp) >= 1 ? sgn(sinp) * PI_2 : std::asin(sinp);
}

radian_t yaw(const quaternion_t& q) {
    const float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
    const float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
    return atan2(siny_cosp, cosy_cosp);

    //return normalize360(atan2(2 * (q.w * q.z + q.x * q.y), q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z));
}

point3<radian_t> toEuler(const quaternion_t& q) {
    return { roll(q), pitch(q), yaw(q) };
}

quaternion_t toQuaternion(const point3<radian_t>& euler)
{
    const float cy = cos(euler.Z * 0.5f);
    const float sy = sin(euler.Z * 0.5f);
    const float cp = cos(euler.Y * 0.5f);
    const float sp = sin(euler.Y * 0.5f);
    const float cr = cos(euler.Z * 0.5f);
    const float sr = sin(euler.Z * 0.5f);

    return {
        cr * cp * cy + sr * sp * sy,
        sr * cp * cy - cr * sp * sy,
        cr * sp * cy + sr * cp * sy,
        cr * cp * sy - sr * sp * cy
    };
}

} // namespace micro
