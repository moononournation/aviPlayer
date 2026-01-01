/*******************************************************************************
 * AVI Player example
 *
 * Dependent libraries:
 * Arduino_GFX: https://github.com/moononournation/Arduino_GFX.git
 * avilib: https://github.com/lanyou1900/avilib.git
 * libhelix: https://github.com/pschatzmann/arduino-libhelix.git
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
 *
 * Video Format:
 * code "cvid": Cinepak
 * cod "MJPG": MJPEG
 *
 * Audio Format:
 * code 1: PCM
 * code 85: MP3
 ******************************************************************************/
const char *root = "/root";
const char *avi_folder = "/avi";

#include <Wire.h>
#include "es8311.h"
// Dev Device Pins: <https://github.com/moononournation/Dev_Device_Pins.git>
// #include "PINS_AD35-S3.h"
// #include "PINS_ESP32-S3-Touch-LCD-1_3.h"
// #include "PINS_ESP32-S3-Touch-LCD-1_3_prism.h"
// #include "PINS_ESP32-S3-Touch-AMOLED-1_8.h"
// #include "PINS_ESP32-S3-Touch-LCD-2_8.h"
// #include "PINS_IBUBLY.h"
// #include "PINS_JC1060P470.h"
// #include "PINS_JC3248W535.h"
// #include "PINS_JC8012P4A1.h"
#include "PINS_T-DECK.h"
// #include "PINS_T4_S3.h"

#ifdef I2S_OUTPUT
#define AVI_SUPPORT_AUDIO
#endif

#include "AviFunc.h"

#ifdef AVI_SUPPORT_AUDIO
#include "esp32_audio.h"
#endif

#include <string>

#include <FFat.h>
#include <LittleFS.h>
#include <SPIFFS.h>
#include <SD.h>
#ifdef SOC_SDMMC_HOST_SUPPORTED
#include <SD_MMC.h>
#endif

void setup()
{
#ifdef DEV_DEVICE_INIT
  DEV_DEVICE_INIT();
#endif

  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  // while(!Serial);
  Serial.println("AVI Player");

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
#if defined(RGB_PANEL) || defined(DSI_PANEL) || defined(CANVAS)
  gfx->flush(true /* force_flush */);
#endif

#ifdef GFX_BL
#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR < 3)
  ledcSetup(0, 1000, 8);
  ledcAttachPin(GFX_BL, 0);
  ledcWrite(0, 63);
#else  // ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttachChannel(GFX_BL, 1000, 8, 1);
  ledcWrite(GFX_BL, 63);
#endif // ESP_ARDUINO_VERSION_MAJOR >= 3
#endif // GFX_BL

  // gfx->setTextColor(RGB565_WHITE, RGB565_BLACK);
  // gfx->setTextBound(60, 60, 240, 240);

#ifdef AVI_SUPPORT_AUDIO
#ifdef AUDIO_EXTRA_PRE_INIT
  AUDIO_EXTRA_PRE_INIT();
#endif

  i2s_init();

#ifdef AUDIO_MUTE
  pinMode(AUDIO_MUTE, OUTPUT);
  digitalWrite(AUDIO_MUTE, LOW); // mute first
#endif
#endif // AVI_SUPPORT_AUDIO

#if defined(SD_D1) && defined(SOC_SDMMC_HOST_SUPPORTED)
#define FILESYSTEM SD_MMC
  Serial.println("mount SD_MMC 4-bit");
  SD_MMC.setPins(SD_SCK, SD_MOSI /* CMD */, SD_MISO /* D0 */, SD_D1, SD_D2, SD_CS /* D3 */);
  if (!SD_MMC.begin(root, false /* mode1bit */, false /* format_if_mount_failed */, SDMMC_FREQ_HIGHSPEED))
#elif defined(SD_SCK) && defined(SOC_SDMMC_HOST_SUPPORTED)
#define FILESYSTEM SD_MMC
  Serial.println("mount SD_MMC 1-bit");
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  SD_MMC.setPins(SD_SCK, SD_MOSI /* CMD */, SD_MISO /* D0 */);
  if (!SD_MMC.begin(root, true /* mode1bit */, false /* format_if_mount_failed */, SDMMC_FREQ_HIGHSPEED))
#elif defined(SD_CS)
#define FILESYSTEM SD
  Serial.println("mount SPI SD");
  if (!SD.begin(SD_CS, SPI, 80000000, "/root"))
#else
#define FILESYSTEM FFat
  Serial.println("mount FFat");
  if (!FFat.begin(false, root))
  // Serial.println("mount LittleFS");
  // if (!LittleFS.begin(false, root))
  // Serial.println("mount SPIFFS");
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
  Serial.printf("Open folder: %s\n", avi_folder);
  File dir = FILESYSTEM.open(avi_folder);
  if (!dir.isDirectory())
  {
    Serial.println("Not a directory");
    delay(5000); // avoid error repeat too fast
  }
  else
  {
    File file = dir.openNextFile();
    while (file)
    {
      if (!file.isDirectory())
      {
        std::string s = file.name();
        // if ((!s.starts_with(".")) && (s.ends_with(".avi")))
        if ((s.rfind(".", 0) != 0) && ((int)s.find(".avi", 0) > 0))
        {
          if (random(100) > 90)
          {
            s = root;
            s += file.path();
            if (avi_open((char *)s.c_str()))
            {
              Serial.println("AVI start");
              gfx->fillScreen(RGB565_BLACK);

#ifdef AVI_SUPPORT_AUDIO
#ifdef AUDIO_MUTE
              digitalWrite(AUDIO_MUTE, HIGH); // unmute
#endif

              if (avi_aRate > 0)
              {
                i2s_set_sample_rate(avi_aRate);
              }

              avi_feed_audio();

              if (avi_aFormat == PCM_CODEC_CODE)
              {
                Serial.println("Start play PCM audio task");
                BaseType_t ret_val = pcm_player_task_start();
                if (ret_val != pdPASS)
                {
                  Serial.printf("pcm_player_task_start failed: %d\n", ret_val);
                }
              }
              else if (avi_aFormat == MP3_CODEC_CODE)
              {
                Serial.println("Start play MP3 audio task");
                BaseType_t ret_val = mp3_player_task_start();
                if (ret_val != pdPASS)
                {
                  Serial.printf("mp3_player_task_start failed: %d\n", ret_val);
                }
              }
              else
              {
                Serial.println("No audio task");
              }
#endif

              avi_start_ms = millis();

              Serial.println("Start play loop");
              while (avi_curr_frame < avi_total_frames)
              {
#ifdef AVI_SUPPORT_AUDIO
                avi_feed_audio();
#endif

                if (avi_decode())
                {
                  avi_draw(0, 0);
                }
              }

#if defined(AVI_SUPPORT_AUDIO) && defined(AUDIO_MUTE)
              digitalWrite(AUDIO_MUTE, LOW); // mute
#endif

              avi_close();
              Serial.println("AVI end");

              avi_show_stat();
              delay(5000); // 5 seconds
            }
          }
        }
      }
      file = dir.openNextFile();
    }
    dir.close();
  }
}
