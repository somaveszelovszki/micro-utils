#include <micro/utils/Line.hpp>
#include <micro/utils/time.hpp>
#include <micro/math/unit_utils.hpp>
#include <cfg_car.hpp>

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

void LineCalculator::removeUnmatchedLines(LinePositions& positions, bool isFront) {
    Lines& lines = this->currentLines();

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

void LineCalculator::removeUnmatchedPositions(LinePositions& positions, uint32_t targetSize, bool isFront) {
    Lines& lines = this->currentLines();

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

static Lines::const_iterator findClosestLine(const Lines& lines, Lines::const_iterator line) {
    static constexpr millimeter_t MAX_DIST = millimeter_t(15);

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

Line LineCalculator::getMainLine(const Lines& lines, const Line& prevMainLine) {
    Line newMainLine;

    switch(lines.size()) {
    case 1:
        newMainLine = lines[0];
        break;
    case 2:
    {
        const Line& line0 = lines[0];
        const Line& line1 = lines[1];
        const millimeter_t diff0 = micro::abs(lines[0].pos_front - prevMainLine.pos_front) + micro::abs(lines[0].pos_rear - prevMainLine.pos_rear);
        const millimeter_t diff1 = micro::abs(lines[1].pos_front - prevMainLine.pos_front) + micro::abs(lines[1].pos_rear - prevMainLine.pos_rear);

        newMainLine = diff0 < diff1 ? line0 : line1;
        break;
    }
    case 3:
    {
        newMainLine = lines[1];
        break;
    }
    default:
        // should not get here
        break;
    }

    return newMainLine;
}

void LineCalculator::update(LinePositions front, LinePositions rear) {

    static microsecond_t prevRunTime = getExactTime();
    const microsecond_t now = getExactTime();

    Lines newLines;

    if (front.size() > 0 && rear.size() > 0) {
        const uint32_t numLines = std::min(front.size(), rear.size());

        this->removeUnmatchedLines(front, true);
        this->removeUnmatchedLines(rear, false);
        // now [lines.size() = numLines]

        this->removeUnmatchedPositions(front, numLines, true);
        this->removeUnmatchedPositions(rear, numLines, false);
        // now [front.size() = numLines] and [rear.size() = numLines]

        const Lines& prevLines = this->currentLines();

        for (uint32_t i = 0; i < numLines; ++i) {
            Line line;
            line.pos_front = front[i];
            line.pos_rear = rear[i];
            line.angle = micro::atan((line.pos_rear - line.pos_front) / cfg::DIST_BTW_OPTO_ROWS);
            newLines.append(line);
        }

        for (Lines::iterator it = newLines.begin(); it != newLines.end(); ++it) {
            Lines::const_iterator prev = findClosestLine(prevLines, it);
            if (prev != prevLines.end() && findClosestLine(newLines, prev) == it) {
                it->angular_velocity = (it->angle - prev->angle) / (now - prevRunTime);
            }
        }
    }

    this->prevLines.push_back(newLines);
    prevRunTime = now;
}

} // namespace micro
