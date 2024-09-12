#define AVI_SUPPORT_CINEPAK
#define AVI_SUPPORT_MJPEG
#define AVI_SUPPORT_AUDIO

extern "C"
{
#include <avilib.h>
}

#define SKIP_FRAME_TOLERANT_MS 250

#define MAX_AUDIO_FRAME_SIZE 1024 * 3

#define UNKNOWN_CODEC_CODE -1
#define PCM_CODEC_CODE 1
#define MP3_CODEC_CODE 85
#define CINEPAK_CODEC_CODE 1001
#define MJPEG_CODEC_CODE 1002

#ifdef AVI_SUPPORT_CINEPAK
#include "cinepak.h"
CinepakDecoder cinepak;
#endif // AVI_SUPPORT_CINEPAK

#ifdef AVI_SUPPORT_MJPEG
#include <ESP32_JPEG_Library.h>
jpeg_dec_handle_t *jpeg_dec;
jpeg_dec_io_t *jpeg_io;
jpeg_dec_header_info_t *out_info;
#endif // AVI_SUPPORT_MJPEG

/* variables */
avi_t *avi;
long avi_total_frames, estimateBufferSize, avi_aRate, avi_aBytes, avi_aChunks, actual_video_size;
long avi_w, avi_h, avi_aChans, avi_aBits, avi_aFormat;
double avi_fr;
char *avi_compressor;
long avi_vcodec;
char *vidbuf;
long avi_curr_frame;
int avi_curr_is_key_frame;
long avi_skipped_frames;
unsigned long avi_start_ms, avi_next_frame_ms, avi_skip_frame_ms;
unsigned long avi_total_read_video_ms;
unsigned long avi_total_decode_video_ms;
unsigned long avi_total_show_video_ms;

#ifdef AVI_SUPPORT_AUDIO
char *audbuf;
size_t audbuf_read;
size_t audbuf_remain;
unsigned long avi_total_read_audio_ms;
unsigned long total_decode_audio_ms;
unsigned long total_play_audio_ms;
#endif // AVI_SUPPORT_AUDIO

bool avi_init()
{
  estimateBufferSize = gfx->width() * gfx->height() * 2 / 5;
  vidbuf = (char *)heap_caps_malloc(estimateBufferSize, MALLOC_CAP_8BIT);
  if (!vidbuf)
  {
    Serial.println("vidbuf heap_caps_malloc failed!");
    return false;
  }

#ifdef AVI_SUPPORT_AUDIO
  audbuf = (char *)heap_caps_malloc(MAX_AUDIO_FRAME_SIZE, MALLOC_CAP_8BIT);
  if (!audbuf)
  {
    Serial.println("audbuf heap_caps_malloc failed!");
    return false;
  }
#endif // AVI_SUPPORT_AUDIO

#ifdef AVI_SUPPORT_MJPEG
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
#endif // AVI_SUPPORT_MJPEG

  return true;
}

bool avi_open(char *avi_filename)
{
  Serial.printf("avi_open(%s)\n", avi_filename);
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
  if (strcmp(avi_compressor, "    ") == 0)
  {
    avi_vcodec = UNKNOWN_CODEC_CODE;
  }
#ifdef AVI_SUPPORT_CINEPAK
  else if (strcmp(avi_compressor, "cvid") == 0)
  {
    avi_vcodec = CINEPAK_CODEC_CODE;
  }
#endif // AVI_SUPPORT_CINEPAK
#ifdef AVI_SUPPORT_MJPEG
  else if (strcmp(avi_compressor, "MJPG") == 0)
  {
    avi_vcodec = MJPEG_CODEC_CODE;
  }
#endif // AVI_SUPPORT_MJPEG
  else
  {
    avi_vcodec = UNKNOWN_CODEC_CODE;
  }
  Serial.printf("AVI avi_total_frames: %ld, %ld x %ld @ %.2f fps, format: %s, estimateBufferSize: %ld, ESP.getFreeHeap(): %ld\n", avi_total_frames, avi_w, avi_h, avi_fr, avi_compressor, estimateBufferSize, (long)ESP.getFreeHeap());

  avi_aChans = AVI_audio_channels(avi);
  avi_aBits = AVI_audio_bits(avi);
  avi_aFormat = AVI_audio_format(avi);
  avi_aRate = AVI_audio_rate(avi);
  avi_aBytes = AVI_audio_bytes(avi);
  avi_aChunks = AVI_audio_chunks(avi);
  Serial.printf("Audio channels: %ld, bits: %ld, format: %ld, rate: %ld, bytes: %ld, chunks: %ld\n", avi_aChans, avi_aBits, avi_aFormat, avi_aRate, avi_aBytes, avi_aChunks);

  avi_curr_frame = 0;
  avi_skipped_frames = 0;

  avi_total_read_video_ms = 0;
  avi_total_decode_video_ms = 0;
  avi_total_show_video_ms = 0;

#ifdef AVI_SUPPORT_AUDIO
  audbuf_remain = 0;
  avi_total_read_audio_ms = 0;
  total_decode_audio_ms = 0;
  total_play_audio_ms = 0;
#endif // AVI_SUPPORT_AUDIO

  return true;
}

