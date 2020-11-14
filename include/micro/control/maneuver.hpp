#pragma once

#include <micro/utils/CarProps.hpp>
#include <micro/utils/ControlData.hpp>
#include <micro/utils/LinePattern.hpp>

namespace micro {

class Maneuver {
public:
    Maneuver()
        : finished_(false) {}

    virtual ~Maneuver() = default;

    virtual void update(const CarProps& car, const LineInfo& lineInfo, MainLine& mainLine, ControlData& controlData) = 0;

    bool finished() const {
        return this->finished_;
    }

protected:
    void finish() {
        this->finished_ = true;
    }

private:
    bool finished_;
};

} // namespace micro
