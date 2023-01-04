#include "SERLCD_I2C.h"

SERLCD_I2C::SERLCD_I2C(byte address, byte columns, byte rows, i2c_inst *I2C) noexcept
    : address(address), columns(columns), rows(rows), I2C_instance(I2C)
{
    _displayControl = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    _displayMode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    init();
}

void SERLCD_I2C::init()
{
    byte msg[] = {
        SPECIAL_COMMAND,
        (byte)(LCD_DISPLAYCONTROL | _displayControl),
        SPECIAL_COMMAND,
        (byte)(LCD_ENTRYMODESET | _displayMode),
        SETTING_COMMAND,
        CLEAR_COMMAND};
    i2c_write_blocking(I2C_instance, address, msg, 6, false);
    sleep_ms(50);
}

double SERLCD_I2C::map(double x, double in_min, double in_max, double out_min, double out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void SERLCD_I2C::clear()
{
    byte msg[] = {
        SETTING_COMMAND,
        CLEAR_COMMAND};
    i2c_write_blocking(I2C_instance, address, msg, 2, false);
    sleep_ms(10);
}

void SERLCD_I2C::home()
{
    byte msg[] = {
        SPECIAL_COMMAND,
        LCD_RETURNHOME};
    i2c_write_blocking(I2C_instance, address, msg, 2, false);
    sleep_ms(50);
}

void SERLCD_I2C::write(const byte *buffer, size_t size)
{
    i2c_write_blocking(I2C_instance, address, buffer, size, false);
    sleep_ms(10);
}

void SERLCD_I2C::print(std::string msg)
{
    write((const byte *)msg.c_str(), msg.length());
}

void SERLCD_I2C::display(bool enable)
{
    if (enable)
        _displayControl |= LCD_DISPLAYON;
    else
        _displayControl &= ~LCD_DISPLAYON;
    byte msg[] = {
        SPECIAL_COMMAND,
        (byte)(LCD_DISPLAYCONTROL | _displayControl)};
    i2c_write_blocking(I2C_instance, address, msg, 2, false);
    sleep_ms(50);
}

void SERLCD_I2C::cursor(bool enable)
{
    if (enable)
        _displayControl |= LCD_CURSORON;
    else
        _displayControl &= ~LCD_CURSORON;
    byte msg[] = {
        SPECIAL_COMMAND,
        (byte)(LCD_DISPLAYCONTROL | _displayControl)};
    i2c_write_blocking(I2C_instance, address, msg, 2, false);
    sleep_ms(50);
}

void SERLCD_I2C::blink(bool enable)
{
    if (enable)
        _displayControl |= LCD_BLINKON;
    else
        _displayControl &= ~LCD_BLINKON;
    byte msg[] = {
        SPECIAL_COMMAND,
        (byte)(LCD_DISPLAYCONTROL | _displayControl)};
    i2c_write_blocking(I2C_instance, address, msg, 2, false);
    sleep_ms(50);
}

void SERLCD_I2C::backlight(byte r, byte g, byte b)
{
    // r = 128 + map(r, 0, 255, 0, 29);
    // g = 128 + map(g, 0, 255, 0, 29);
    // b = 128 + map(b, 0, 255, 0, 29);

    byte msg[] = {
        SETTING_COMMAND,
        SET_RGB_COMMAND,
        r,
        g,
        b};
    i2c_write_blocking(I2C_instance, address, msg, 5, false);
    sleep_ms(10);
}