#ifdef AVI_SUPPORT_AUDIO
void avi_feed_audio()
{
  if (audbuf_remain == 0)
  {
    unsigned long curr_ms = millis();
    audbuf_read = AVI_read_audio(avi, audbuf, MAX_AUDIO_FRAME_SIZE);
    audbuf_remain = audbuf_read;
    avi_total_read_audio_ms += millis() - curr_ms;
  }
}
#endif // AVI_SUPPORT_AUDIO

bool avi_decode()
{
  avi_next_frame_ms = avi_start_ms + ((avi_curr_frame + 1) * 1000 / avi_fr);
  avi_skip_frame_ms = avi_next_frame_ms + SKIP_FRAME_TOLERANT_MS;

#ifdef AVI_SUPPORT_MJPEG
  if (
      (avi_vcodec == MJPEG_CODEC_CODE) && (millis() >= avi_skip_frame_ms) // MJPEG can direct skip decode frame
  )
  {
    // Serial.printf("Skipped frame %ld\n", avi_curr_frame);
    ++avi_curr_frame;
    ++avi_skipped_frames;
    return false;
  }
  else
#endif // AVI_SUPPORT_MJPEG
  {
    AVI_set_video_position(avi, avi_curr_frame);

    long video_bytes = AVI_frame_size(avi, avi_curr_frame);
    if (video_bytes > estimateBufferSize)
    {
      Serial.printf("video_bytes(%ld) > estimateBufferSize(%ld)\n", video_bytes, estimateBufferSize);
      ++avi_curr_frame;
      return false;
    }
    else
    {
      unsigned long curr_ms = millis();
      actual_video_size = AVI_read_frame(avi, vidbuf, &avi_curr_is_key_frame);
      avi_total_read_video_ms += millis() - curr_ms;
#ifdef AVI_SUPPORT_AUDIO
      // Serial.printf("frame: %ld, avi_curr_is_key_frame: %ld, video_bytes: %ld, actual_video_size: %ld, audio_bytes: %ld, ESP.getFreeHeap(): %ld\n", avi_curr_frame, avi_curr_is_key_frame, video_bytes, actual_video_size, audio_bytes, (long)ESP.getFreeHeap());
#else
      // Serial.printf("frame: %ld, avi_curr_is_key_frame: %ld, video_bytes: %ld, actual_video_size: %ld, ESP.getFreeHeap(): %ld\n", avi_curr_frame, avi_curr_is_key_frame, video_bytes, actual_video_size, (long)ESP.getFreeHeap());
#endif

      curr_ms = millis();
      if (avi_vcodec == UNKNOWN_CODEC_CODE)
      {
      }
#ifdef AVI_SUPPORT_CINEPAK
      else if (avi_vcodec == CINEPAK_CODEC_CODE)
      {
        cinepak.decodeFrame((uint8_t *)vidbuf, actual_video_size, output_buf, output_buf_size);
      }
#endif // AVI_SUPPORT_CINEPAK
#ifdef AVI_SUPPORT_MJPEG
      else if (avi_vcodec == MJPEG_CODEC_CODE)
      {
        jpeg_io->inbuf = (uint8_t *)vidbuf;
        jpeg_io->inbuf_len = actual_video_size;

        jpeg_dec_parse_header(jpeg_dec, jpeg_io, out_info);

        jpeg_io->outbuf = (uint8_t *)output_buf;

        jpeg_dec_process(jpeg_dec, jpeg_io);
      }
#endif // AVI_SUPPORT_MJPEG
      avi_total_decode_video_ms += millis() - curr_ms;

      ++avi_curr_frame;
      return true;
    }
  }
}

