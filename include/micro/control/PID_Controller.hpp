#pragma once

#include <micro/math/unit_utils.hpp>

namespace micro {

struct PID_Params {
    float P = 0.0f;
    float I = 0.0f;
    float D = 0.0f;

    bool operator==(const PID_Params& other) const { return P == other.P && I == other.I && D == other.D; }
    bool operator!=(const PID_Params& other) const { return !(*this == other); }
};

inline PID_Params operator+(const PID_Params& a, const PID_Params& b) { return { a.P + b.P, a.I + b.I, a.D + b.D }; }
inline PID_Params operator-(const PID_Params& a, const PID_Params& b) { return { a.P - b.P, a.I - b.I, a.D - b.D }; }

template <typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
inline PID_Params operator*(const PID_Params& p, const T& c) { return { p.P * c, p.I * c, p.D * c }; }

template <typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
inline PID_Params operator*(const T& c, const PID_Params& p) { return p * c; }

class PID_Controller {
public:
    PID_Controller(const PID_Params& params, const float outMax, const float deadband);

    void tune(const PID_Params& params);

    float output() const {
        return this->output_;
    }

    void update(const float measured);

    float desired;

private:
    PID_Params params_;
    const float outMax_;
    const float deadband_;
    float prevErr_;
    float integral_;
    float output_;
    millisecond_t prevUpdateTime_;
};
} // namespace micro
