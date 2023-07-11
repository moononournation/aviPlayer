#include "driver/i2s.h"

static esp_err_t i2s_init(i2s_port_t i2s_num, uint32_t sample_rate,
                          int mck_io_num,   /*!< MCK in out pin. Note that ESP32 supports setting MCK on GPIO0/GPIO1/GPIO3 only*/
                          int bck_io_num,   /*!< BCK in out pin*/
                          int ws_io_num,    /*!< WS in out pin*/
                          int data_out_num, /*!< DATA out pin*/
                          int data_in_num,   /*!< DATA in pin*/
                          int audio_feed_per_frame
)
{
    int dma_buf_len = 512;
    int dma_buf_count = (audio_feed_per_frame + (dma_buf_len - 1)) / dma_buf_len * 6;

    esp_err_t ret_val = ESP_OK;

    i2s_config_t i2s_config;
    i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
    i2s_config.sample_rate = sample_rate;
    i2s_config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
    i2s_config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
    i2s_config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
    i2s_config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
    i2s_config.dma_buf_count = dma_buf_count;
    i2s_config.dma_buf_len = dma_buf_len;
    i2s_config.use_apll = false;
    i2s_config.tx_desc_auto_clear = true;
    i2s_config.fixed_mclk = 0;
    i2s_config.mclk_multiple = I2S_MCLK_MULTIPLE_DEFAULT;
    i2s_config.bits_per_chan = I2S_BITS_PER_CHAN_16BIT;

    i2s_pin_config_t pin_config;
    pin_config.mck_io_num = mck_io_num;
    pin_config.bck_io_num = bck_io_num;
    pin_config.ws_io_num = ws_io_num;
    pin_config.data_out_num = data_out_num;
    pin_config.data_in_num = data_in_num;

    ret_val |= i2s_driver_install(i2s_num, &i2s_config, 0, NULL);
    ret_val |= i2s_set_pin(i2s_num, &pin_config);

    return ret_val;
}

static void audioFeed(char *audbuf, size_t len, uint8_t gain_level)
{
    size_t l = len;
    uint16_t b;
    uint16_t *p = (uint16_t *)(audbuf + (len << 2) - 1);
    audbuf += len - 1;
    while (l--)
    {
        b = *audbuf--;
        b <<= gain_level;
        *p-- = b;
        *p-- = b;
    }
    size_t i2s_bytes_written = 0;
    i2s_write(I2S_NUM_0, p, len << 2, &i2s_bytes_written, portMAX_DELAY);
    // log_d("len: %d, i2s_bytes_written: %d", len, i2s_bytes_written);
}