void avi_draw(int x, int y)
{
  if ((avi_vcodec == MJPEG_CODEC_CODE)   // always show decoded MJPEG frame
      || (millis() < avi_skip_frame_ms)) // skip lagging frame
  {
    unsigned long curr_ms = millis();
#ifdef RGB_PANEL
    gfx->flush();
#else
#ifdef CANVAS_R1
    g->draw16bitBeRGBBitmapR1(x, y, output_buf, avi_w, avi_h);
#else
    gfx->draw16bitBeRGBBitmap(x, y, output_buf, avi_w, avi_h);
#endif // #ifdef CANVAS_R1
#endif // #ifdef RGB_PANEL
    avi_total_show_video_ms += millis() - curr_ms;

    while (millis() < avi_next_frame_ms)
    {
      vTaskDelay(pdMS_TO_TICKS(1));
    }
  }
  else
  {
    ++avi_skipped_frames;
    // Serial.printf("Skip frame %ld > %ld\n", millis(), avi_next_frame_ms);
  }
}

void avi_close()
{
  AVI_close(avi);
  // if (avi_vcodec == MJPEG_CODEC_CODE)
  // {
  //   jpeg_dec_close(jpeg_dec);
  // }
#ifdef AVI_SUPPORT_AUDIO
  audbuf_read = 0;
#endif // AVI_SUPPORT_AUDIO
}

