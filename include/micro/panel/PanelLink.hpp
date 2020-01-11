#pragma once

#include "panelLink.h"
#include <micro/utils/time.hpp>
#include <micro/math/unit_utils.hpp>

#include <FreeRTOS.h>
#include <task.h>

namespace micro {

template <typename T_rx, typename T_tx, panelLinkRole_t role>
class PanelLink {
public:
    PanelLink(UART_HandleTypeDef *huart, millisecond_t rxPeriod, millisecond_t txPeriod) {
        panelLink_initialize(this->link(), role, huart,
            &this->rxDataBuffer_, sizeof(T_rx), static_cast<uint32_t>(rxPeriod.get()),
            &this->txDataBuffer_, sizeof(T_tx), static_cast<uint32_t>(txPeriod.get()));
    }

    bool isConnected() const {
        return panelLink_isConnected(this->link());
    }

    bool shouldSend() const {
        return panelLink_shouldSend(this->link());
    }

    void send(const T_tx& txData) {
        panelLink_send(this->link(), &txData);
    }

    void onNewRxData() {
        panelLink_onNewRxData(this->link());
    }

    bool readAvailable(T_rx& rxData) {
        return panelLink_readAvailable(this->link(), &rxData);
    }

    void update() {
        panelLink_update(this->link());
    }

private:
    const panelLink_t* link() const { return const_cast<const panelLink_t*>(&this->link_); }
    panelLink_t* link() { return const_cast<panelLink_t*>(&this->link_); }

    volatile panelLink_t link_;
    T_rx rxDataBuffer_;
    T_tx txDataBuffer_;
};

}  // namespace micro
