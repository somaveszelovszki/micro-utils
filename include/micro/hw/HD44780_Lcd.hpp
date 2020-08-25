#pragma once

#include <micro/container/vec.hpp>
#include <micro/port/gpio.hpp>

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

    HD44780_Lcd(const wireMode_t wireMode, const displayType_t displayType, const gpio_t& rs, const gpio_t& rw, const gpio_t& en, const vec<gpio_t, 8>& data);

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

    void writeByte(const uint8_t data);

    void write(const uint8_t data, const dataType_t dataType);

    void write(const uint8_t * const data, const uint8_t size);

    const wireMode_t wireMode_;
    const displayType_t displayType_;
    gpio_t rs_;
    gpio_t rw_;
    gpio_t en_;
    vec<gpio_t, 8> data_;
};

}  // namespace hw
}  // namespace micro
