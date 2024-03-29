#include <micro/utils/types.hpp>

namespace micro {

const char* to_string(const Status& status) {
    const char *result = nullptr;
    switch (status) {
        case Status::OK:            result = "OK";            break;
        case Status::ERROR:         result = "ERROR";         break;
        case Status::BUSY:          result = "BUSY";          break;
        case Status::TIMEOUT:       result = "TIMEOUT";       break;
        case Status::INVALID_ID:    result = "INVALID_ID";    break;
        case Status::INVALID_DATA:  result = "INVALID_DATA";  break;
        case Status::NO_NEW_DATA:   result = "NO_NEW_DATA";   break;
        case Status::BUFFER_FULL:   result = "BUFFER_FULL";   break;
        default:                    result = "unknown error"; break;
    }
    return result;
}

const char* to_string(const Direction& dir) {
    const char *result = nullptr;
    switch (dir) {
        case Direction::LEFT:   result = "LEFT";    break;
        case Direction::CENTER: result = "CENTER";  break;
        case Direction::RIGHT:  result = "RIGHT";   break;
        default:                result = "unknown"; break;
    }
    return result;
}

const char* to_string(const Sign& sign) {
    const char *result = nullptr;
    switch (sign) {
        case Sign::POSITIVE: result = "POSITIVE"; break;
        case Sign::NEUTRAL:  result = "NEUTRAL";  break;
        case Sign::NEGATIVE: result = "NEGATIVE"; break;
        default:             result = "unknown";  break;
    }
    return result;
}

} // namespace micro
