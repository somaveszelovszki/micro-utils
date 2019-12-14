#include <micro/utils/Line.hpp>
#include <micro/utils/time.hpp>
#include <micro/math/unit_utils.hpp>
#include <cfg_car.hpp>

#include <algorithm>

namespace micro {

Lines::const_iterator LineCalculator::getClosestLine(const Lines& lines, millimeter_t pos, bool isFront, millimeter_t& dist) {
    dist = millimeter_t::infinity();
    Lines::const_iterator lineIter = lines.end();

    for (Lines::const_iterator it = lines.begin(); it != lines.end(); ++it) {
        const millimeter_t d = micro::abs((isFront ? it->pos_front : it->pos_rear) - pos);
        if (d < dist) {
            dist = d;
            lineIter = it;
        }
    }

    return lineIter;
}

Lines::const_iterator LineCalculator::findClosestLine(const Lines& lines, Lines::const_iterator line) {
    Lines::const_iterator closest = lines.end();
    millimeter_t minSumDist = millimeter_t::infinity();

    for (Lines::const_iterator it = lines.begin(); it != lines.end(); ++it) {

        const millimeter_t frontDist = abs(line->pos_front - it->pos_front);
        const millimeter_t rearDist = abs(line->pos_rear - it->pos_rear);

        const millimeter_t sumDist = frontDist + rearDist;
        if (sumDist < minSumDist) {
            minSumDist = sumDist;
            closest = it;
        }
    }

    return closest;
}

Lines::const_iterator LineCalculator::findLine(const Lines& lines, const uint32_t id) {
    return std::find_if(lines.begin(), lines.end(), [id](const Line& l) { return l.id == id; });
}

void LineCalculator::removeUnmatchedLines(Lines& lines, LinePositions& positions, bool isFront) {
    if (positions.size() < lines.size()) {
        Lines prevLines = lines;
        lines.clear();

        for (const millimeter_t pos : positions) {
            millimeter_t minDist = millimeter_t::infinity();
            Lines::const_iterator closestLine = getClosestLine(prevLines, pos, isFront, minDist);
            if (closestLine != prevLines.end()) {
                lines.append(*closestLine);
                prevLines.erase(closestLine);
            }
        }
    }
}

void LineCalculator::removeUnmatchedPositions(const Lines& lines, LinePositions& positions, uint32_t targetSize, bool isFront) {
    while (positions.size() > targetSize) {

        // Iterates through all the current line positions, and for each of them,
        // finds the distance to the closest line position in the previous measurement.
        // For the detected lines that were already calculated in the previous measurement,
        // this distance will be lower than for the positions that are new lines.
        // The second type of positions need to be filtered out, therefore the algorithm
        // finds the position with the largest minimum distance from the previous line positions.

        LinePositions::iterator maxIter = positions.end();
        millimeter_t largestMinDist(0);
        for (LinePositions::iterator linePosIter = positions.begin(); linePosIter != positions.end(); ++linePosIter) {

            // finds minimum distance from the previous line positions
            millimeter_t minDist = millimeter_t::infinity();
            getClosestLine(lines, *linePosIter, isFront, minDist);

            // stores largest minimum distance
            if (minDist > largestMinDist) {
                largestMinDist = minDist;
                maxIter = linePosIter;
            }
        }

        positions.erase(maxIter);
    }
}

void LineCalculator::updateMainLine(const Lines& lines, Line& mainLine) {

    switch(lines.size()) {
    case 0:
        mainLine.angular_velocity = rad_per_sec_t(0);
        break;
    case 1:
        mainLine = lines[0];
        break;
    case 2:
    {
        const Line& line0 = lines[0];
        const Line& line1 = lines[1];
        const millimeter_t diff0 = micro::abs(lines[0].pos_front - mainLine.pos_front) + micro::abs(lines[0].pos_rear - mainLine.pos_rear);
        const millimeter_t diff1 = micro::abs(lines[1].pos_front - mainLine.pos_front) + micro::abs(lines[1].pos_rear - mainLine.pos_rear);

        mainLine = diff0 < diff1 ? line0 : line1;
        break;
    }
    case 3:
    {
        mainLine = lines[1];
        break;
    }
    default:
        // should not get here
        break;
    }
}

void LineCalculator::update(LinePositions front) {

    const microsecond_t now = getExactTime();
    Lines lines;

    if (!front.size()) {
        lines.clear();
        // main line is not changed
    } else {
        lines.clear();

        for (uint32_t i = 0; i < front.size(); ++i) {
            Line line;
            line.pos_front = front[i];
            lines.append(line);
        }
    }

    this->prevLines.push_back({ lines, now });
}

void LineCalculator::update(LinePositions front, LinePositions rear) {

    const microsecond_t now = getExactTime();
    Lines lines;

    if (front.size() > 0 && rear.size() > 0) {
        const uint32_t numLines = std::min(front.size(), rear.size());

        Lines prevLines = this->currentLines().lines;

        this->removeUnmatchedLines(prevLines, front, true);
        this->removeUnmatchedLines(prevLines, rear, false);
        // now [prevLines.size() = numLines]

        removeUnmatchedPositions(prevLines, front, numLines, true);
        removeUnmatchedPositions(prevLines, rear, numLines, false);
        // now [front.size() = numLines] and [rear.size() = numLines]

        for (uint32_t i = 0; i < numLines; ++i) {
            Line line;
            line.pos_front = front[i];
            line.pos_rear = rear[i];
            line.angle = micro::atan((line.pos_rear - line.pos_front) / cfg::DIST_BTW_OPTO_ROWS);
            lines.append(line);
        }

        // Executes line tracking:
        // Finds lines in the previous measurement.
        // If they are found, they inherit the previous line id.
        // If they are not found, the are given a new unique id.
        // Calculates angular velocity based on this tracking, with filtering.

        const StampedLines& oldLines = this->prevLines.peek_back(10);
        const microsecond_t angVelTimeDiff = now - oldLines.time;

        for (Lines::iterator it = lines.begin(); it != lines.end(); ++it) {
            Lines::const_iterator prev = findClosestLine(prevLines, it);
            if (prev != prevLines.end() && findClosestLine(lines, prev) == it) {
                it->id = prev->id;

                Lines::const_iterator oldLine = findLine(oldLines.lines, it->id);
                if (oldLine != oldLines.lines.end()) {
                    it->angular_velocity = (it->angle - oldLine->angle) / angVelTimeDiff;
                } else {
                    it->angular_velocity = rad_per_sec_t(0);
                }
            } else {
                it->id = ++this->currentMaxLineId;
                it->angular_velocity = rad_per_sec_t(0);
            }
        }
    }

    this->prevLines.push_back({ lines, now });
}

} // namespace micro
