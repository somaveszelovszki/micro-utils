#pragma once

#include <micro/math/unit_utils.hpp>
#include <micro/utils/timer.hpp>

namespace micro {

/* @brief Base class for sensor data filters.
 * @tparam T Type of the data to filter.
 **/
template <typename T>
class Filter {
public:
    T value() const {
        return this->filteredValue_;
    }

    millisecond_t lastUpdateTimestamp() const {
        return this->lastUpdateTimestamp_;
    }

    virtual T update(const T& measuredValue) = 0;

protected:
    explicit Filter(const T& init)
        : filteredValue_(init) {}

    /* @brief Updates filter.
     * @note Every descendant must call this function at each run.
     **/
    void updateTimestamp() {
        this->lastUpdateTimestamp_ = getTime();
    }

    T filteredValue_;    // The filtered value.

    virtual ~Filter() {}

private:
    millisecond_t lastUpdateTimestamp_;
};

/* @brief Filter implementation for removing bouncing errors.
 * @tparam T Type of the data to filter.
 * @tparam N Number of samples used for validation (we believe a sudden change only if N pieces of data reassure us)
 * e.g. N = 2
 *      dataIn:     0 0 1 0 0 1 1 1 1
 *      filtered:   0 0 0 0 0 0 0 1 1
 **/
template <typename T, uint8_t N>
class BounceFilter : public Filter<T> {

public:
    /* @brief Constructor.
     * @param _complianceRate The compliance rate. A new measurement within the compliance interval of the current measurement is automatically accepted.
     * @param _deadBand The dead-band. A new measurement within the dead-band of the current measurement is automatically accepted.
     **/
    BounceFilter(const T& init, float _complianceRate, const T& _deadBand)
        : Filter<T>(init)
        , complianceRate(_complianceRate)
        , deadBand(_deadBand)
        , idx(0) {}

    /* @brief Updates filter with a new measurement.
     * @param measuredValue The new measurement.
     **/
    T update(const T& measuredValue) override;

private:
    /* @brief Checks if new measurement is within the acceptance interval of the stored measurements.
     * If this function returns true, the new measurement shall be accepted as a valid measurement.
     **/
    bool isInRangeOfRaw(const T& measuredValue) const;

    const float complianceRate; // The compliance rate. A new measurement within the compliance interval of the current measurement is automatically accepted.
    const T deadBand;           // The dead-band. A new measurement within the dead-band of the current measurement is automatically accepted.
    T raw[N];                   // The stored raw measurements.
    uint8_t idx;                // The current measurement index.
};

template <typename T, uint8_t N>
T BounceFilter<T, N>::update(const T& measuredValue) {
    this->updateTimestamp();

    // If measured value is in range of the filtered (output) value, it will be the next output, as it is a valid value.
    // If it is not in the range, it can mean 2 things:
    //      1.) It is a measurement error that needs to be filtered
    //      2.) There has been a sudden change in the environment, and the measurement is valid.
    //
    // In both cases, measurement value will not be saved as output value, only put in the 'raw' array.
    // If a given number of the following samples are in range of this measurement, the sudden change has been validated, and output will be updated.
    if (micro::isBtw(measuredValue, this->filteredValue_ - this->deadBand, this->filteredValue_ + this->deadBand) ||
        micro::isInRange(measuredValue, this->filteredValue_, this->complianceRate)                               ||
        this->isInRangeOfRaw(measuredValue)) {
        this->filteredValue_ = measuredValue;
    }

    this->raw[this->idx] = measuredValue;
    this->idx = (this->idx + 1) % N;
    return this->filteredValue_;
}

template <typename T, uint8_t N>
bool BounceFilter<T, N>::isInRangeOfRaw(const T& measuredValue) const {
    uint8_t i;
    for (i = 0; i < N; ++i) {
        if (!micro::isInRange(measuredValue, this->raw[i], this->complianceRate)) {
            break;
        }
    }
    return i == N;
}

/* @brief Digital low-pass filter implementation. Decreases intensity of sudden changes by calculating the average of the current and past measurements.
 * @tparam T Type of the data to filter.
 * @tparam N Number of samples to calculate average from.
 **/
template <typename T, uint32_t N>
class LowPassFilter : public Filter<T> {
public:
    explicit LowPassFilter(const T& init)
        : Filter<T>(init)
        , idx(0) {

        for (uint32_t i = 0; i < N; ++i) {
            this->raw[i] = init;
        }
    }

    /* @brief Updates filter with a new measurement.
     * @param measuredValue The new measurement.
     **/
    T update(const T& measuredValue) override;

private:
    T raw[N];       // The stored raw measurements.
    uint32_t idx;   // The current measurement index.
};

template <typename T, uint32_t N>
T LowPassFilter<T, N>::update(const T& measuredValue) {
    this->updateTimestamp();

    // To gain performance, the average is not calculated in every iteration, only the diff.
    // This can cause a minor accumulative error. To prevent this, periodically the average is re-calculated.
    if (this->idx == 0) {
        this->raw[this->idx] = measuredValue;

        this->filteredValue_ = this->raw[0];
        for (uint32_t i = 1; i < N; ++i) {
            this->filteredValue_ += this->raw[i];
        }
        this->filteredValue_ /= N;

    } else {
        this->filteredValue_ += (measuredValue - this->raw[this->idx]) / N;
        this->raw[this->idx] = measuredValue;
    }

    this->idx = this->idx == N - 1 ? 0 : this->idx + 1;
    return this->filteredValue_;
}

} // namespace micro
