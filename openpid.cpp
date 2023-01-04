#include <stdio.h>
#include <memory>
#include <string>

#include "pico/stdlib.h"
#include "pico/binary_info/code.h"

#include "pio_rotary_encoder.pio.h"
#include "pio_rotary_encoder.h"

#include "SERLCD_I2C.h"
#include "JOYSTICK_I2C.h"

#include "pid.h"

#define P_INC 0.01
#define I_INC 0.0001
#define D_INC 0.001
#define I_LIMIT_INC 0.1

// This represents a state machine
enum screen
{
    START,

    PROPORTION,
    INTEGRAL,
    DERIVATIVE,
    INTEGRAL_LIMIT,

    RUNNING
};

void set_screen(std::unique_ptr<SERLCD_I2C> &lcd, screen new_screen, screen &scr, PID &pid)
{
    std::string line1;
    std::string line2;
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
        line1 = "<- Proportion ->";
        line2 = std::to_string(pid.get_k_p());
        lcd->print(line1 + "\n" + line2);
        break;
    case INTEGRAL:
        lcd->clear();
        lcd->backlight(0x00, 0xFF, 0x00);
        line1 = "<-  Integral  ->";
        line2 = std::to_string(pid.get_k_i());
        lcd->print(line1 + "\n" + line2);
        break;
    case DERIVATIVE:
        lcd->clear();
        lcd->backlight(0x00, 0xFF, 0x00);
        line1 = "<- Derivative ->";
        line2 = std::to_string(pid.get_k_d());
        lcd->print(line1 + "\n" + line2);
        break;
    case INTEGRAL_LIMIT:
        lcd->clear();
        lcd->backlight(0x00, 0xFF, 0x00);
        line1 = "<- Int Limit  ->";
        line2 = std::to_string(pid.get_integral_limit());
        lcd->print(line1 + "\n" + line2);
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
    PID pid{};
    screen currentScreen = START;
    set_screen(lcd, START, currentScreen, pid);

    
    // ONLY FOR DEBUGGING JOYSTICK!
    while(false) {
        joystick->update();
        JoystickStatus status = joystick->get_status();
        lcd->clear();
        lcd->backlight(0x00, 0xFF, 0xFF);
        std::string s;
        if(joystick->is_left())
            s = "LEFT";
        else if(joystick->is_right())
            s = "RIGHT";
        else if(joystick->is_up())
            s = "UP";
        else if(joystick->is_down())
            s = "DOWN";
        else if(joystick->is_pressed())
            s = "PRESSED";
        else
            s = "NONE";
        lcd->print(s);
    }
    


    long unsigned int count = 0;
    while (true)
    {
        if(currentScreen != RUNNING) {
            joystick->update();
        }

        switch (currentScreen)
        {
        case START:
            sleep_ms(5000);
            set_screen(lcd, PROPORTION, currentScreen, pid);
            break;
        case RUNNING:
            // Only check for joystick motion every 1000 loops, if running
            if (count++ >= 10000)
            {
                count = 0;
                joystick->update();
                if (joystick->any_action())
                {
                    set_screen(lcd, PROPORTION, currentScreen, pid);
                    // TODO: Set motor to zero
                    joystick->wait_for_release();
                    continue;
                }
            }
            rotation = my_encoder.get_rotation();
            printf("rotation=%d\n", rotation);
            //  TODO: Calculate PID and set motor
            break;
        case PROPORTION:
            if (joystick->is_right())
            {
                set_screen(lcd, INTEGRAL, currentScreen, pid);
                joystick->wait_for_release();
            }
            else if (joystick->is_left())
            {
                set_screen(lcd, INTEGRAL_LIMIT, currentScreen, pid);
                joystick->wait_for_release();
            }
            else if (joystick->is_pressed())
            {
                set_screen(lcd, RUNNING, currentScreen, pid);
                joystick->wait_for_release();
            }
            else if (joystick->is_up())
            {
                pid.increment_k_p(P_INC);
                set_screen(lcd, PROPORTION, currentScreen, pid);
            }
            else if (joystick->is_down())
            {
                pid.increment_k_p(-P_INC);
                set_screen(lcd, PROPORTION, currentScreen, pid);
            }
            break;
        case INTEGRAL:
            if (joystick->is_right())
            {
                set_screen(lcd, DERIVATIVE, currentScreen, pid);
                joystick->wait_for_release();
            }
            else if (joystick->is_left())
            {
                set_screen(lcd, PROPORTION, currentScreen, pid);
                joystick->wait_for_release();
            }
            else if (joystick->is_pressed())
            {
                set_screen(lcd, RUNNING, currentScreen, pid);
                joystick->wait_for_release();
            }
            else if (joystick->is_up())
            {
                pid.increment_k_i(I_INC);
                set_screen(lcd, INTEGRAL, currentScreen, pid);
            }
            else if (joystick->is_down())
            {
                pid.increment_k_i(-I_INC);
                set_screen(lcd, INTEGRAL, currentScreen, pid);
            }
            break;
        case DERIVATIVE:
            if (joystick->is_right())
            {
                set_screen(lcd, INTEGRAL_LIMIT, currentScreen, pid);
                joystick->wait_for_release();
            }
            else if (joystick->is_left())
            {
                set_screen(lcd, INTEGRAL, currentScreen, pid);
                joystick->wait_for_release();
            }
            else if (joystick->is_pressed())
            {
                set_screen(lcd, RUNNING, currentScreen, pid);
                joystick->wait_for_release();
            }
            else if (joystick->is_up())
            {
                pid.increment_k_d(D_INC);
                set_screen(lcd, DERIVATIVE, currentScreen, pid);
            }
            else if (joystick->is_down())
            {
                pid.increment_k_d(-D_INC);
                set_screen(lcd, DERIVATIVE, currentScreen, pid);
            }
            break;
        case INTEGRAL_LIMIT:
            if (joystick->is_right())
            {
                set_screen(lcd, PROPORTION, currentScreen, pid);
                joystick->wait_for_release();
            }
            else if (joystick->is_left())
            {
                set_screen(lcd, DERIVATIVE, currentScreen, pid);
                joystick->wait_for_release();
            }
            else if (joystick->is_pressed())
            {
                set_screen(lcd, RUNNING, currentScreen, pid);
                joystick->wait_for_release();
            }
            else if (joystick->is_up())
            {
                pid.increment_integral_limit(I_LIMIT_INC);
                set_screen(lcd, INTEGRAL_LIMIT, currentScreen, pid);
            }
            else if (joystick->is_down())
            {
                pid.increment_integral_limit(-I_LIMIT_INC);
                set_screen(lcd, INTEGRAL_LIMIT, currentScreen, pid);
            }
            break;
        }
    }
}