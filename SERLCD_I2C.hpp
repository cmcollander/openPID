#pragma once

#include <hardware/gpio.h>
#include <hardware/i2c.h>

#include <string_view>
#include <cstdint>
#include <array>
#include <string>
#include <stdio.h>

class SERLCD_I2C final
{
private:
   using byte = uint8_t;

   // Command Characters
   static constexpr byte SPECIAL_COMMAND = 0xFE;
   static constexpr byte SETTING_COMMAND = 0x7C;

   // OpenLCD Commands
   static constexpr byte CLEAR_COMMAND = 0x2D;
   static constexpr byte SET_RGB_COMMAND = 0x2B;

   // Special Commands
   static constexpr byte LCD_RETURNHOME = 0x02;
   static constexpr byte LCD_ENTRYMODESET = 0x04;
   static constexpr byte LCD_DISPLAYCONTROL = 0x08;

   static constexpr byte LCD_ENTRYRIGHT = 0x00;
   static constexpr byte LCD_ENTRYLEFT = 0x02;
   static constexpr byte LCD_ENTRYSHIFTINCREMENT = 0x01;
   static constexpr byte LCD_ENTRYSHIFTDECREMENT = 0x00;

   static constexpr byte LCD_DISPLAYON = 0x04;
   static constexpr byte LCD_DISPLAYOFF = 0x00;
   static constexpr byte LCD_CURSORON = 0x02;
   static constexpr byte LCD_CURSOROFF = 0x00;
   static constexpr byte LCD_BLINKON = 0x01;
   static constexpr byte LCD_BLINKOFF = 0x00;

   byte address{};
   byte columns{};
   byte rows{};
   i2c_inst *I2C_instance{nullptr};
   byte _displayControl{};
   byte _displayMode{};

   void init();
   void write(const byte *buffer, size_t size);
   double map(double x, double in_min, double in_max, double out_min, double out_max);

public:
   SERLCD_I2C(byte address, byte columns, byte rows, i2c_inst *I2C)
   noexcept;
   void clear();
   void home();
   void print(std::string msg);
   void display(bool enable);
   void cursor(bool enable);
   void blink(bool enable);
   void backlight(byte r, byte g, byte b);
};
