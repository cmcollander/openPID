#include "JOYSTICK_I2C.h"

JOYSTICK_I2C::JOYSTICK_I2C(byte address, i2c_inst *I2C) noexcept
    : address(address), I2C_instance(I2C) {}

void JOYSTICK_I2C::update()
{
    std::array<byte, 8> data;
    i2c_read_blocking(I2C_instance, address, data.data(), 8, false);

    status.x = static_cast<int>((data[3] << 8) | data[4]) >> 6;
    status.y = static_cast<int>((data[5] << 8) | data[6]) >> 6;
    status.pushed = (data[7] & 0x01) == 0x00;
}

bool JOYSTICK_I2C::is_pressed()
{
    return status.pushed;
}

JoystickStatus JOYSTICK_I2C::get_status()
{
    return status;
}

bool JOYSTICK_I2C::is_right()
{
    return status.y < 450;
}

bool JOYSTICK_I2C::is_left()
{
    return 575 < status.y;
}

bool JOYSTICK_I2C::is_up()
{
    return status.x < 450;
}

bool JOYSTICK_I2C::is_down()
{
    return 575 < status.x;
}

bool JOYSTICK_I2C::any_action()
{
    return is_left() || is_right() || is_up() || is_down() || is_pressed();
}

void JOYSTICK_I2C::wait_for_release()
{
    while (any_action())
    {
        update();
    }
}