#include <micro/utils/Line.hpp>
#include <micro/utils/time.hpp>
#include <micro/math/unit_utils.hpp>

#include <algorithm>

namespace micro {

Lines::const_iterator findClosestLine(const Lines& lines, millimeter_t pos) {
    millimeter_t minDist = millimeter_t::infinity();
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

bool areClose(const Lines& lines) {

    constexpr millimeter_t MAX_CLOSE_LINES_DISTANCE = centimeter_t(5.2f);

    bool close = true;
    for (uint32_t i = 1; i < lines.size(); ++i) {
        if (lines[i].pos - lines[i - 1].pos > MAX_CLOSE_LINES_DISTANCE) {
            close = false;
            break;
        }
    }
    return close;
}

bool areFar(const Lines& lines) {
    return !areClose(lines);
}

} // namespace micro
