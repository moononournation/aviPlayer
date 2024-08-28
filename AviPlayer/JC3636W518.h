#pragma once

// Display
#include <Arduino_GFX_Library.h>
#define GFX_BL 15
Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    10 /* cs */, 9 /* sck */, 11 /* d0 */, 12 /* d1 */, 13 /* d2 */, 14 /* d3 */);
Arduino_ST77916 *gfx = new Arduino_ST77916(bus, 47 /* RST */, 0 /* rotation */, true /* IPS */, 360 /* width */, 360 /* height */);
#define GFX_SPEED 80000000UL

// I2C
#define I2C_SDA 7
#define I2C_SCL 8
#define I2C_FREQ 400000UL

// Touchscreen
#define TOUCH_MODULES_CST_SELF
#define TOUCH_MODULE_ADDR CTS816S_SLAVE_ADDRESS
#define TOUCH_SCL I2C_SCL
#define TOUCH_SDA I2C_SDA
#define TOUCH_RES 40
#define TOUCH_INT 41

// SD card
#define SD_SCK 3
#define SD_MOSI 4 // CMD
#define SD_MISO 2 // D0
#define SD_D1 1
#define SD_D2 6
#define SD_CS 5   // D3

// I2S
#define I2S_DEFAULT_GAIN_LEVEL 0.05
#define I2S_OUTPUT_NUM I2S_NUM_0
#define I2S_MCLK -1
#define I2S_BCLK 18
#define I2S_LRCK 16
#define I2S_DOUT 17
#define I2S_DIN -1

#define AUDIO_MUTE_PIN 48   // LOW for mute
