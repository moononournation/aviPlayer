/***
 * Required libraries:
 * avilib: https://github.com/lanyou1900/avilib.git
 */

const char *root = "/root";
// const char *avi_file = "/root/AviPcmu8Cinepak240p15fps.avi";
const char *avi_file = "/root/AviPcmu8Mjpeg240p15fps.avi";
// const char *avi_file = "/root/AviPcmu8Mjpeg320p10fps.avi";

#include <WiFi.h>

#include <FFat.h>
#include <LittleFS.h>
#include <SD_MMC.h>

extern "C"
{
#include <avilib.h>
}

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
static unsigned long start_ms;

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
  Serial.println("AviDryRun");

  if (!FFat.begin(false, root))
  // if (!LittleFS.begin(false, root))
  // pinMode(SD_CS /* CS */, OUTPUT);
  // digitalWrite(SD_CS /* CS */, HIGH);
  // SD_MMC.setPins(SD_SCK /* CLK */, SD_MOSI /* CMD/MOSI */, SD_MISO /* D0/MISO */);
  // if (!SD_MMC.begin(root, true /* mode1bit */, false /* format_if_mount_failed */, SDMMC_FREQ_DEFAULT))
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
