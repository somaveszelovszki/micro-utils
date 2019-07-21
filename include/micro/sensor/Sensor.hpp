#pragma once

#include <micro/utils/runnable.hpp>

namespace micro {

template <typename M>
class Sensor : public runnable_t {

public:
    explicit Sensor(millisecond_t _period) : runnable_t(_period) {}

    /* @brief Gets measured value.
     * @returns The measured value.
     **/
    const M& getMeasured() const {
        return this->meas;
    }

protected:
    M meas; // The measured value.
};

} // namespace micro
