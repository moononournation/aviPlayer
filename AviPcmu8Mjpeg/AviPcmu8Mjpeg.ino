/*
 * require libraries:
 * Arduino_GFX: https://github.com/moononournation/Arduino_GFX.git
 * avilib: https://github.com/lanyou1900/avilib.git
 * JPEGDEC: https://github.com/bitbank2/JPEGDEC.git
 */
#include <FFat.h>
#include <LittleFS.h>
#include <SD_MMC.h>
const char *root = "/root";
const char *avi_file = "/root/AviPcmu8Mjpeg320p10fps.avi";

extern "C"
{
#include <avilib.h>
}
static avi_t *a;
static long frames, estimateBufferSize, aRate, aBytes, aChunks, actual_video_size;
static int w, h, aChans, aBits, aFormat;
static double fr;
static char *compressor;
static char *vidbuf;
static char *audbuf;
static bool isStopped = true;
static int curr_frame = 0;
static long curr_chunk = 0;
static int skipped_frames = 0;
static unsigned long start_ms, next_frame_ms;
static int audio_feed_per_frame;

#include <Arduino_GFX_Library.h>

// #define GFX_BL DF_GFX_BL // default backlight pin, you may replace DF_GFX_BL to actual backlight pin
///* More data bus class: https://github.com/moononournation/Arduino_GFX/wiki/Data-Bus-Class */
// Arduino_DataBus *bus = create_default_Arduino_DataBus();
///* More display class: https://github.com/moononournation/Arduino_GFX/wiki/Display-Class */
// Arduino_GFX *gfx = new Arduino_ILI9341(bus, DF_GFX_RST, 3/* rotation */, false /* IPS */);

#define GFX_DEV_DEVICE ZX3D50CE02S
#define GFX_BL 45
Arduino_DataBus *bus = new Arduino_ESP32LCD8(
    0 /* DC */, GFX_NOT_DEFINED /* CS */, 47 /* WR */, GFX_NOT_DEFINED /* RD */,
    9 /* D0 */, 46 /* D1 */, 3 /* D2 */, 8 /* D3 */, 18 /* D4 */, 17 /* D5 */, 16 /* D6 */, 15 /* D7 */);
Arduino_GFX *gfx = new Arduino_ST7796(bus, 4 /* RST */, 3 /* rotation */, true /* IPS */);

#include <JPEGDEC.h>
JPEGDEC jpegdec;

#include "esp32_audio.h"
// microSD card
#define SD_SCK 39
#define SD_MISO 38
#define SD_MOSI 40
#define SD_CS 41
// I2S
#define I2S_DOUT 37
#define I2S_BCLK 36
#define I2S_LRCK 35

// pixel drawing callback
static int drawMCU(JPEGDRAW *pDraw)
{
  // Serial.printf("Draw pos = (%d, %d), size = %d x %d\n", pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
} /* drawMCU() */

void setup()
{
  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  // while(!Serial);
  Serial.println("AviPcmu8Mjpeg");

  gfx->begin();
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
    Serial.println(F("ERROR: File system mount failed!"));
  }
  else
  {
    a = AVI_open_input_file(avi_file, 1);

    frames = AVI_video_frames(a);
    w = AVI_video_width(a);
    h = AVI_video_height(a);
    fr = AVI_frame_rate(a);
    compressor = AVI_video_compressor(a);
    estimateBufferSize = w * h * 2 / 7;
    Serial.printf("AVI frames: %d, %d x %d @ %.2f fps, format: %s, estimateBufferSize: %d\n", frames, w, h, fr, compressor, estimateBufferSize);

    aChans = AVI_audio_channels(a);
    aBits = AVI_audio_bits(a);
    aFormat = AVI_audio_format(a);
    aRate = AVI_audio_rate(a);
    aBytes = AVI_audio_bytes(a);
    aChunks = AVI_audio_chunks(a);
    Serial.printf("Audio channels: %d, bits: %d, format: %d, rate: %d, bytes: %d, chunks: %d\n", aChans, aBits, aFormat, aRate, aBytes, aChunks);

    vidbuf = (char *)malloc(estimateBufferSize);
    audio_feed_per_frame = aRate / fr;
    audbuf = (char *)malloc(audio_feed_per_frame * 4);

    i2s_init(I2S_NUM_0,
             aRate /* sample_rate */,
             -1 /* mck_io_num */,  /*!< MCK in out pin. Note that ESP32 supports setting MCK on GPIO0/GPIO1/GPIO3 only*/
             I2S_BCLK,             /*!< BCK in out pin*/
             I2S_LRCK,             /*!< WS in out pin*/
             I2S_DOUT,             /*!< DATA out pin*/
             -1 /* data_in_num */, /*!< DATA in pin*/
             audio_feed_per_frame);
    i2s_zero_dma_buffer(I2S_NUM_0);

    isStopped = false;
    start_ms = millis();
    next_frame_ms = start_ms + ((curr_frame + 1) * 1000 / fr);
  }
}

void loop()
{
  if (!isStopped)
  {
    if (curr_frame < frames)
    {
      int len = AVI_read_audio(a, audbuf, audio_feed_per_frame);
      audioFeed(audbuf, len, 5);
      if (curr_frame == 0)
      {
        len = AVI_read_audio(a, audbuf, audio_feed_per_frame);
        audioFeed(audbuf, len, 5);
      }

      if (millis() < next_frame_ms) // check show frame or skip frame
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
          jpegdec.openRAM((uint8_t *)vidbuf, actual_video_size, drawMCU);
          jpegdec.setPixelType(RGB565_BIG_ENDIAN);
          jpegdec.decode(0, 0, 0);
          jpegdec.close();
        }
        while (millis() < next_frame_ms)
        {
          vTaskDelay(pdMS_TO_TICKS(1));
        }
      }
      else
      {
        ++skipped_frames;
        // Serial.printf("Skip frame %d > %d\n", millis(), next_frame_ms);
      }

      // Serial.printf("frame: %d, iskeyframe: %d, video_bytes: %d, actual_video_size: %d, audio_bytes: %d\n", curr_frame, iskeyframe, video_bytes, actual_video_size, audio_bytes);

      ++curr_frame;
      next_frame_ms = start_ms + ((curr_frame + 1) * 1000 / fr);
    }
    else
    {
      i2s_zero_dma_buffer(I2S_NUM_0);
      AVI_close(a);
      isStopped = true;
      Serial.printf("Duration: %d, skipped frames: %d\n", millis() - start_ms, skipped_frames);
    }
  }
  else
  {
    delay(100);
  }
}
