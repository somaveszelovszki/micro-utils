#include <micro/debug/params.hpp>
#include <micro/bsp/task.hpp>
#include <micro/utils/log.hpp>

namespace micro {

void Params::updateParam(uint8_t id, const uint8_t *buf, uint8_t size) {
    Param *param = this->values.get(id);
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
            LOG_ERROR("Invalid Param buffer size [%d] for id [%d]", static_cast<int32_t>(size), static_cast<int32_t>(id));
        }
    } else {
        LOG_ERROR("Invalid Param id [%d]", static_cast<int32_t>(id));
    }
}

} // namespace micro
