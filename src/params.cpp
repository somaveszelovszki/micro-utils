#include <micro/debug/params.hpp>
#include <micro/bsp/task.hpp>
#include <micro/utils/log.hpp>

namespace micro {

void Params::updateParam(const char *name, const uint8_t *buf, uint8_t size) {

    Param *param = nullptr;
    for (Param& p : this->values) {
        if (strcmp(p.name, name) == 0) {
            param = &p;
            break;
        }
    }

    if (param) {
        if (param->size == size) {

            if (param->hmutex.ptr != nullptr) {
                while (!isOk(micro::mutexTake(param->hmutex, millisecond_t(1)))) {}
                memcpy(param->buf, buf, size);
                micro::mutexRelease(param->hmutex);
            } else {
                taskSuspendAll();
                memcpy(param->buf, buf, size);
                taskResumeAll();
            }

        } else {
            LOG_ERROR("Invalid Param buffer size [%d] for [%s]", static_cast<int32_t>(size), name);
        }
    } else {
        LOG_ERROR("Invalid Param name [%s]", name);
    }
}

} // namespace micro
