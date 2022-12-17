#include <stdio.h>
#include <memory>

#include "pico/stdlib.h"
#include "pico/binary_info/code.h"

#include "pio_rotary_encoder.pio.h"
#include "pio_rotary_encoder.h"

#include "SERLCD_I2C.hpp"

int main()
{
    stdio_init_all();

    // Rotary Encoder Setup
    RotaryEncoder my_encoder(6); // Connect encoder to D0 and D1, GPIO 6 and 7
    my_encoder.set_rotation(0);

    // General I2C Setup
    constexpr auto I2C = PICO_DEFAULT_I2C_INSTANCE;
    constexpr auto SDA = PICO_DEFAULT_I2C_SDA_PIN;
    constexpr auto SCL = PICO_DEFAULT_I2C_SCL_PIN;
    constexpr auto BAUD_RATE = 9600;
    i2c_init(I2C, BAUD_RATE);
    gpio_set_function(SDA, GPIO_FUNC_I2C);
    gpio_set_function(SCL, GPIO_FUNC_I2C);
    //gpio_pull_up(SDA);
    //gpio_pull_up(SCL);

    // LCD Setup
    constexpr auto I2C_ADDR_LCD = 0x72;
    constexpr auto LCD_COLUMNS = 16;
    constexpr auto LCD_ROWS = 2;
    auto lcd = std::make_unique<SERLCD_I2C>(I2C_ADDR_LCD, LCD_COLUMNS, LCD_ROWS, I2C);

    // Separate behavior based on if the user is setting parameters, or the control loop is running
    bool running = false;

    while (true)
    {
        if(running) {
            int rotation = my_encoder.get_rotation();
            printf("rotation=%d\n", my_encoder.get_rotation());
        }
        else {
            lcd->clear();
            lcd->backlight(0xFF,0x00,0x00);
            lcd->print("Red");
            sleep_ms(1000);
            lcd->clear();
            lcd->backlight(0x00,0xFF,0x00);
            lcd->print("Green");
            sleep_ms(1000);
            lcd->clear();
            lcd->backlight(0x00,0x00,0xFF);
            lcd->print("Blue");
            sleep_ms(1000);
        }
    }
}