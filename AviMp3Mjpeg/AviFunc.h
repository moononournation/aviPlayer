extern "C"
{
#include <avilib.h>
}

#include "esp32_audio.h"

#include <ESP32_JPEG_Library.h>
jpeg_dec_handle_t *jpeg_dec;
jpeg_dec_io_t *jpeg_io;
jpeg_dec_header_info_t *out_info;

/* variables */
avi_t *avi;
long avi_total_frames, estimateBufferSize, avi_aRate, avi_aBytes, avi_aChunks, actual_video_size;
long avi_w, avi_h, avi_aChans, avi_aBits, avi_aFormat;
double avi_fr;
char *avi_compressor;
char *vidbuf;
long avi_curr_frame;
long avi_skipped_frames;
bool avi_skipped_last_frame;
unsigned long avi_start_ms, avi_next_frame_ms;
unsigned long avi_total_read_video_ms;
unsigned long avi_total_decode_video_ms;
unsigned long avi_total_show_video_ms;

bool avi_init()
{
  estimateBufferSize = gfx->width() * gfx->height() * 2 / 5;
  vidbuf = (char *)heap_caps_malloc(estimateBufferSize, MALLOC_CAP_8BIT);
  if (!vidbuf)
  {
    Serial.println("vidbuf heap_caps_malloc failed!");
    return false;
  }

  audbuf = (char *)heap_caps_malloc(MP3_MAX_FRAME_SIZE, MALLOC_CAP_8BIT);
  if (!audbuf)
  {
    Serial.println("audbuf heap_caps_malloc failed!");
    return false;
  }

  // Generate default configuration
  jpeg_dec_config_t config = {
      .output_type = JPEG_RAW_TYPE_RGB565_BE,
      .rotate = JPEG_ROTATE_0D,
  };
  // Create jpeg_dec
  jpeg_dec = jpeg_dec_open(&config);

  // Create io_callback handle
  jpeg_io = (jpeg_dec_io_t *)calloc(1, sizeof(jpeg_dec_io_t));

  // Create out_info handle
  out_info = (jpeg_dec_header_info_t *)calloc(1, sizeof(jpeg_dec_header_info_t));

  return true;
}

bool avi_open(char *avi_filename)
{
  avi = AVI_open_input_file(avi_filename, 1);

  if (!avi)
  {
    Serial.printf("AVI_open_input_file %s failed!\n", avi_filename);
    return false;
  }

  avi_total_frames = AVI_video_frames(avi);
  avi_w = AVI_video_width(avi);
  avi_h = AVI_video_height(avi);
  avi_fr = AVI_frame_rate(avi);
  avi_compressor = AVI_video_compressor(avi);
  Serial.printf("AVI avi_total_frames: %ld, %ld x %ld @ %.2f fps, format: %s, estimateBufferSize: %ld, ESP.getFreeHeap(): %ld\n", avi_total_frames, avi_w, avi_h, avi_fr, avi_compressor, estimateBufferSize, (long)ESP.getFreeHeap());

  avi_aChans = AVI_audio_channels(avi);
  avi_aBits = AVI_audio_bits(avi);
  avi_aFormat = AVI_audio_format(avi);
  avi_aRate = AVI_audio_rate(avi);
  avi_aBytes = AVI_audio_bytes(avi);
  avi_aChunks = AVI_audio_chunks(avi);
  Serial.printf("Audio channels: %ld, bits: %ld, format: %ld, rate: %ld, bytes: %ld, chunks: %ld\n", avi_aChans, avi_aBits, avi_aFormat, avi_aRate, avi_aBytes, avi_aChunks);

  audbuf_remain = 0;
  avi_curr_frame = 0;
  avi_skipped_frames = 0;

  avi_total_read_video_ms = 0;
  avi_total_decode_video_ms = 0;
  avi_total_show_video_ms = 0;

  return true;
}

void avi_feed_audio()
{
  if (audbuf_remain == 0)
  {
    unsigned long curr_ms = millis();
    audbuf_read = AVI_read_audio(avi, audbuf, MP3_MAX_FRAME_SIZE);
    audbuf_remain = audbuf_read;
    total_read_audio_ms += millis() - curr_ms;
  }
}

void avi_draw(int x, int y)
{
  if (millis() < avi_next_frame_ms) // check show frame or skip frame
  {
    AVI_set_video_position(avi, avi_curr_frame);

    int iskeyframe;
    long video_bytes = AVI_frame_size(avi, avi_curr_frame);
    if (video_bytes > estimateBufferSize)
    {
      Serial.printf("video_bytes(%ld) > estimateBufferSize(%ld)\n", video_bytes, estimateBufferSize);
    }
    else
    {
      unsigned long curr_ms = millis();
      actual_video_size = AVI_read_frame(avi, vidbuf, &iskeyframe);
      avi_total_read_video_ms += millis() - curr_ms;
      // Serial.printf("frame: %ld, iskeyframe: %ld, video_bytes: %ld, actual_video_size: %ld, audio_bytes: %ld, ESP.getFreeHeap(): %ld\n", avi_curr_frame, iskeyframe, video_bytes, actual_video_size, audio_bytes, (long)ESP.getFreeHeap());

      curr_ms = millis();
      if ((!avi_skipped_last_frame) || iskeyframe)
      {
        avi_skipped_last_frame = false;

        jpeg_io->inbuf = (uint8_t *)vidbuf;
        jpeg_io->inbuf_len = actual_video_size;

        jpeg_dec_parse_header(jpeg_dec, jpeg_io, out_info);

        jpeg_io->outbuf = (uint8_t *)output_buf;

        jpeg_dec_process(jpeg_dec, jpeg_io);

        avi_total_decode_video_ms += millis() - curr_ms;

        curr_ms = millis();
#ifdef RGB_PANEL
        gfx->flush();
#else
        gfx->draw16bitBeRGBBitmap(x, y, output_buf, avi_w, avi_h);
#endif
        avi_total_show_video_ms += millis() - curr_ms;
      }
      else
      {
        ++avi_skipped_frames;
      }
    }
    while (millis() < avi_next_frame_ms)
    {
      vTaskDelay(pdMS_TO_TICKS(1));
    }
  }
  else
  {
    ++avi_skipped_frames;
    avi_skipped_last_frame = true;
    // Serial.printf("Skip frame %ld > %ld\n", millis(), avi_next_frame_ms);
  }

  ++avi_curr_frame;
  avi_next_frame_ms = avi_start_ms + ((avi_curr_frame + 1) * 1000 / avi_fr);
}

void avi_close()
{
  // AVI_close(avi);
  // jpeg_dec_close(jpeg_dec);
  audbuf_read = 0;
}