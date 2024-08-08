#include "driver/i2s.h"

#include "MP3DecoderHelix.h"

#define I2S_OUTPUT_NUM I2S_NUM_0
#define I2S_DEFAULT_SAMPLE_RATE 480000

char *audbuf;
size_t audbuf_read;
size_t audbuf_remain;
unsigned long total_read_audio_ms;
unsigned long total_decode_audio_ms;
unsigned long total_play_audio_ms;

int _samprate = 0;
void audioDataCallback(MP3FrameInfo &info, int16_t *pwm_buffer, size_t len, void *ref)
{
  unsigned long s = millis();
  if (_samprate != info.samprate)
  {
    Serial.printf("bitrate: %d, nChans: %d, samprate: %d, bitsPerSample: %d, outputSamps: %d, layer: %d, version: %d\n",
                  info.bitrate, info.nChans, info.samprate, info.bitsPerSample, info.outputSamps, info.layer, info.version);
    i2s_set_clk(I2S_OUTPUT_NUM, info.samprate /* sample_rate */, info.bitsPerSample /* bits_cfg */, (info.nChans == 2) ? I2S_CHANNEL_STEREO : I2S_CHANNEL_MONO /* channel */);
    _samprate = info.samprate;
  }
  for (int i = 0; i < len; i++)
  {
    pwm_buffer[i] = pwm_buffer[i] * I2S_DEFAULT_GAIN_LEVEL;
  }
  size_t i2s_bytes_written = 0;
  i2s_write(I2S_OUTPUT_NUM, pwm_buffer, len * 2, &i2s_bytes_written, portMAX_DELAY);
  // Serial.printf("len: %d, i2s_bytes_written: %d\n", len, i2s_bytes_written);
  total_play_audio_ms += millis() - s;
}

libhelix::MP3DecoderHelix mp3(audioDataCallback);

esp_err_t i2s_init()
{
  esp_err_t ret_val = ESP_OK;

  i2s_config_t i2s_config;
  i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
  i2s_config.sample_rate = I2S_DEFAULT_SAMPLE_RATE;
  i2s_config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
  i2s_config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
  i2s_config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
  i2s_config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
  i2s_config.dma_buf_count = 32;
  i2s_config.dma_buf_len = 320;
  i2s_config.use_apll = false;
  i2s_config.tx_desc_auto_clear = true;
  i2s_config.fixed_mclk = 0;
  i2s_config.mclk_multiple = I2S_MCLK_MULTIPLE_128;
  i2s_config.bits_per_chan = I2S_BITS_PER_CHAN_16BIT;

  i2s_pin_config_t pin_config;
  pin_config.mck_io_num = I2S_MCLK;
  pin_config.bck_io_num = I2S_BCLK;
  pin_config.ws_io_num = I2S_LRCK;
  pin_config.data_out_num = I2S_DOUT;
  pin_config.data_in_num = I2S_DIN;

  ret_val |= i2s_driver_install(I2S_OUTPUT_NUM, &i2s_config, 0, NULL);
  ret_val |= i2s_set_pin(I2S_OUTPUT_NUM, &pin_config);

  i2s_zero_dma_buffer(I2S_OUTPUT_NUM);

  return ret_val;
}

void mp3_player_task(void *pvParam)
{
  unsigned long ms;
  char *p;
  long w;

  Serial.printf("mp3_player_task start\n");
  mp3.begin();
  do
  {
    ms = millis();

    p = audbuf;
    while (audbuf_remain > 0)
    {
      w = mp3.write(p, audbuf_remain);
      // Serial.printf("r: %d, w: %d\n", r, w);
      audbuf_remain -= w;
      p += w;
    }

    total_decode_audio_ms += millis() - ms;

    vTaskDelay(pdMS_TO_TICKS(1));
  } while (audbuf_read > 0);

  Serial.printf("mp3_player_task stop\n");
  mp3.end();

  i2s_zero_dma_buffer(I2S_OUTPUT_NUM);
  vTaskDelete(NULL);
}

BaseType_t mp3_player_task_start()
{
  total_read_audio_ms = 0;
  total_decode_audio_ms = 0;
  total_play_audio_ms = 0;

  return xTaskCreatePinnedToCore(
      (TaskFunction_t)mp3_player_task,
      (const char *const)"MP3 Player Task",
      (const uint32_t)2000,
      (void *const)NULL,
      (UBaseType_t)configMAX_PRIORITIES - 1,
      (TaskHandle_t *const)NULL,
      (const BaseType_t)0);
}
