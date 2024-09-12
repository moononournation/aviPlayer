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
// const char *avi_folder = "/";
const char *avi_folder = "/avi_240p";
// const char *avi_folder = "/avi_272p";
// const char *avi_folder = "/avi288x240";
// const char *avi_folder = "/avi480x220";
// #ifdef CANVAS_R1
// const char *avi_folder = "/avi480x320";
// #else
// const char *avi_folder = "/avi320x480";
// #endif

// uncomment one and only one of below dev device pin definition header
// #include "iBubly.h"
// #include "ESP32_4827A043.h"
// #include "JC3248W535.h"
// #include "JC3636W518.h"
// #include "JC4827W543.h"
#include "T_DECK.h"
// #include "T_DISPLAY_S3_PRO.h"

#include <FFat.h>
#include <LittleFS.h>
#include <SPIFFS.h>
#include <SD.h>
#include <SD_MMC.h>

size_t output_buf_size;
uint16_t *output_buf;

#include "AviFunc.h"
#include "esp32_audio.h"

void setup()
{
  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  // while(!Serial);
  Serial.println("AVI Player");

  // If display and SD shared same interface, init SPI first
#ifdef SPI_SCK
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
#endif

#ifdef GFX_EXTRA_PRE_INIT
  GFX_EXTRA_PRE_INIT();
#endif

  // Init Display
  // if (!gfx->begin())
  if (!gfx->begin(GFX_SPEED))
  {
    Serial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(BLACK);
#ifdef CANVAS
  gfx->flush();
#endif

#ifdef GFX_BL
#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR < 3)
  ledcSetup(0, 1000, 8);
  ledcAttachPin(GFX_BL, 0);
  ledcWrite(0, 204);
#else  // ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttachChannel(GFX_BL, 1000, 8, 1);
  ledcWrite(GFX_BL, 204);
#endif // ESP_ARDUINO_VERSION_MAJOR >= 3
#endif // GFX_BL

  // gfx->setTextColor(WHITE, BLACK);
  // gfx->setTextBound(60, 60, 240, 240);

#ifdef AUDIO_MUTE_PIN
  pinMode(AUDIO_MUTE_PIN, OUTPUT);
  digitalWrite(AUDIO_MUTE_PIN, HIGH);
#endif

  i2s_init();

#if defined(SD_D1)
#define FILESYSTEM SD_MMC
  SD_MMC.setPins(SD_SCK, SD_MOSI /* CMD */, SD_MISO /* D0 */, SD_D1, SD_D2, SD_CS /* D3 */);
  if (!SD_MMC.begin(root, false /* mode1bit */, false /* format_if_mount_failed */, SDMMC_FREQ_HIGHSPEED))
#elif defined(SD_SCK)
#define FILESYSTEM SD_MMC
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  SD_MMC.setPins(SD_SCK, SD_MOSI /* CMD */, SD_MISO /* D0 */);
  if (!SD_MMC.begin(root, true /* mode1bit */, false /* format_if_mount_failed */, SDMMC_FREQ_HIGHSPEED))
#elif defined(SD_CS)
#define FILESYSTEM SD
  if (!SD.begin(SD_CS, SPI, 80000000, "/root"))
#else
#define FILESYSTEM FFat
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
#ifdef RGB_PANEL
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
          s = root;
          s += file.path();
          if (avi_open((char *)s.c_str()))
          {
            Serial.println("AVI start");
            gfx->fillScreen(BLACK);

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

            avi_start_ms = millis();

            Serial.println("Start play loop");
            while (avi_curr_frame < avi_total_frames)
            {
              avi_feed_audio();
              if (avi_decode())
              {
                avi_draw(0, 0);
              }
            }

            avi_close();
            Serial.println("AVI end");

            avi_show_stat();
            delay(5000); // 5 seconds
          }
        }
      }
      file = dir.openNextFile();
    }
    dir.close();
  }
}