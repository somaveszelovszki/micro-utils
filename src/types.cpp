#include <micro/utils/types.hpp>
#include <micro/utils/log.hpp>

namespace micro {

const char* getStatusString(Status status) {

    static const char * const STR_OK            = "OK";
    static const char * const STR_ERROR         = "ERROR";
    static const char * const STR_BUSY          = "BUSY";
    static const char * const STR_TIMEOUT       = "TIMEOUT";
    static const char * const STR_INVALID_ID    = "INVALID_ID";
    static const char * const STR_INVALID_DATA  = "INVALID_DATA";
    static const char * const STR_NO_NEW_DATA   = "NO_NEW_DATA";
    static const char * const STR_BUFFER_FULL   = "BUFFER_FULL";
    static const char * const STR_unknown       = "unknown error";

    const char *result = STR_unknown;
    switch (status) {
        case Status::OK:            result = STR_OK;            break;
        case Status::ERROR:         result = STR_ERROR;         break;
        case Status::BUSY:          result = STR_BUSY;          break;
        case Status::TIMEOUT:       result = STR_TIMEOUT;       break;
        case Status::INVALID_ID:    result = STR_INVALID_ID;    break;
        case Status::INVALID_DATA:  result = STR_INVALID_DATA;  break;
        case Status::NO_NEW_DATA:   result = STR_NO_NEW_DATA;   break;
        case Status::BUFFER_FULL:   result = STR_BUFFER_FULL;   break;
        default:                    result = STR_unknown;       break;
    }
    return result;
}

const char* getLogLevelString(const logLevel_t level) {

    static const char * const STR_Debug   = "[D]";
    static const char * const STR_Info    = "[I]";
    static const char * const STR_Warning = "[W]";
    static const char * const STR_Error   = "[E]";
    static const char * const STR_unknown = "[unknown]";

    const char *result = STR_unknown;
    switch (level) {
        case LogLevel_Debug:   result = STR_Debug;   break;
        case LogLevel_Info:    result = STR_Info;    break;
        case LogLevel_Warning: result = STR_Warning; break;
        case LogLevel_Error:   result = STR_Error;   break;
        default:               result = STR_unknown; break;
    }
    return result;
}

} // namespace micro

extern "C" void onHardFault() {
    while(1) {}
}
