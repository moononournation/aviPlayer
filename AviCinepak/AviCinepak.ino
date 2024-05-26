/***
 * Required libraries:
 * Arduino_GFX: https://github.com/moononournation/Arduino_GFX.git
 * avilib: https://github.com/lanyou1900/avilib.git
 */

const char *root = "/root";
const char *avi_file = "/root/AviMp3Cinepak240p15fps.avi";

#include <WiFi.h>

#include <FFat.h>
#include <LittleFS.h>
#include <SD_MMC.h>

extern "C"
{
#include <avilib.h>
}

/*******************************************************************************
 * Start of Arduino_GFX setting
 ******************************************************************************/
#include <Arduino_GFX_Library.h>
#define GFX_DEV_DEVICE ZX3D50CE02S
#define GFX_BL 45
Arduino_DataBus *bus = new Arduino_ESP32LCD8(
    0 /* DC */, GFX_NOT_DEFINED /* CS */, 47 /* WR */, GFX_NOT_DEFINED /* RD */,
    9 /* D0 */, 46 /* D1 */, 3 /* D2 */, 8 /* D3 */, 18 /* D4 */, 17 /* D5 */, 16 /* D6 */, 15 /* D7 */);
Arduino_GFX *gfx = new Arduino_ST7796(bus, 4 /* RST */, 3 /* rotation */, true /* IPS */);
/*******************************************************************************
 * End of Arduino_GFX setting
 ******************************************************************************/

#include "cinepak.h"
CinepakDecoder decoder;

/* variables */
static avi_t *a;
static long frames, estimateBufferSize, aRate, aBytes, aChunks, actual_video_size;
static long w, h, aChans, aBits, aFormat;
static double fr;
static char *compressor;
static char *vidbuf;
static char *audbuf;
static bool isStopped = true;
static long curr_frame = 0;
static long curr_chunk = 0;
static long skipped_frames = 0;
static bool skipped_last_frame = false;
static unsigned long start_ms, next_frame_ms;

// microSD card
#define SD_SCK 39
#define SD_MISO 38
#define SD_MOSI 40
#define SD_CS 41

void setup()
{
  WiFi.mode(WIFI_OFF);

  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  // while(!Serial);
  Serial.println("AviCinepak");

#ifdef GFX_EXTRA_PRE_INIT
  GFX_EXTRA_PRE_INIT();
#endif

  Serial.println("Init display");
  if (!gfx->begin())
  {
    Serial.println("Init display failed!");
  }
  gfx->fillScreen(BLACK);

#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif

  // if (!FFat.begin(false, root))
  // if (!LittleFS.begin(false, root))
  pinMode(SD_CS /* CS */, OUTPUT);
  digitalWrite(SD_CS /* CS */, HIGH);
  SD_MMC.setPins(SD_SCK /* CLK */, SD_MOSI /* CMD/MOSI */, SD_MISO /* D0/MISO */);
  if (!SD_MMC.begin(root, true /* mode1bit */, false /* format_if_mount_failed */, SDMMC_FREQ_DEFAULT))
  {
    Serial.println("ERROR: File system mount failed!");
  }
  else
  {
    a = AVI_open_input_file(avi_file, 1);

    if (a)
    {
      frames = AVI_video_frames(a);
      w = AVI_video_width(a);
      h = AVI_video_height(a);
      fr = AVI_frame_rate(a);
      compressor = AVI_video_compressor(a);
      estimateBufferSize = w * h * 2 / 5;
      Serial.printf("AVI frames: %ld, %ld x %ld @ %.2f fps, format: %s, estimateBufferSize: %ld, ESP.getFreeHeap(): %ld\n", frames, w, h, fr, compressor, estimateBufferSize, (long)ESP.getFreeHeap());

      aChans = AVI_audio_channels(a);
      aBits = AVI_audio_bits(a);
      aFormat = AVI_audio_format(a);
      aRate = AVI_audio_rate(a);
      aBytes = AVI_audio_bytes(a);
      aChunks = AVI_audio_chunks(a);
      Serial.printf("Audio channels: %ld, bits: %ld, format: %ld, rate: %ld, bytes: %ld, chunks: %ld\n", aChans, aBits, aFormat, aRate, aBytes, aChunks);

      output_buf_size = w * h * 2;
      output_buf = (uint16_t *)heap_caps_malloc(output_buf_size, MALLOC_CAP_DMA);
      if (!output_buf)
      {
        output_buf = (uint16_t *)malloc(output_buf_size);
        if (!output_buf)
        {
          Serial.println("output_buf malloc failed!");
        }
      }

      vidbuf = (char *)heap_caps_malloc(estimateBufferSize, MALLOC_CAP_8BIT);
      if (!vidbuf)
      {
        vidbuf = (char *)malloc(estimateBufferSize);
        if (!vidbuf)
        {
          Serial.println("vidbuf malloc failed!");
        }
      }

      audbuf = (char *)heap_caps_malloc(1024, MALLOC_CAP_8BIT);
      if (!audbuf)
      {
        audbuf = (char *)malloc(1024);
        if (!audbuf)
        {
          Serial.println("audbuf malloc failed!");
        }
      }

      isStopped = false;
      start_ms = millis();
      next_frame_ms = start_ms + ((curr_frame + 1) * 1000 / fr);
    }
  }
}

void loop()
{
  if (!isStopped)
  {
    if (curr_frame < frames)
    {
      long audio_bytes = AVI_audio_size(a, curr_chunk++);
      AVI_read_audio(a, audbuf, audio_bytes);

      // if (millis() < next_frame_ms) // check show frame or skip frame
      {
        AVI_set_video_position(a, curr_frame);

        int iskeyframe;
        long video_bytes = AVI_frame_size(a, curr_frame);
        if (video_bytes > estimateBufferSize)
        {
          Serial.printf("video_bytes(%d) > estimateBufferSize(%d)\n", video_bytes, estimateBufferSize);
        }
        else
        {
          actual_video_size = AVI_read_frame(a, vidbuf, &iskeyframe);
          // Serial.printf("frame: %d, iskeyframe: %d, video_bytes: %d, actual_video_size: %d, audio_bytes: %d, ESP.getFreeHeap(): %d\n", curr_frame, iskeyframe, video_bytes, actual_video_size, audio_bytes, (long)ESP.getFreeHeap());

          if ((!skipped_last_frame) || iskeyframe)
          {
            skipped_last_frame = false;
            decoder.decodeFrame((uint8_t *)vidbuf, actual_video_size, output_buf, output_buf_size);
            gfx->draw16bitBeRGBBitmap(0, 0, output_buf, w, h);
          }
          // else
          // {
          //   ++skipped_frames;
          // }
        }
        while (millis() < next_frame_ms)
        {
          vTaskDelay(pdMS_TO_TICKS(1));
        }
      }
      // else
      // {
      //   ++skipped_frames;
      //   skipped_last_frame = true;
      //   // Serial.printf("Skip frame %d > %d\n", millis(), next_frame_ms);
      // }

      ++curr_frame;
      next_frame_ms = start_ms + ((curr_frame + 1) * 1000 / fr);
    }
    else
    {
      AVI_close(a);
      isStopped = true;
      Serial.printf("Duration: %lu, skipped frames: %ld\n", millis() - start_ms, skipped_frames);
    }
  }
  else
  {
    delay(100);
  }
}
