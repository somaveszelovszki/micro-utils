#pragma once

#include "units.hpp"

namespace micro {

/* @brief Gets time since system startup.
 * @returns Time since system startup.
 **/
millisecond_t getTime();

/* @brief Gets exact time since system startup.
 * @returns Time since system startup.
 **/
microsecond_t getExactTime();

} // namespace micro
