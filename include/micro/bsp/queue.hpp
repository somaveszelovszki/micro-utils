#pragma once

#include <micro/utils/units.hpp>

namespace micro {

struct queue_handle_t { void *ptr; }; // Queue handle - underlying instance type is OS library dependent.

/* @brief Puts content of buffer to the end of the queue if it is not full.
 * @param hQueue Pointer to the queue handle.
 * @param txBuffer The buffer storing the data to send.
 * @return Status indicating operation success (if not OK, it means the buffer is full).
 **/
Status queueSend(queue_handle_t hqueue, const void *txBuffer);

/* @brief Gets oldest element from the queue, if any.
 * @param hQueue Pointer to the queue handle.
 * @param rxBuffer The buffer that will store the received data.
 * @return Status indicating operation success (if not OK, it means the buffer is empty).
 **/
Status queueReceive(queue_handle_t hqueue, void *rxBuffer);

} // namespace micro
