#include <algorithm>

#include <micro/math/unit_utils.hpp>
#include <micro/utils/Line.hpp>
#include <micro/utils/timer.hpp>

namespace micro {

Lines::const_iterator findClosestLine(const Lines& lines, millimeter_t pos) {
    millimeter_t minDist          = micro::numeric_limits<millimeter_t>::infinity();
    Lines::const_iterator closest = lines.end();

    for (Lines::const_iterator it = lines.begin(); it != lines.end(); ++it) {
        const millimeter_t d = micro::abs(it->pos - pos);
        if (d < minDist) {
            minDist = d;
            closest = it;
        }
    }

    return closest;
}

Lines::const_iterator findLine(const Lines& lines, const uint32_t id) {
    return std::find_if(lines.begin(), lines.end(), [id](const Line& l) { return l.id == id; });
}

MainLine::MainLine(const meter_t carFrontRearSensorRowDist)
    : carFrontRearSensorRowDist(carFrontRearSensorRowDist) {
}

void MainLine::updateFrontRearLines() {
    const meter_t diff     = (this->carFrontRearSensorRowDist / 2) * tan(this->centerLine.angle);
    const meter_t frontPos = centerLine.pos - diff;
    const meter_t rearPos  = centerLine.pos + diff;

    this->frontLine.id  = 0;
    this->frontLine.pos = frontPos;
    this->rearLine.id   = 0;
    this->rearLine.pos  = -rearPos;
}

void MainLine::updateCenterLine() {
    // gets front and rear positions in the car's coordinate system
    const meter_t frontPos = this->frontLine.pos;
    const meter_t rearPos  = -this->rearLine.pos;

    this->centerLine.pos   = avg(frontPos, rearPos);
    this->centerLine.angle = atan2(-(frontPos - rearPos), this->carFrontRearSensorRowDist);
}

void updateMainLine(const Lines& lines, Line& mainLine) {
    Lines::const_iterator l = findLine(lines, mainLine.id);
    if (l != lines.end()) {
        mainLine = *l;
    } else {
        l = findClosestLine(lines, mainLine.pos);
        if (l != lines.end()) {
            mainLine = *l;
        }
    }
}

void updateMainLine(const Lines& frontLines, const Lines& rearLines, MainLine& mainLine) {
    updateMainLine(frontLines, mainLine.frontLine);
    updateMainLine(rearLines, mainLine.rearLine);
    mainLine.updateCenterLine();
}

bool areClose(const Lines& lines) {
    if (lines.empty()) {
        return true;
    }

    millimeter_t prevPos = lines.begin()->pos;
    return std::all_of(std::next(lines.begin()), lines.end(), [&prevPos](const auto& l) {
        constexpr millimeter_t MAX_CLOSE_LINES_DISTANCE = centimeter_t(5.3f);
        return l.pos - std::exchange(prevPos, l.pos) <= MAX_CLOSE_LINES_DISTANCE;
    });
}

bool areFar(const Lines& lines) {
    return lines.empty() || !areClose(lines);
}

} // namespace micro
