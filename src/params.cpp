#include <micro/debug/params.hpp>
#include <micro/utils/log.hpp>
#include <micro/utils/numeric.hpp>
#include <micro/utils/str_utils.hpp>

#include <FreeRTOS.h>
#include <cmsis_os.h>
#include <semphr.h>

namespace micro {

uint32_t Params::serializeAll(char * const str, uint32_t size) {
    uint32_t idx = 0;
    str[idx++] = '{';

    for (uint32_t i = 0; i < this->values.size(); ++i) {
        Param& p = this->values[i];

        str[idx++] = '"';

        strncpy(&str[idx], p.name, size - idx);
        idx += strlen(p.name);

        str[idx++] = '"';
        str[idx++] = ':';

        if (p.hmutex != nullptr) {
            while (!xSemaphoreTake(p.hmutex, 1)) {}
            idx += p.serialize(&str[idx], size - idx, p.buf);
            xSemaphoreGive(p.hmutex);
        } else {
            vTaskSuspendAll();
            idx += p.serialize(&str[idx], size - idx, p.buf);
            xTaskResumeAll();
        }
        if (idx >= size) break;

        if (i < this->values.size() - 1) {
            str[idx++] = ',';
            if (idx >= size) break;
        }

        str[idx++] = '\r';
        str[idx++] = '\n';
    }

    str[idx++] = '}';

    return idx;
}

uint32_t Params::deserializeAll(const char * const str) {
    const uint32_t size = strlen(str);

    uint32_t idx = 0;
    idx++; // '{'

    char name[STR_MAX_LEN_PARAM_NAME];

    while (idx < size) {
        idx++; // '"'
        idx += strcpy_until(name, &str[idx], min(static_cast<uint32_t>(STR_MAX_LEN_PARAM_NAME), size - idx), '"');
        idx++; // '"'
        idx++; // ':'

        Param * const p = this->get(name);
        if (!p) {
            LOG_ERROR("Unknown parameter name: '%s'.", name);
            break;
        }

        if (p->hmutex != nullptr) {
            while (!xSemaphoreTake(p->hmutex, 1)) {}
            idx += p->deserialize(&str[idx], p->buf);
            xSemaphoreGive(p->hmutex);
        } else {
            vTaskSuspendAll();
            idx += p->deserialize(&str[idx], p->buf);
            xTaskResumeAll();
        }

        if ('}' == str[idx]) {
            break;
        } else {
            idx++; // ','
        }
    }

    for (uint32_t i = 0; i < this->values.size(); ++i) {

    }

    return idx;
}

Param* Params::get(const char *name) {
    Param *param = nullptr;
    for (Param& p : this->values) {
        if (strcmp(p.name, name) == 0) {
            param = &p;
            break;
        }
    }
    return param;
}

} // namespace micro
