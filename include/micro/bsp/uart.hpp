#pragma once

#include <micro/utils/units.hpp>

namespace micro {

struct uart_handle_t { void *ptr; }; // UART handle - underlying instance type is bsp library dependent.

/* @brief Receives data through UART. Receives in blocking mode.
 * @param huart Pointer to the UART handle.
 * @param rxBuffer The buffer that will store the received data.
 * @param size Number of bytes to receive.
 * @param timeout The maximum timeout to wait for UART to be free to use.
 * @return Status indicating operation success.
 **/
Status UART_Receive(uart_handle_t huart, uint8_t *rxBuffer, uint32_t size, millisecond_t timeout);

/* @brief Receives data through UART. Receives in non-blocking mode.
 * @param huart Pointer to the UART handle.
 * @param rxBuffer The buffer that will store the received data.
 * @param size Number of bytes to receive.
 * @return Status indicating operation success.
 **/
Status UART_Receive_IT(uart_handle_t huart, uint8_t *rxBuffer, uint32_t size);

/* @brief Receives data through UART. Receives in non-blocking mode.
 * @param huart Pointer to the UART handle.
 * @param rxBuffer The buffer that will store the received data.
 * @param size Number of bytes to receive.
 * @return Status indicating operation success.
 **/
Status UART_Receive_DMA(uart_handle_t huart, uint8_t *rxBuffer, uint32_t size);

/* @brief Transmits data through UART. Transfers in blocking mode.
 * @param huart Pointer to the UART handle.
 * @param txBuffer The buffer storing the data to send.
 * @param size Number of bytes to send.
 * @param timeout The maximum timeout to wait for UART to be free to use.
 * @return Status indicating operation success.
 **/
Status UART_Transmit(uart_handle_t huart, const uint8_t *txBuffer, uint32_t size, millisecond_t timeout);

/* @brief Transmits data through UART. Transfers in non-blocking mode.
 * @param huart Pointer to the UART handle.
 * @param txBuffer The buffer storing the data to send.
 * @param size Number of bytes to send.
 * @return Status indicating operation success.
 **/
Status UART_Transmit_IT(uart_handle_t huart, const uint8_t *txBuffer, uint32_t size);

/* @brief Transmits data through UART. Transfers in non-blocking mode.
 * @param huart Pointer to the UART handle.
 * @param txBuffer The buffer storing the data to send.
 * @param size Number of bytes to send.
 * @return Status indicating operation success.
 **/
Status UART_Transmit_DMA(uart_handle_t huart, const uint8_t *txBuffer, uint32_t size);

/* @brief Stops UART data transfer
 * @param huart Pointer to the UART handle.
 * @return Status indicating operation success.
 **/
Status UART_Stop_DMA(uart_handle_t huart);
} // namespace micro
