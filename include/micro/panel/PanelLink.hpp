#pragma once

#include <micro/utils/timer.hpp>
#include <micro/port/task.hpp>
#include <micro/port/uart.hpp>

namespace micro {

struct PanelLinkData {
    uint8_t checksum = 0;
} __attribute__((packed));

template <typename T, class = typename std::enable_if<std::is_base_of<PanelLinkData, T>::value>::type>
uint8_t calcChecksum(const T& data) {

    const uint8_t * const buffer = reinterpret_cast<const uint8_t*>(&data) + 1; // the first byte of the type is the checksum
    uint8_t sum = 0x55; // the checksum for a 0-length type will be 0x55

    for (uint8_t i = 0; i < sizeof(T) - 1; ++i) {
        sum += buffer[i] * (i + 1);
    }

    return sum;
}

template <typename T, class = typename std::enable_if<std::is_base_of<PanelLinkData, T>::value>::type>
bool isChecksumOk(const T& data) {
    return data.checksum == calcChecksum(data);
}

enum class panelLinkRole_t {
    Master,
    Slave
};

template <typename T_rx, typename T_tx>
class PanelLink {
public:
    static_assert(std::is_base_of<PanelLinkData, T_rx>::value, "T_rx must be a descendant of PanelLinkData");
    static_assert(std::is_base_of<PanelLinkData, T_tx>::value, "T_tx must be a descendant of PanelLinkData");

    PanelLink(panelLinkRole_t role, const uart_t& uart);

    bool isConnected() const;

    bool shouldSend() const;

    void send(const T_tx& txData);

    void onNewRxData();

    bool readAvailable(T_rx& rxData);

    void update();

private:
    enum class state_t {
        Disconnected,
        WaitingData,
        Connected,
    };

    state_t state_;
    panelLinkRole_t role_;
    const uart_t uart_;
    T_rx rxData_;
    T_rx rxAccessibleData_;
    millisecond_t lastRxTime_;
    T_tx txData_;
    millisecond_t lastTxTime_;
    bool isAvailable_;
    PanelLinkData startData_;
};

template <typename T_rx, typename T_tx>
PanelLink<T_rx, T_tx>::PanelLink(panelLinkRole_t role, const uart_t& uart)
    : state_(state_t::Disconnected)
    , role_(role)
    , uart_(uart)
    , isAvailable_(false) {}

template <typename T_rx, typename T_tx>
bool PanelLink<T_rx, T_tx>::isConnected() const {
    return state_t::Connected == this->state_;
}

template <typename T_rx, typename T_tx>
bool PanelLink<T_rx, T_tx>::shouldSend() const {
    return this->isConnected() && getTime() - this->lastTxTime_ >= T_tx::period();
}

template <typename T_rx, typename T_tx>
void PanelLink<T_rx, T_tx>::send(const T_tx& txData) {
    if (this->isConnected()) {
        this->txData_ = txData;
        this->txData_.checksum = calcChecksum(this->txData_);
        uart_transmit(this->uart_, (uint8_t*)&this->txData_, sizeof(this->txData_));
        this->lastTxTime_ = getTime();
    }
}

template <typename T_rx, typename T_tx>
void PanelLink<T_rx, T_tx>::onNewRxData() {
    if (reinterpret_cast<uint8_t*>(&this->startData_) == this->uart_.handle->pRxBuffPtr ? isChecksumOk(this->startData_) : isChecksumOk(this->rxData_)) {
        this->rxAccessibleData_ = this->rxData_;
        this->isAvailable_ = true;
        this->lastRxTime_ = getTime();
    }
}

template <typename T_rx, typename T_tx>
bool PanelLink<T_rx, T_tx>::readAvailable(T_rx& rxData) {
    bool available = false;
    if (this->isConnected() && this->isAvailable_) {
        const interruptStatus_t interruptStatus = os_enterCritical();
        rxData = this->rxAccessibleData_;
        this->isAvailable_ = false;
        os_exitCritical(interruptStatus);
        available = true;
    }
    return available;
}

template <typename T_rx, typename T_tx>
void PanelLink<T_rx, T_tx>::update() {
    switch (this->state_) {

    case state_t::Disconnected:
        this->isAvailable_ = false;
        this->lastRxTime_ = this->lastTxTime_ = getTime();

        if (panelLinkRole_t::Master == this->role_) {
            uart_receive(this->uart_, reinterpret_cast<uint8_t*>(&this->rxData_), sizeof(T_rx));
            this->startData_.checksum = calcChecksum(this->startData_);
            uart_transmit(this->uart_, reinterpret_cast<uint8_t*>(&this->startData_), sizeof(PanelLinkData));
        } else { // Slave
            this->startData_.checksum = 0;
            uart_receive(this->uart_, reinterpret_cast<uint8_t*>(&this->startData_), sizeof(PanelLinkData));
        }
        this->state_ = state_t::WaitingData;
        break;

    case state_t::WaitingData:
        if (this->isAvailable_) {
            if (panelLinkRole_t::Master == this->role_) {
                this->state_ = state_t::Connected;
            } else { // Slave
                this->isAvailable_ = false;
                uart_stopReceive(this->uart_);
                uart_receive(this->uart_, reinterpret_cast<uint8_t*>(&this->rxData_), sizeof(T_rx));
                this->lastTxTime_ = millisecond_t(0); // forces response to be sent as soon as possible
                this->state_ = state_t::Connected;
            }
        } else if (getTime() - this->lastRxTime_ > T_rx::timeout()) {
            uart_stopReceive(this->uart_);
            this->state_ = state_t::Disconnected;
        }
        break;

    case state_t::Connected:
        if (getTime() - this->lastRxTime_ > T_rx::timeout()) {
            uart_stopReceive(this->uart_);
            this->state_ = state_t::Disconnected;
        }
        break;
    }
}

}  // namespace micro
