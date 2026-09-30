/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x64 pixel display using SPI to communicate
 4 or 5 pins are required to interface.

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "lv_font_xzl_16.h"

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI 9
#define OLED_CLK 10
#define OLED_DC 11
#define OLED_CS 12
#define OLED_RESET 13

class Adafruit_SSD1306_CN : public Adafruit_SSD1306 {
private:
  uint32_t cp;
  uint8_t utf8_cnt;
public:
  using Adafruit_SSD1306::Adafruit_SSD1306;
  using Adafruit_SSD1306::write;
  size_t write(uint8_t b) override {
    switch (b >> 4) {
      case 0x8:
      case 0x9:
      case 0xA:
      case 0xB:
        // UTF-8 middle byte
        if (utf8_cnt == 0) {
          return Adafruit_SSD1306::write('?');
        }
        cp <<= 6;
        cp |= (b & 0x3F);
        if ((--utf8_cnt) == 0) {
          uint8_t buf[32];
          if (!xzl_unicode_to_bitmap(cp, buf)) return Adafruit_SSD1306::write('?');
          if (cursor_x + 16 > _width) {
            cursor_y += 16;
            cursor_x = 0;
          }
          drawBitmap(cursor_x, cursor_y, buf, 16, 16, 1);
          cursor_x += 16;
        }
        return 1;
      case 0xC:
      case 0xD:
        // UTF-8 2-byte character
        utf8_cnt = 1;
        cp = b & 0x1F;
        return 1;
      case 0xE:
        // UTF-8 3-byte character
        utf8_cnt = 2;
        cp = b & 0x0F;
        return 1;
      case 0xF:
        // UTF-8 4-byte character
        utf8_cnt = 3;
        cp = b & 0x07;
        return 1;
      default:
        // ASCII
        utf8_cnt = 0;
        return Adafruit_SSD1306::write(b);
    }
  }
};

Adafruit_SSD1306_CN display(
  SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK,
  OLED_DC, OLED_RESET, OLED_CS);

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000);  // Pause for 2 seconds

  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);              // Start at top-left corner
  display.println("HelloWorld袖珍龙 中文系统 文本测试1天地玄黄2宇宙洪荒3");

  display.display();
}

void loop() {
}