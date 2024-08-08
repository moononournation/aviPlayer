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
 ******************************************************************************/
const char *root = "/root";
// char *avi_filename = (char *)"/root/AviMp3Mjpeg240p15fps.avi";
char *avi_filename = (char *)"/root/AviMp3Mjpeg176p.avi";

#include "T_DECK.h"

#include <FFat.h>
#include <LittleFS.h>
#include <SPIFFS.h>
#include <SD.h>
#include <SD_MMC.h>

uint16_t *output_buf;

#include "AviFunc.h"

void setup()
{
  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  // while(!Serial);
  Serial.println("AviMp3Mjpeg");

  // If display and SD shared same interface, init SPI first
#ifdef SPI_SCK
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
#endif

#ifdef GFX_EXTRA_PRE_INIT
  GFX_EXTRA_PRE_INIT();
#endif

  // Init Display
  // if (!gfx->begin())
  if (!gfx->begin(80000000))
  {
    Serial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(BLACK);

#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif

  // gfx->setTextColor(WHITE, BLACK);
  // gfx->setTextBound(60, 60, 240, 240);

#ifdef AUDIO_MUTE_PIN
  pinMode(AUDIO_MUTE_PIN, OUTPUT);
  digitalWrite(AUDIO_MUTE_PIN, HIGH);
#endif

  i2s_init();

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
  // if (!FFat.begin(false, root))
  // if (!LittleFS.begin(false, root))
  // if (!SPIFFS.begin(false, root))
#endif
  {
    Serial.println("ERROR: File system mount failed!");
  }
  else
  {
    output_buf = (uint16_t *)aligned_alloc(16, gfx->width() * gfx->height() * 2);
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
    gfx->fillScreen(BLACK);

    avi_start_ms = millis();
    avi_next_frame_ms = avi_start_ms + ((avi_curr_frame + 1) * 1000 / avi_fr);

    avi_feed_audio();

    Serial.println("Start play audio task");
    BaseType_t ret_val = mp3_player_task_start();
    if (ret_val != pdPASS)
    {
      Serial.printf("mp3_player_task_start failed: %d\n", ret_val);
    }

    Serial.println("Start play loop");
    while (avi_curr_frame < avi_total_frames)
    {
      avi_feed_audio();
      // avi_draw(0, 0);
      avi_draw(0, 32);
    }

    int time_used = millis() - avi_start_ms;

    avi_close();
    Serial.println("AVI end");

    long played_frames = avi_total_frames - avi_skipped_frames;
    float fps = 1000.0 * played_frames / time_used;
    total_decode_audio_ms -= total_play_audio_ms;

    Serial.printf("Played avi_frames: %ld\n", played_frames);
    Serial.printf("Skipped avi_frames: %ld (%0.1f %%)\n", avi_skipped_frames, 100.0 * avi_skipped_frames / avi_total_frames);
    Serial.printf("Time used: %lu ms\n", time_used);
    Serial.printf("Expected FPS: %0.1f\n", avi_fr);
    Serial.printf("Actual FPS: %0.1f\n", fps);
    Serial.printf("Read audio: %lu ms (%0.1f %%)\n", total_read_audio_ms, 100.0 * total_read_audio_ms / time_used);
    Serial.printf("Decode audio: %lu ms (%0.1f %%)\n", total_decode_audio_ms, 100.0 * total_decode_audio_ms / time_used);
    Serial.printf("Play audio: %lu ms (%0.1f %%)\n", total_play_audio_ms, 100.0 * total_play_audio_ms / time_used);
    Serial.printf("Read video: %lu ms (%0.1f %%)\n", avi_total_read_video_ms, 100.0 * avi_total_read_video_ms / time_used);
    Serial.printf("Decode video: %lu ms (%0.1f %%)\n", avi_total_decode_video_ms, 100.0 * avi_total_decode_video_ms / time_used);
    Serial.printf("Show video: %lu ms (%0.1f %%)\n", avi_total_show_video_ms, 100.0 * avi_total_show_video_ms / time_used);

#define CHART_MARGIN 32
#define LEGEND_A_COLOR 0x1BB6
#define LEGEND_B_COLOR 0xFBE1
#define LEGEND_C_COLOR 0x2D05
#define LEGEND_D_COLOR 0xD125
#define LEGEND_E_COLOR 0x9337
#define LEGEND_F_COLOR 0x8AA9
#define LEGEND_G_COLOR 0xE3B8
#define LEGEND_H_COLOR 0x7BEF
#define LEGEND_I_COLOR 0xBDE4
#define LEGEND_J_COLOR 0x15F9

    int16_t r1 = ((gfx->height() - CHART_MARGIN - CHART_MARGIN) / 2);
    int16_t r2 = r1 / 2;
    int16_t cx = gfx->width() - r1 - CHART_MARGIN;
    int16_t cy = r1 + CHART_MARGIN;

    float arc_start1 = 0;
    float arc_end1 = arc_start1 + max(2.0, 360.0 * total_read_audio_ms / time_used);
    for (int i = arc_start1 + 1; i < arc_end1; i += 2)
    {
      gfx->fillArc(cx, cy, r1, r2, arc_start1 - 90.0, i - 90.0, LEGEND_A_COLOR);
    }
    gfx->fillArc(cx, cy, r1, r2, arc_start1 - 90.0, arc_end1 - 90.0, LEGEND_A_COLOR);

    float arc_start2 = arc_end1;
    float arc_end2 = arc_start2 + max(2.0, 360.0 * avi_total_read_video_ms / time_used);
    for (int i = arc_start2 + 1; i < arc_end2; i += 2)
    {
      gfx->fillArc(cx, cy, r1, r2, arc_start2 - 90.0, i - 90.0, LEGEND_B_COLOR);
    }
    gfx->fillArc(cx, cy, r1, r2, arc_start2 - 90.0, arc_end2 - 90.0, LEGEND_B_COLOR);

    float arc_start3 = arc_end2;
    float arc_end3 = arc_start3 + max(2.0, 360.0 * avi_total_decode_video_ms / time_used);
    for (int i = arc_start3 + 1; i < arc_end3; i += 2)
    {
      gfx->fillArc(cx, cy, r1, r2, arc_start3 - 90.0, i - 90.0, LEGEND_C_COLOR);
    }
    gfx->fillArc(cx, cy, r1, r2, arc_start3 - 90.0, arc_end3 - 90.0, LEGEND_C_COLOR);

    float arc_start4 = arc_end3;
    float arc_end4 = arc_start4 + max(2.0, 360.0 * avi_total_show_video_ms / time_used);
    for (int i = arc_start4 + 1; i < arc_end4; i += 2)
    {
      gfx->fillArc(cx, cy, r1, r2, arc_start4 - 90.0, i - 90.0, LEGEND_D_COLOR);
    }
    gfx->fillArc(cx, cy, r1, r2, arc_start4 - 90.0, arc_end4 - 90.0, LEGEND_D_COLOR);

    float arc_start5 = 0;
    float arc_end5 = arc_start5 + max(2.0, 360.0 * total_decode_audio_ms / time_used);
    for (int i = arc_start5 + 1; i < arc_end5; i += 2)
    {
      gfx->fillArc(cx, cy, r2, 0, arc_start5 - 90.0, i - 90.0, LEGEND_E_COLOR);
    }
    gfx->fillArc(cx, cy, r2, 0, arc_start5 - 90.0, arc_end5 - 90.0, LEGEND_E_COLOR);

    gfx->setCursor(0, 0);
    gfx->setTextColor(WHITE, BLACK);
    gfx->printf("Played avi_frames: %d\n", played_frames);
    gfx->printf("Skipped avi_frames: %ld (%0.1f %%)\n", avi_skipped_frames, 100.0 * avi_skipped_frames / avi_total_frames);
    gfx->printf("Time used: %d ms\n", time_used);
    gfx->printf("Expected FPS: %0.1f\n", avi_fr);
    gfx->printf("Actual FPS: %0.1f\n\n", fps);
    gfx->setTextColor(LEGEND_A_COLOR, BLACK);
    gfx->printf("Read audio: %lu ms (%0.1f %%)\n", total_read_audio_ms, 100.0 * total_read_audio_ms / time_used);
    gfx->setTextColor(LEGEND_B_COLOR, BLACK);
    gfx->printf("Read video: %lu ms (%0.1f %%)\n", avi_total_read_video_ms, 100.0 * avi_total_read_video_ms / time_used);
    gfx->setTextColor(LEGEND_C_COLOR, BLACK);
    gfx->printf("Decode video: %lu ms (%0.1f %%)\n", avi_total_decode_video_ms, 100.0 * avi_total_decode_video_ms / time_used);
    gfx->setTextColor(LEGEND_D_COLOR, BLACK);
    gfx->printf("Show video: %lu ms (%0.1f %%)\n", avi_total_show_video_ms, 100.0 * avi_total_show_video_ms / time_used);
    gfx->setTextColor(LEGEND_E_COLOR, BLACK);
    gfx->printf("Decode audio: %lu ms (%0.1f %%)\n", total_decode_audio_ms, 100.0 * total_decode_audio_ms / time_used);
    gfx->setTextColor(LEGEND_G_COLOR, BLACK);
    gfx->printf("Play audio: %lu ms (%0.1f %%)\n", total_play_audio_ms, 100.0 * total_play_audio_ms / time_used);
  }

  delay(60 * 1000);
}
