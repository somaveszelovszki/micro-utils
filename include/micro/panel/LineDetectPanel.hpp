#pragma once

#include <micro/bsp/uart.hpp>
#include <micro/utils/Line.hpp>

namespace micro {

class LineDetectPanel {
public:
    LineDetectPanel(uart_handle_t *_huart);

    Status start();

    void getLinePositions(LinePositions& result) const;

private:
    uart_handle_t *huart;
    uint8_t linesBuffer[1 + cfg::MAX_NUM_LINES];
};

}  // namespace micro
