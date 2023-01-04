#pragma once

#include <hardware/gpio.h>
#include <hardware/i2c.h>

#include <string_view>
#include <cstdint>
#include <array>
#include <string>
#include <stdio.h>

struct JoystickStatus
{
   int x;
   int y;
   bool pushed;
};

class JOYSTICK_I2C final
{
private:
   using byte = uint8_t;

   byte address{};
   JoystickStatus status{};
   i2c_inst *I2C_instance{nullptr};

public:
   JOYSTICK_I2C(byte address, i2c_inst *I2C) noexcept;
   void update(); // This actually pulls the data from the joystick

   // These functions are used to get the data pulled from the joystick at the last update
   bool is_pressed();
   JoystickStatus get_status();
   bool is_left();
   bool is_right();
   bool is_up();
   bool is_down();
   bool any_action();
   
   void wait_for_release();
};
