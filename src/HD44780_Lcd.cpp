#include <micro/hw/HD44780_Lcd.hpp>

#include <string.h>

#define CLEAR_DISPLAY           0x01

#define RETURN_HOME             0x02

#define ENTRY_MODE_SET          0x04
#define OPT_S                   0x01   // Shift entire display to right
#define OPT_INC                 0x02   // Cursor increment

#define DISPLAY_ON_OFF_CONTROL  0x08
#define OPT_D                   0x04   // Turn on display
#define OPT_C                   0x02   // Turn on cursor
#define OPT_B                   0x01   // Turn on cursor blink

#define CURSOR_DISPLAY_SHIFT    0x10   // Move and shift cursor
#define OPT_SC                  0x08
#define OPT_RL                  0x04

#define FUNCTION_SET            0x20
#define OPT_DL                  0x10    // Set interface data length
#define OPT_N                   0x08    // Set number of display lines
#define OPT_F                   0x04    // Set alternate font
#define SETCGRAM_ADDR           0x040
#define SET_DDRAM_ADDR          0x80    // Set DDRAM address

#define LCD_DATA_REG            1
#define LCD_COMMAND_REG         0

namespace micro {
namespace hw {

constexpr uint8_t ROW_16[] = {0x00, 0x40, 0x10, 0x50};
constexpr uint8_t ROW_20[] = {0x00, 0x40, 0x14, 0x54};

HD44780_Lcd::HD44780_Lcd(const wireMode_t wireMode, const displayType_t displayType, const gpio_t& rs, const gpio_t& rw, const gpio_t& en, const vec<gpio_t, 8>& data)
    : wireMode_(wireMode)
    , displayType_(displayType)
    , rs_(rs)
    , rw_(rw)
    , en_(en)
    , data_(data) {}

Status HD44780_Lcd::initialize() {
    HAL_Delay(50);

    gpio_write(this->rw_, gpioPinState_t::RESET);
    HAL_Delay(50);

    if(wireMode_t::Wire4 == this->wireMode_) {
            this->write(0x33, dataType_t::Command);
            this->write(0x32, dataType_t::Command);
            this->write(FUNCTION_SET | OPT_N, dataType_t::Command); // 4-bit mode
    } else { // Wire8
        this->write(FUNCTION_SET | OPT_DL | OPT_N, dataType_t::Command);
    }

    this->clear();
    this->write(DISPLAY_ON_OFF_CONTROL | OPT_D, dataType_t::Command);     // LCD on, cursor off, cursor-blink off
    this->write(ENTRY_MODE_SET | OPT_INC, dataType_t::Command);           // Increment cursor

    return Status::OK;
}

void HD44780_Lcd::defineCharacter(uint8_t code, const uint8_t bitmap[8]) {
    this->write(SETCGRAM_ADDR + (code << 3), dataType_t::Command);
    for(uint8_t i = 0; i < 8; ++i){
        this->write(bitmap[i], dataType_t::Data);
    }
}

void HD44780_Lcd::setCursor(const uint8_t row, const uint8_t col) {
    if (displayType_t::Lcd16xN == this->displayType_) {
        this->write(SET_DDRAM_ADDR + ROW_16[row] + col, dataType_t::Command);
    } else { // Lcd20xN
        this->write(SET_DDRAM_ADDR + ROW_20[row] + col, dataType_t::Command);
    }
}

void HD44780_Lcd::write(const uint8_t data, const dataType_t dataType) {
    gpio_write(this->rs_, dataType_t::Command == dataType ? gpioPinState_t::RESET : gpioPinState_t::SET); // selects command/data register

    if(wireMode_t::Wire4 == this->wireMode_)
    {
        this->writeByte((data >> 4) & 0x0f);
        this->writeByte(data & 0x0f);
    }
    else { // Wire8
        this->writeByte(data);
    }
}

void HD44780_Lcd::write(const char * const str) {

    const uint32_t displayWidth = displayType_t::Lcd16xN == this->displayType_ ? 16 : 20;
    const uint32_t len          = micro::min<uint32_t>(strlen(str), displayWidth);

    for(uint8_t i = 0; i < len; ++i) {
        this->write(str[i]);
    }

    for (uint32_t i = len; i < displayWidth; ++i) {
        this->write(' ');
    }
}

void HD44780_Lcd::write(char c) {
    if (c >= 'a' && c <= 'z') {
        c = 0b01100001 + (c - 'a');
    } else if (c >= 'A' && c <= 'Z') {
        c = 0b01000001 + (c - 'A');
    } else if (c >= '0' && c <= '9') {
        c = 0b00110000 + (c - '0');
    } else if (c == '+') {
        c = 0b00101011;
    } else if (c == '-') {
        c = 0b00101101;
    } else if (c == '*') {
        c = 0b00101010;
    } else if (c == '/') {
        c = 0b00101111;
    } else if (c == '=') {
        c = 0b00111101;
    } else if (c == ' ') {
        c = 0b00100000;
    }

    this->write(static_cast<uint8_t>(c), dataType_t::Data);
}

void HD44780_Lcd::clear() {
    this->write(CLEAR_DISPLAY, dataType_t::Command);
}

void HD44780_Lcd::writeByte(const uint8_t data) {

    gpio_write(this->en_, gpioPinState_t::SET);

    for(uint8_t i = 0; i < this->data_.size(); ++i) {
        gpio_write(this->data_[i], ((data >> i) & 0x01) ? gpioPinState_t::SET : gpioPinState_t::RESET);
    }

    gpio_write(this->en_, gpioPinState_t::RESET); // data is received on falling edge
}

}  // namespace hw
}  // namespace micro
