#pragma once

#include <micro/port/gpio.hpp>

#include <initializer_list>

namespace micro {
namespace hw {

class HD44780_Lcd {
public:
    enum class wireMode_t : uint8_t {
        Wire4 = 0,
        Wire8
    };

    enum class displayType_t : uint8_t {
        Lcd16xN = 0,
        Lcd20xN
    };

    HD44780_Lcd(const wireMode_t wireMode, const displayType_t displayType, const gpio_t& rs, const gpio_t& rw, const gpio_t& en, const std::initializer_list<gpio_t>& data);

    Status initialize();

    void defineCharacter(uint8_t code, const uint8_t bitmap[8]);

    void setCursor(const uint8_t row, const uint8_t col);

    void write(const char * const str);

    void write(char c);

    void clear();

private:
    enum dataType_t : uint8_t {
        Command = 0,
        Data
    };

    uint8_t numWires() const;

    uint8_t width() const;

    void writeByte(const uint8_t data);

    void write(const uint8_t data, const dataType_t dataType);

    void write(const uint8_t * const data, const uint8_t size);

    const wireMode_t wireMode_;
    const displayType_t displayType_;
    gpio_t rs_;
    gpio_t rw_;
    gpio_t en_;
    gpio_t data_[8];
};

}  // namespace hw
}  // namespace micro
