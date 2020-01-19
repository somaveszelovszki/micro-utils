#pragma once

#include <micro/math/numeric.hpp>
#include <micro/math/unit_utils.hpp>

namespace micro {

/* @brief Base class for sensor data filters.
 * @tparam T Type of the data to filter.
 **/
template <typename T>
class Filter {
protected:
    /* @brief Constructor.
     **/
    Filter() : numCalled(0) {}

    /* @brief Updates filter-called counter.
     * @note Every descendant must call this function at each run.
     **/
    uint32_t updateNumCalled() {
        return ++numCalled;
    }

    virtual T update(const T& measuredValue) = 0;

    T filteredValue;    // The filtered value.

    virtual ~Filter() {}

private:
    uint32_t numCalled; // Counts the times the filter has been called.
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
    BounceFilter(float _complianceRate, const T& _deadBand)
        : complianceRate(_complianceRate)
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
    const T deadBand;               // The dead-band. A new measurement within the dead-band of the current measurement is automatically accepted.
    T raw[N];                       // The stored raw measurements.
    uint8_t idx;                    // The current measurement index. TODO stored measurements in a ringbuffer
};

template <typename T, uint8_t N>
T BounceFilter<T, N>::update(const T& measuredValue) {
    // If measured value is in range of the filtered (output) value, it will be the next output, as it is a valid value.
    // If it is not in the range, it can mean 2 things:
    //      1.) It is a measurement error that needs to be filtered
    //      2.) There has been a sudden change in the environment, and the measurement is valid.
    //
    // In both cases, measurement value will not be saved as output value, only put in the 'raw' array.
    // If a given number of the following samples are in range of this measurement, the sudden change has been validated, and output will be updated.
    if(this->updateNumCalled() > 1 || micro::isInRange(measuredValue, this->filteredValue, this->complianceRate) || this->isInRangeOfRaw(measuredValue)) {
        this->filteredValue = measuredValue;
    }

    this->raw[this->idx] = measuredValue;
    this->idx = (this->idx + 1) % N;
    return this->filteredValue;
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

/* @brief Filter implementation that drops sudden changes.
 * @tparam T Type of the data to filter.
 **/
template <typename T>
class NoJumpFilter : public Filter<T> {

public:
    /* @brief Constructor.
     * @param _complianceRate The compliance rate. A new measurement within the compliance interval of the current measurement is automatically accepted.
     * @param _deadBand The dead-band. A new measurement within the dead-band of the current measurement is automatically accepted.
     **/
    NoJumpFilter(float _complianceRate, const T& _deadBand)
        : complianceRate(_complianceRate)
        , deadBand(_deadBand){}

    /* @brief Updates filter with a new measurement.
     * @param measuredValue The new measurement.
     **/
    T update(const T& measuredValue) override;

private:
    const float complianceRate; // The compliance rate. A new measurement within the compliance interval of the current measurement is automatically accepted.
    const T deadBand;               // The dead-band. A new measurement within the dead-band of the current measurement is automatically accepted.
};

template <typename T>
T NoJumpFilter<T>::update(const T& measuredValue) {
    // If measured value is in range of the filtered (output) value, it will be the next output, as it is a valid value.
    // If it is not in the range, it will not be saved.
    if(this->updateNumCalled() > 1 || micro::eq(measuredValue, this->filteredValue, this->deadBand) || micro::isInRange(measuredValue, this->filteredValue, this->complianceRate)) {
        this->filteredValue = measuredValue;
    }

    return this->filteredValue;
}

/* @brief Digital low-pass filter implementation. Decreases intensity of sudden changes by calculating the average of the current and past measurements.
 * @tparam T Type of the data to filter.
 * @tparam N Number of samples to calculate average from.
 * @tparam cleanUpdatePeriod To gain performance, the average is not calculated in every iteration, only the diff.
 *      This can cause a minor accumulative error. To prevent this, periodically the average is re-calculated.
 **/
template <typename T, uint32_t N, uint32_t cleanUpdatePeriod = N * 100>
class LowPassFilter : public Filter<T> {
public:
    /* @brief Constructor.
     **/
    LowPassFilter()
        : idx(0) {}

    /* @brief Updates filter with a new measurement.
     * @param measuredValue The new measurement.
     **/
    T update(const T& measuredValue) override;

private:
    T raw[N];       // The stored raw measurements.
    uint32_t idx;    // The current measurement index.
};

template <typename T, uint32_t N, uint32_t cleanUpdatePeriod>
T LowPassFilter<T, N, cleanUpdatePeriod>::update(const T& measuredValue) {

    if (this->updateNumCalled() % cleanUpdatePeriod == 0) {
        this->raw[this->idx] = measuredValue;

        this->filteredValue = this->raw[0];
        for (uint32_t i = 1; i < N; ++i) {
            this->filteredValue += this->raw[i];
        }
        this->filteredValue /= N;

    } else {
        this->filteredValue += (measuredValue - this->raw[this->idx]) / N;
        this->raw[this->idx] = measuredValue;
    }

    this->idx = (this->idx + 1) % N;
    return this->filteredValue;
}

} // namespace micro
