#include <micro/utils/Line.hpp>
#include <micro/math/unit_utils.hpp>
#include <cfg_car.hpp>

namespace micro {

static void removeUnmatchedPositions(LinePositions& positions, const Lines& lines, uint32_t targetSize, bool isFront) {
    while (positions.size() > targetSize) {

        // Iterates through all the current line positions, and for each of them,
        // finds the distance to the closest line position in the previous measurement.
        // For the detected lines that were already calculated in the previous measurement,
        // this distance will be lower than for the positions that are new lines.
        // The second type of positions need to be filtered out, therefore the algorithm
        // finds the position with the largest minimum distance from the previous line positions.

        LinePositions::iterator maxIter = positions.end();
        millimeter_t maxDist(0);
        for (LinePositions::iterator linePosIter = positions.begin(); linePosIter != positions.end(); ++linePosIter) {

            // finds minimum distance from the previous line positions
            millimeter_t currentMinDist(std::numeric_limits<double>::infinity());
            for (Lines::const_iterator lineIter = lines.begin(); lineIter != lines.end(); ++lineIter) {
                const millimeter_t dist = micro::abs((isFront ? lineIter->pos_front : lineIter->pos_rear) - *linePosIter);
                if (dist < currentMinDist) {
                    currentMinDist = dist;
                }
            }

            // stores largest minimum distance
            if (currentMinDist > maxDist) {
                maxDist = currentMinDist;
                maxIter = linePosIter;
            }
        }

        positions.erase(maxIter);
    }
}

static void updateMainLine(const Lines& lines, Line& mainLine) {
    switch(lines.size()) {
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

void calculateLines(LinePositions front, Lines& lines, Line& mainLine) {

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

        updateMainLine(lines, mainLine);
    }
}

void calculateLines(LinePositions front, LinePositions rear, Lines& lines, Line& mainLine) {

    if (!front.size() || !rear.size()) {
        lines.clear();
        // main line is not changed
    } else {
        removeUnmatchedPositions(front, lines, rear.size(), true);
        removeUnmatchedPositions(rear, lines, front.size(), false);

        // now front and rear sizes are equal

        lines.clear();

        for (uint32_t i = 0; i < front.size(); ++i) {
            Line line;
            line.pos_front = front[i];
            line.pos_rear = rear[i];

            if (micro::eq(line.pos_front, line.pos_rear, millimeter_t(1.0f))) {
                line.angle = radian_t::zero();
            } else {
                line.angle = micro::atan(cfg::DIST_BTW_OPTO_ROWS / (line.pos_front - line.pos_rear));
                if (line.angle < radian_t::zero()) {
                    line.angle += micro::PI_2;
                } else {
                    line.angle -= micro::PI_2;
                }
            }
            lines.append(line);
        }

        updateMainLine(lines, mainLine);
    }
}

} // namespace micro
