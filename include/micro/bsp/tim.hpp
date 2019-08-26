#pragma once

#include <micro/utils/units.hpp>

namespace micro {

struct tim_handle_t { void *ptr; };       // Timer handle type - underlying instance type is bsp library dependent.
struct tim_channel_t { uint32_t value; }; // Timer channel identifier type.

/* @brief Gets time since system startup.
 * @returns Time since system startup.
 **/
millisecond_t getTime();

/* @brief Gets exact time since system startup.
 * @returns Exact time since system startup.
 **/
microsecond_t getExactTime();

/* @brief Delays given amount of time. Does not block processor.
 * @param delay The amount of time to delay.
 **/
void nonBlockingDelay(millisecond_t delay);

/* @brief Delays given amount of time. Blocks processor.
 * @param delay The amount of time to delay.
 **/
void blockingDelay(millisecond_t delay);

/* @brief Sets PWM absolute duty cycle.
 * @param htim Pointer to the timer handle.
 * @param channel The timer's channel.
 * @param duty The duty cycle.
 * @return Status indicating operation success.
 **/
Status writePWM(tim_handle_t htim, tim_channel_t channel, uint32_t duty);

/* @brief Gets counter value.
 * @param htim Pointer to the timer handle.
 * @returns The timer counter's value.
 **/
uint32_t getTimerCounter(tim_handle_t htim);

/* @brief Sets counter value.
 * @param htim Pointer to the timer handle.
 * @param cntr The timer counter's value.
 **/
void setTimerCounter(tim_handle_t htim, uint32_t cntr);

/* @brief Gets compare value.
 * @param htim Pointer to the timer handle.
 * @param channel The timer's channel.
 **/
uint32_t getTimerCompare(tim_handle_t htim, tim_channel_t channel);

} // namespace micro
