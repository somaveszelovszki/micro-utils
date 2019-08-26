#pragma once

#include <micro/utils/types.hpp>

namespace micro {

struct adc_handle_t { void *ptr; };       // ADC handle type - underlying instance type is bsp library dependent.
struct adc_channel_t { uint16_t value; }; // ADC channel identifier type.

/* @brief Sets ADC channel.
 * @param hadc Pointer to the ADC handle.
 * @param channel The channel to set.
 * @returns Status indicating operation success.
 **/
Status ADC_SetChannel(adc_handle_t hadc, adc_channel_t channel);

/* @brief Starts ADC and reads value.
 * @param hadc Pointer to the ADC handle.
 * @param pResult Pointer to the result variable.
 * @returns Status indicating operation success.
 **/
Status ADC_ReadValue(adc_handle_t hadc, uint32_t *pResult);

} // namespace micro
