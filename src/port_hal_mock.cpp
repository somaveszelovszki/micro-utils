#if !defined STM32

#include <micro/port/can.hpp>
#include <micro/port/gpio.hpp>
#include <micro/port/i2c.hpp>
#include <micro/port/spi.hpp>
#include <micro/port/timer.hpp>
#include <micro/port/uart.hpp>

namespace micro {

// CAN

Status can_transmit(const can_t&, const canFrame_t&) { return Status::OK; }
Status can_receive(const can_t&, canFrame_t& OUT) { return Status::OK; }

// GPIO

Status gpio_read(const gpio_t&, gpioPinState_t& OUT) { return Status::OK; }
Status gpio_write(const gpio_t&, const gpioPinState_t) { return Status::OK; }
Status gpio_toggle(const gpio_t&) { return Status::OK; }

// I2C

Status i2c_masterReceive(const i2c_t&, const uint16_t, uint8_t * const, const uint32_t) { return Status::OK; }
Status i2c_masterTransmit(const i2c_t&, const uint16_t, const uint8_t * const, const uint32_t) { return Status::OK; }
Status i2c_memoryRead(const i2c_t&, const uint16_t, const uint16_t, const uint8_t, uint8_t * const, const uint32_t) { return Status::OK; }
Status i2c_memoryWrite(const i2c_t&, const uint16_t, const uint16_t, const uint8_t, const uint8_t * const, const uint32_t) { return Status::OK; }

// SPI

Status spi_exchange(const spi_t&, const uint8_t * const, uint8_t * const, const uint32_t) { return Status::OK; }

// TIMER

void time_init(const timer_t&) {}

millisecond_t getTime() {
    return millisecond_t(0);
}

microsecond_t getExactTime() {
    return microsecond_t(0);
}

Status timer_getCounter(const timer_t&, uint32_t& OUT) { return Status::OK; }
Status timer_setCounter(const timer_t&, const uint32_t) { return Status::OK; }
Status timer_getCompare(const timer_t&, const uint32_t, uint32_t& OUT) { return Status::OK; }
Status timer_setCompare(const timer_t&, const uint32_t, const uint32_t) { return Status::OK; }
Status timer_setDuty(const timer_t&, const uint32_t, const float) { return Status::OK; }

// UART

Status uart_receive(const uart_t&, uint8_t * const, const uint32_t) { return Status::OK; }
Status uart_transmit(const uart_t&, const uint8_t * const, const uint32_t) { return Status::OK; }
Status uart_stopReceive(const uart_t&) { return Status::OK; }
Status uart_stopTransmit(const uart_t&) { return Status::OK; }

} // namespace micro

#endif // !STM32
