#include <micro/debug/params.hpp>
#include <micro/bsp/task.hpp>
#include <micro/utils/log.hpp>

namespace micro {

void Params::serialize(const char *name, char * const str, uint32_t size) {
    Param *param = this->get(name);

    if (param) {
        if (param->hmutex.ptr != nullptr) {
            while (!isOk(micro::mutexTake(param->hmutex, millisecond_t(1)))) {}
            param->serialize(str, size, param->buf);
            micro::mutexRelease(param->hmutex);
        } else {
            taskSuspendAll();
            param->serialize(str, size, param->buf);
            taskResumeAll();
        }
    } else {
        LOG_ERROR("Invalid Param name [%s]", name);
    }
}
void Params::deserialize(const char *name, const char * const str) {
    Param *param = this->get(name);

    if (param) {
        if (param->hmutex.ptr != nullptr) {
            while (!isOk(micro::mutexTake(param->hmutex, millisecond_t(1)))) {}
            param->deserialize(str, param->buf);
            micro::mutexRelease(param->hmutex);
        } else {
            taskSuspendAll();
            param->deserialize(str, param->buf);
            taskResumeAll();
        }
    } else {
        LOG_ERROR("Invalid Param name [%s]", name);
    }
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
