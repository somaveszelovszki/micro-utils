#include <micro/panel/MotorPanel.hpp>
#include <micro/panel/handshake.hpp>
#include <micro/utils/convert.hpp>

namespace micro {

MotorPanel::MotorPanel(uart_handle_t* _huart)
    : huart(_huart) {}

Status MotorPanel::start(bool useSafetyEnableSignal) {
    static constexpr uint8_t startChars[2] = { 'R', '\n' };
    static constexpr uint8_t startWithSafetyEnableSignalChars[2] = { 'S', '\n' };

    Status status = handshake(this->huart, useSafetyEnableSignal ? startWithSafetyEnableSignalChars : startChars, 2);
    if (isOk(status)) {
        UART_Receive_DMA(this->huart, this->speedBuffer, 4);
    }

    return status;
}

m_per_sec_t MotorPanel::getSpeed() const {
    return m_per_sec_t(toFloat32(this->speedBuffer));
}

} // namespace micro
