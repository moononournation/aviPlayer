/*******************************************************************************
 * AVI Player example
 *
 * Dependent libraries:
 * Arduino_GFX: https://github.com/moononournation/Arduino_GFX.git
 * avilib: https://github.com/lanyou1900/avilib.git
 * ESP32_JPEG: https://github.com/esp-arduino-libs/ESP32_JPEG.git
 *
 * Setup steps:
 * 1. Change your LCD parameters in Arduino_GFX setting
 * 2. Upload AVI file
 *   FFat/LittleFS:
 *     upload FFat (FatFS) data with ESP32 Sketch Data Upload:
 *     ESP32: https://github.com/lorol/arduino-esp32fs-plugin
 *   SD:
 *     Copy files to SD card
 ******************************************************************************/
const char *root = "/root";
// char *avi_filename = (char *)"/root/AviMp3Mjpeg240p15fps.avi";
// char *avi_filename = (char *)"/root/AviMp3Mjpeg272p15fps.avi";
// char *avi_filename = (char *)"/root/AviMp3Mjpeg170x320.avi";
// char *avi_filename = (char *)"/root/AviMp3Mjpeg172x320.avi";
// char *avi_filename = (char *)"/root/AviMp3Mjpeg320x170.avi";
// char *avi_filename = (char *)"/root/AviMp3Mjpeg320x172.avi";
char *avi_filename = (char *)"/root/avi/slides.avi";

// Dev Device Pins: <https://github.com/moononournation/Dev_Device_Pins.git>
// #include "PINS_T-DECK.h"
#include "PINS_ESP32-8048S070.h"

#include <FFat.h>
#include <LittleFS.h>
#include <SPIFFS.h>
#include <SD.h>
#include <SD_MMC.h>

size_t output_buf_size;
uint16_t *output_buf;

#include "AviFunc.h"

void setup()
{
#ifdef DEV_DEVICE_INIT
  DEV_DEVICE_INIT();
#endif

  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  // while(!Serial);
  Serial.println("AviMjpeg");

  // If display and SD shared same interface, init SPI first
#ifdef SPI_SCK
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
#endif

  // Init Display
  // if (!gfx->begin())
  if (!gfx->begin(GFX_SPEED))
  {
    Serial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(RGB565_BLACK);

#ifdef GFX_BL
#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR < 3)
  ledcSetup(0, 1000, 8);
  ledcAttachPin(GFX_BL, 0);
  ledcWrite(0, 192);
#else  // ESP_ARDUINO_VERSION_MAJOR >= 3
//  ledcAttachChannel(GFX_BL, 1000, 8, 1);
//  ledcWrite(GFX_BL, 192);
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif // ESP_ARDUINO_VERSION_MAJOR >= 3
#endif // GFX_BL

  // gfx->setTextColor(RGB565_WHITE, RGB565_BLACK);
  // gfx->setTextBound(60, 60, 240, 240);

#if defined(SD_D1)
  SD_MMC.setPins(SD_SCK, SD_MOSI /* CMD */, SD_MISO /* D0 */, SD_D1, SD_D2, SD_CS /* D3 */);
  if (!SD_MMC.begin(root, false /* mode1bit */, false /* format_if_mount_failed */, SDMMC_FREQ_HIGHSPEED))
#elif defined(SD_SCK)
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  SD_MMC.setPins(SD_SCK, SD_MOSI /* CMD */, SD_MISO /* D0 */);
  if (!SD_MMC.begin(root, true /* mode1bit */, false /* format_if_mount_failed */, SDMMC_FREQ_DEFAULT))
#elif defined(SD_CS)
  if (!SD.begin(SD_CS, SPI, 80000000, "/root"))
#else
  if (!FFat.begin(false, root))
  // if (!LittleFS.begin(false, root))
  // if (!SPIFFS.begin(false, root))
#endif
  {
    Serial.println("ERROR: File system mount failed!");
  }
  else
  {
    output_buf_size = gfx->width() * gfx->height() * 2;
#if defined(RGB_PANEL) | defined(DSI_PANEL)
    output_buf = gfx->getFramebuffer();
#else
    output_buf = (uint16_t *)aligned_alloc(16, output_buf_size);
#endif
    if (!output_buf)
    {
      Serial.println("output_buf aligned_alloc failed!");
    }

    avi_init();
  }
}

void loop()
{
  if (avi_open(avi_filename))
  {
    Serial.println("AVI start");
    gfx->fillScreen(RGB565_BLACK);

    avi_start_ms = millis();

    Serial.println("Start play loop");
    while (avi_curr_frame < avi_total_frames)
    {
      if (avi_decode())
      {
        avi_draw(0, 0);
      }
    }

    avi_close();
    Serial.println("AVI end");

//    avi_show_stat();
  }

  delay(1 * 1000);
}
