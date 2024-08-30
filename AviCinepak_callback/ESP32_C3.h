#pragma once

#define SPI_MOSI 6
#define SPI_MISO 5
#define SPI_SCK 4

// Display
#include <Arduino_GFX_Library.h>
#define GFX_BL 0
Arduino_DataBus *bus = new Arduino_HWSPI(3 /* DC */, 7 /* CS */, SPI_SCK, SPI_MOSI, SPI_MISO);
// 1.69" IPS round corner LCD 240x280
Arduino_ST7789 *gfx = new Arduino_ST7789(bus, 2 /* RST */, 1 /* rotation */, true /* IPS */, 240 /* width */, 288 /* height */, 0 /* col offset 1 */, 20 /* row offset 1 */, 0 /* col offset 2 */, 12 /* row offset 2 */);
#define GFX_SPEED 80000000UL

// Button
// #define LEFT_BTN_PIN 0
// #define RIGHT_BTN_PIN 21

// I2C
// #define I2C_SDA 18
// #define I2C_SCL 8
// #define I2C_FREQ 800000UL

// Touchscreen
// #define TOUCH_MODULES_GT911                    // GT911 / CST_SELF / CST_MUTUAL / ZTW622 / L58 / FT3267 / FT5x06
// #define TOUCH_MODULE_ADDR GT911_SLAVE_ADDRESS1 // CTS328_SLAVE_ADDRESS / L58_SLAVE_ADDRESS / CTS826_SLAVE_ADDRESS / CTS820_SLAVE_ADDRESS / CTS816S_SLAVE_ADDRESS / FT3267_SLAVE_ADDRESS / FT5x06_ADDR / GT911_SLAVE_ADDRESS1 / GT911_SLAVE_ADDRESS2 / ZTW622_SLAVE1_ADDRESS / ZTW622_SLAVE2_ADDRESS
// #define TOUCH_SCL I2C_SCL
// #define TOUCH_SDA I2C_SDA
// #define TOUCH_RES -1
// #define TOUCH_INT TDECK_TOUCH_INT

// SD card
// #define SD_SCK TDECK_SPI_SCK
// #define SD_MOSI TDECK_SPI_MOSI // CMD
// #define SD_MISO TDECK_SPI_MISO // D0
// #define SD_D1 1
// #define SD_D2 6
#define SD_CS 1 // D3

// I2S
// #define I2S_DEFAULT_GAIN_LEVEL 0.5
#define I2S_OUTPUT_NUM I2S_NUM_0
#define I2S_MCLK -1
#define I2S_BCLK 20
#define I2S_LRCK 10
#define I2S_DOUT 21
#define I2S_DIN -1

// #define AUDIO_MUTE_PIN 48   // LOW for mute
