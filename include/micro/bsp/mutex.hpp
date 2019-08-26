#pragma once

#include <micro/utils/units.hpp>

namespace micro {

struct mutex_handle_t { void *ptr; }; // Mutex handle - underlying instance type is OS library dependent.

/* @brief Waits for mutex to become available and reserves it. Polls while blocking.
 * @param hmutex Pointer to the mutex handle.
 * @param The timeout to wait for the mutex.
 * @return Status indicating operation success (if not OK, it means the mutex is not available).
 **/
Status mutexTake(mutex_handle_t hmutex, millisecond_t timeout);

/* @brief Checks if mutex is available and reserves it. Does not block.
 * @note This function must be called from an interrupt service routine!
 * @param hmutex Pointer to the mutex handle.
 * @return Status indicating operation success (if not OK, it means the mutex is not available).
 **/
Status mutexTake_ISR(mutex_handle_t hmutex);

/* @brief Releases mutex.
 * @param hmutex Pointer to the mutex handle.
 * @return Status indicating operation success.
 **/
Status mutexRelease(mutex_handle_t hmutex);

/* @brief Releases mutex.
 * @note This function must be called from an interrupt service routine!
 * @param hmutex Pointer to the mutex handle.
 * @return Status indicating operation success.
 **/
Status mutexRelease_ISR(mutex_handle_t hmutex);

} // namespace micro
