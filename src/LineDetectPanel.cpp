#include <micro/panel/LineDetectPanel.hpp>
#include <micro/panel/handshake.hpp>

namespace micro {

LineDetectPanel::LineDetectPanel(uart_handle_t* _huart)
    : huart(_huart) {}

Status LineDetectPanel::start() {
    static constexpr uint8_t startChars[2] = { 'S', '\n' };

    Status status = handshake(this->huart, startChars, 2);
    if (isOk(status)) {
        UART_Receive_DMA(this->huart, this->linesBuffer, 1 + cfg::MAX_NUM_LINES);
    }

    return status;
}

void LineDetectPanel::getLinePositions(LinePositions& result) const {
    result.clear();
    for (uint8_t i = 0; i < this->linesBuffer[0]; ++i) {
        result.append(millimeter_t(this->linesBuffer[1 + i]));
    }
}

} // namespace micro
