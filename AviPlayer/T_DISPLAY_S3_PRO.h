#pragma once

#define SPI_MOSI 17
#define SPI_MISO 8
#define SPI_SCK 18

// Display
#include <Arduino_GFX_Library.h>
#define GFX_BL 48
Arduino_DataBus *bus = new Arduino_HWSPI(9 /* DC */, 39 /* CS */, 18 /* SCK */, 17 /* MOSI */, 8 /* MISO */);
Arduino_GFX *gfx = new Arduino_ST7796(bus, 47 /* RST */, 3 /* rotation */, true /* IPS */, 222 /* width */, 480 /* height */, 49 /* col offset 1 */, 0 /* row offset 1 */, 49 /* col offset 2 */, 0 /* row offset 2 */);
#define GFX_SPEED 80000000UL

// Button
#define BTN_A_PIN 0
#define BTN_B_PIN 12
#define BTN_C_PIN 16

// I2C
#define I2C_SDA 5
#define I2C_SCL 6
#define I2C_FREQ 400000UL

// Touchscreen
// #define TOUCH_MODULES_GT911                    // GT911 / CST_SELF / CST_MUTUAL / ZTW622 / L58 / FT3267 / FT5x06
// #define TOUCH_MODULE_ADDR GT911_SLAVE_ADDRESS1 // CTS328_SLAVE_ADDRESS / L58_SLAVE_ADDRESS / CTS826_SLAVE_ADDRESS / CTS820_SLAVE_ADDRESS / CTS816S_SLAVE_ADDRESS / FT3267_SLAVE_ADDRESS / FT5x06_ADDR / GT911_SLAVE_ADDRESS1 / GT911_SLAVE_ADDRESS2 / ZTW622_SLAVE1_ADDRESS / ZTW622_SLAVE2_ADDRESS
// #define TOUCH_SCL I2C_SCL
// #define TOUCH_SDA I2C_SDA
// #define TOUCH_RES -1
// #define TOUCH_INT TDECK_TOUCH_INT

// SD card
// #define SD_SCK SPI_SCK
// #define SD_MOSI SPI_MOSI // CMD
// #define SD_MISO SPI_MISO // D0
// #define SD_D1 1
// #define SD_D2 6
#define SD_CS 14 // D3

// I2S
#define I2S_DEFAULT_GAIN_LEVEL 0.5
#define I2S_OUTPUT_NUM I2S_NUM_0
#define I2S_MCLK -1
#define I2S_BCLK 4
#define I2S_LRCK 15
#define I2S_DOUT 11
#define I2S_DIN -1

// #define AUDIO_MUTE_PIN 48   // LOW for mute
