#include <stdio.h>
#include <memory>

#include "pico/stdlib.h"
#include "pico/binary_info/code.h"

#include "pio_rotary_encoder.pio.h"
#include "pio_rotary_encoder.h"

#include "SERLCD_I2C.hpp"
#include "JOYSTICK_I2C.hpp"

// This represents a state machine
enum screen
{
    START,
    PROPORTION,
    INTEGRAL,
    DERIVATIVE,
    TARGET,
    RUNNING
};

enum target
{
    POSITION,
    VELOCITY
};

void set_screen(std::unique_ptr<SERLCD_I2C> &lcd, screen new_screen, screen &scr)
{
    scr = new_screen;
    switch (scr)
    {
    case START:
        lcd->clear();
        lcd->backlight(0xFF, 0x00, 0xFF);
        lcd->print("  OpenPID v2.0  \nChris Collander");
        break;
    case PROPORTION:
        lcd->clear();
        lcd->backlight(0x00, 0xFF, 0x00);
        lcd->print("Proportion");
        break;
    case INTEGRAL:
        lcd->clear();
        lcd->backlight(0x00, 0xFF, 0x00);
        lcd->print("Integral");
        break;
    case DERIVATIVE:
        lcd->clear();
        lcd->backlight(0x00, 0xFF, 0x00);
        lcd->print("Derivative");
        break;
    case TARGET:
        lcd->clear();
        lcd->backlight(0x00, 0xFF, 0x00);
        lcd->print("Target");
        break;
    case RUNNING:
        lcd->clear();
        lcd->backlight(0xFF, 0x00, 0x00);
        lcd->print("    Running!    \n Press to stop  ");
        break;
    }
}

int main()
{
    stdio_init_all();

    // Rotary Encoder Setup
    RotaryEncoder my_encoder(6); // Connect encoder to D0 and D1, GPIO 6 and 7
    int rotation = 0;
    my_encoder.set_rotation(rotation);

    // General I2C Setup
    constexpr auto I2C = PICO_DEFAULT_I2C_INSTANCE;
    constexpr auto SDA = PICO_DEFAULT_I2C_SDA_PIN;
    constexpr auto SCL = PICO_DEFAULT_I2C_SCL_PIN;
    i2c_init(I2C, 9600);
    gpio_set_function(SDA, GPIO_FUNC_I2C);
    gpio_set_function(SCL, GPIO_FUNC_I2C);
    gpio_pull_up(SDA);
    gpio_pull_up(SCL);

    // LCD Setup
    auto lcd = std::make_unique<SERLCD_I2C>(0x72, 16, 2, I2C);

    // Joystick Setup
    auto joystick = std::make_unique<JOYSTICK_I2C>(0x20, I2C);

    // Starting state
    screen currentScreen = START;
    set_screen(lcd, START, currentScreen);
    target currentTarget = POSITION;
    double k_p = 0.0;
    double k_i = 0.0;
    double k_d = 0.0;
    double k_p_step = 0.1;
    double k_i_step = 0.1;
    double k_d_step = 0.1;

    long unsigned int count = 0;
    while (true)
    {
        switch (currentScreen)
        {
        case RUNNING:
            // Only check for joystick motion every 1000 loops, if running
            if (count++ >= 1000)
            {
                count = 0;
                if (joystick->any_action())
                {
                    set_screen(lcd, PROPORTION, currentScreen);
                    // TODO: Set motor to zero
                    continue;
                }
            }
            rotation = my_encoder.get_rotation();
            // printf("rotation=%d\n", my_encoder.get_rotation());
            //  TODO: Calculate PID and set motor
            break;
        case PROPORTION:
            if (joystick->is_right())
            {
                set_screen(lcd, INTEGRAL, currentScreen);
            }
            else if (joystick->is_left())
            {
                set_screen(lcd, TARGET, currentScreen);
            }
            else if (joystick->is_pressed())
            {
                set_screen(lcd, RUNNING, currentScreen);
            }
            else if (joystick->is_up())
            {
                k_p += k_p_step;
                set_screen(lcd, PROPORTION, currentScreen);
            }
            else if (joystick->is_down())
            {
                k_p -= k_p_step;
                set_screen(lcd, PROPORTION, currentScreen);
            }
            break;
        case INTEGRAL:
            if (joystick->is_right())
            {
                set_screen(lcd, DERIVATIVE, currentScreen);
            }
            else if (joystick->is_left())
            {
                set_screen(lcd, PROPORTION, currentScreen);
            }
            else if (joystick->is_pressed())
            {
                set_screen(lcd, RUNNING, currentScreen);
            }
            else if (joystick->is_up())
            {
                k_i += k_i_step;
                set_screen(lcd, INTEGRAL, currentScreen);
            }
            else if (joystick->is_down())
            {
                k_i -= k_i_step;
                set_screen(lcd, INTEGRAL, currentScreen);
            }
            break;
        case DERIVATIVE:
            if (joystick->is_right())
            {
                set_screen(lcd, TARGET, currentScreen);
            }
            else if (joystick->is_left())
            {
                set_screen(lcd, INTEGRAL, currentScreen);
            }
            else if (joystick->is_pressed())
            {
                set_screen(lcd, RUNNING, currentScreen);
            }
            else if (joystick->is_up())
            {
                k_d += k_d_step;
                set_screen(lcd, DERIVATIVE, currentScreen);
            }
            else if (joystick->is_down())
            {
                k_d -= k_d_step;
                set_screen(lcd, DERIVATIVE, currentScreen);
            }
            break;
        case TARGET:
            if (joystick->is_right())
            {
                set_screen(lcd, PROPORTION, currentScreen);
            }
            else if (joystick->is_left())
            {
                set_screen(lcd, DERIVATIVE, currentScreen);
            }
            else if (joystick->is_pressed())
            {
                set_screen(lcd, RUNNING, currentScreen);
            }
            else if (joystick->is_up() || joystick->is_down())
            {
                currentTarget = (currentTarget == POSITION) ? VELOCITY : POSITION;
            }
            break;
        }
    }
}