void avi_show_stat()
{
  int time_used = millis() - avi_start_ms;
  long played_frames = avi_total_frames - avi_skipped_frames;
  float fps = 1000.0 * played_frames / time_used;

  Serial.printf("Played avi_frames: %ld\n", played_frames);
  Serial.printf("Skipped avi_frames: %ld (%0.1f %%)\n", avi_skipped_frames, 100.0 * avi_skipped_frames / avi_total_frames);
  Serial.printf("Time used: %lu ms\n", time_used);
  Serial.printf("Expected FPS: %0.1f\n", avi_fr);
  Serial.printf("Actual FPS: %0.1f\n", fps);
  Serial.printf("Read video: %lu ms (%0.1f %%)\n", avi_total_read_video_ms, 100.0 * avi_total_read_video_ms / time_used);
  Serial.printf("Decode video: %lu ms (%0.1f %%)\n", avi_total_decode_video_ms, 100.0 * avi_total_decode_video_ms / time_used);
  Serial.printf("Show video: %lu ms (%0.1f %%)\n", avi_total_show_video_ms, 100.0 * avi_total_show_video_ms / time_used);
#ifdef AVI_SUPPORT_AUDIO
  Serial.printf("Read audio: %lu ms (%0.1f %%)\n", avi_total_read_audio_ms, 100.0 * avi_total_read_audio_ms / time_used);
  Serial.printf("Decode audio: %lu ms (%0.1f %%)\n", total_decode_audio_ms, 100.0 * total_decode_audio_ms / time_used);
  Serial.printf("Play audio: %lu ms (%0.1f %%)\n", total_play_audio_ms, 100.0 * total_play_audio_ms / time_used);
#endif // AVI_SUPPORT_AUDIO

#ifdef CANVAS
  gfx->draw16bitBeRGBBitmap(0, 0, output_buf, avi_w, avi_h);
#endif

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
  float arc_end1 = arc_start1 + max(2.0, 360.0 * avi_total_read_video_ms / time_used);
  for (int i = arc_start1 + 1; i < arc_end1; i += 2)
  {
    gfx->fillArc(cx, cy, r1, r2, arc_start1 - 90.0, i - 90.0, LEGEND_A_COLOR);
  }
  gfx->fillArc(cx, cy, r1, r2, arc_start1 - 90.0, arc_end1 - 90.0, LEGEND_A_COLOR);

  float arc_start2 = arc_end1;
  float arc_end2 = arc_start2 + max(2.0, 360.0 * avi_total_decode_video_ms / time_used);
  for (int i = arc_start2 + 1; i < arc_end2; i += 2)
  {
    gfx->fillArc(cx, cy, r1, r2, arc_start2 - 90.0, i - 90.0, LEGEND_B_COLOR);
  }
  gfx->fillArc(cx, cy, r1, r2, arc_start2 - 90.0, arc_end2 - 90.0, LEGEND_B_COLOR);

  float arc_start3 = arc_end2;
  float arc_end3 = arc_start3 + max(2.0, 360.0 * avi_total_show_video_ms / time_used);
  for (int i = arc_start3 + 1; i < arc_end3; i += 2)
  {
    gfx->fillArc(cx, cy, r1, r2, arc_start3 - 90.0, i - 90.0, LEGEND_C_COLOR);
  }
  gfx->fillArc(cx, cy, r1, r2, arc_start3 - 90.0, arc_end3 - 90.0, LEGEND_C_COLOR);

#ifdef AVI_SUPPORT_AUDIO
  float arc_start4 = arc_end3;
  float arc_end4 = arc_start4 + max(2.0, 360.0 * avi_total_read_audio_ms / time_used);
  for (int i = arc_start4 + 1; i < arc_end4; i += 2)
  {
    gfx->fillArc(cx, cy, r1, r2, arc_start4 - 90.0, i - 90.0, LEGEND_D_COLOR);
  }
  gfx->fillArc(cx, cy, r1, r2, arc_start4 - 90.0, arc_end4 - 90.0, LEGEND_D_COLOR);

  float arc_start5 = 0;
  float arc_end5 = arc_start5 + max(2.0, 360.0 * total_decode_audio_ms / time_used);
  for (int i = arc_start5 + 1; i < arc_end5; i += 2)
  {
    gfx->fillArc(cx, cy, r2, 0, arc_start5 - 90.0, i - 90.0, LEGEND_G_COLOR);
  }
  gfx->fillArc(cx, cy, r2, 0, arc_start5 - 90.0, arc_end5 - 90.0, LEGEND_G_COLOR);

  float arc_start6 = arc_end5;
  float arc_end6 = arc_start6 + max(2.0, 360.0 * total_play_audio_ms / time_used);
  for (int i = arc_start6 + 1; i < arc_end6; i += 2)
  {
    gfx->fillArc(cx, cy, r2, 0, arc_start6 - 90.0, i - 90.0, LEGEND_H_COLOR);
  }
  gfx->fillArc(cx, cy, r2, 0, arc_start6 - 90.0, arc_end6 - 90.0, LEGEND_H_COLOR);
#endif // AVI_SUPPORT_AUDIO

  gfx->setCursor(0, 0);
  gfx->setTextColor(WHITE, BLACK);
  gfx->printf("Played avi_frames: %d\n", played_frames);
  gfx->printf("Skipped avi_frames: %ld (%0.1f %%)\n", avi_skipped_frames, 100.0 * avi_skipped_frames / avi_total_frames);
  gfx->printf("Time used: %d ms\n", time_used);
  gfx->printf("Expected FPS: %0.1f\n", avi_fr);
  gfx->printf("Actual FPS: %0.1f\n\n", fps);
  gfx->setTextColor(LEGEND_A_COLOR, BLACK);
  gfx->printf("Read video: %lu ms (%0.1f %%)\n", avi_total_read_video_ms, 100.0 * avi_total_read_video_ms / time_used);
  gfx->setTextColor(LEGEND_B_COLOR, BLACK);
  gfx->printf("Decode video: %lu ms (%0.1f %%)\n", avi_total_decode_video_ms, 100.0 * avi_total_decode_video_ms / time_used);
  gfx->setTextColor(LEGEND_C_COLOR, BLACK);
  gfx->printf("Show video: %lu ms (%0.1f %%)\n", avi_total_show_video_ms, 100.0 * avi_total_show_video_ms / time_used);
#ifdef AVI_SUPPORT_AUDIO
  gfx->setTextColor(LEGEND_D_COLOR, BLACK);
  gfx->printf("Read audio: %lu ms (%0.1f %%)\n", avi_total_read_audio_ms, 100.0 * avi_total_read_audio_ms / time_used);
  gfx->setTextColor(LEGEND_G_COLOR, BLACK);
  gfx->printf("Decode audio: %lu ms (%0.1f %%)\n", total_decode_audio_ms, 100.0 * total_decode_audio_ms / time_used);
  gfx->setTextColor(LEGEND_H_COLOR, BLACK);
  gfx->printf("Play audio: %lu ms (%0.1f %%)\n", total_play_audio_ms, 100.0 * total_play_audio_ms / time_used);
#endif // AVI_SUPPORT_AUDIO

#ifdef RGB_PANEL
  gfx->flush();
#endif

#ifdef CANVAS
  gfx->flush();
#endif
}
