/*******************************************************************************
 * AVI Player example
 *
 * Dependent libraries:
 * avilib: https://github.com/lanyou1900/avilib.git
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
char *avi_filename = (char *)"/root/AviMp3Cinepak240p30fps.avi";
// char *avi_filename = (char *)"/root/AviMp3Cinepak272p30fps.avi";
// char *avi_filename = (char *)"/root/AviMp3Cinepak400p10fps.avi";
// char *avi_filename = (char *)"/root/AviMp3Mjpeg240p15fps.avi";
// char *avi_filename = (char *)"/root/AviMp3Mjpeg272p15fps.avi";
// char *avi_filename = (char *)"/root/AviPcmu8Mjpeg240p15fps.avi";
// char *avi_filename = (char *)"/root/AviPcmu8Mjpeg272p15fps.avi";

#include "T_DECK.h"

#include <FFat.h>
#include <LittleFS.h>
#include <SPIFFS.h>
#include <SD.h>
#include <SD_MMC.h>

extern "C"
{
#include <avilib.h>
}

/* variables */
avi_t *a;
long frames, estimateBufferSize, aRate, aBytes, aChunks, actual_video_size;
long w, h, aChans, aBits, aFormat;
double fr;
char *compressor;
char *vidbuf;
char *audbuf;
bool isStopped = true;
long curr_frame = 0;
unsigned long start_ms;

void setup()
{
  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  // while(!Serial);
  Serial.println("AviDryRun");

#ifdef PERI_POWERON
  pinMode(PERI_POWERON, OUTPUT);
  digitalWrite(PERI_POWERON, HIGH);
#endif

#ifdef SPI_CS_1
  pinMode(SPI_CS_1, OUTPUT);
  digitalWrite(SPI_CS_1, HIGH);
#endif

#ifdef SPI_CS_2
  pinMode(SPI_CS_2, OUTPUT);
  digitalWrite(SPI_CS_2, HIGH);
#endif

  // If display and SD shared same interface, init SPI first
#ifdef SPI_SCK
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
#endif

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
    a = AVI_open_input_file(avi_filename, 1);

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

      vidbuf = (char *)heap_caps_malloc(estimateBufferSize, MALLOC_CAP_8BIT);
      if (!vidbuf)
      {
        Serial.println("vidbuf heap_caps_malloc failed!");
      }

      audbuf = (char *)heap_caps_malloc(1024, MALLOC_CAP_8BIT);
      if (!audbuf)
      {
        Serial.println("audbuf heap_caps_malloc failed!");
      }

      isStopped = false;
      start_ms = millis();
    }
  }
}

void loop()
{
  if (!isStopped)
  {
    if (curr_frame < frames)
    {
      long audio_bytes = AVI_audio_size(a, curr_frame);
      AVI_read_audio(a, audbuf, audio_bytes);

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
      }

      // Serial.printf("frame: %d, iskeyframe: %d, video_bytes: %d, actual_video_size: %d, audio_bytes: %d\n", curr_frame, iskeyframe, video_bytes, actual_video_size, audio_bytes);

      curr_frame++;
    }
    else
    {
      AVI_close(a);
      isStopped = true;
      Serial.printf("Duration: %lu\n", millis() - start_ms);
    }
  }
  else
  {
    delay(100);
  }
}
