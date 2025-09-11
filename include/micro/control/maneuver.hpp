#pragma once

#include <micro/utils/CarProps.hpp>
#include <micro/utils/ControlData.hpp>
#include <micro/utils/LinePattern.hpp>

namespace micro {

class Maneuver {
  public:
    virtual ~Maneuver() = default;

    virtual void update(const CarProps& car, const LineInfo& lineInfo, MainLine& mainLine,
                        ControlData& controlData) = 0;

    bool finished() const { return this->finished_; }

  protected:
    Maneuver() : finished_(false) {}

    void initialize() { this->finished_ = false; }

    void finish() { this->finished_ = true; }

  private:
    bool finished_;
};

} // namespace micro
