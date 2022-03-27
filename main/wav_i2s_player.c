/**
 * @file wav_i2s_player.c
 * @brief example wav player with i2s for esp32 lyrat
 * @author Assouline Yohann
 * @date 2022-01-18
 */

#define DR_WAV_IMPLEMENTATION
#include "lib/dr_libs/dr_wav.h"

// adf
#include "board.h"

// idf
#include "driver/i2s.h"
#include "driver/sdmmc_host.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

// std
#include <stddef.h>
#include <stdlib.h>

static const char *TAG = "WAV_I2S_PLAYER";

#define WAV_SAMPLE_RATE 44100
#define PATH_TO_WAV_FILE "/sdcard/sample.wav"

static void init_sdcard(void)
{
    sdmmc_card_t *card;

    const esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 15,
        .allocation_unit_size = 16 * 1024,
    };

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();

    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.width = 1;

    gpio_set_pull_mode(GPIO_NUM_15, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_NUM_2, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_NUM_4, GPIO_PULLUP_ONLY);

    if (esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SD card");
        ESP_ERROR_CHECK(ESP_FAIL);
    }

    ESP_LOGI(TAG, "SD card initialization OK");

    sdmmc_card_print_info(stdout, card);
}

static void init_i2s(void)
{
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = WAV_SAMPLE_RATE,
        .bits_per_sample = 16,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .intr_alloc_flags = ESP_INTR_FLAG_INTRDISABLED,
        .dma_buf_count = 8,
        .dma_buf_len = 1024,
        .use_apll = 1,
        .tx_desc_auto_clear = 1,
        .fixed_mclk = 0,
    };

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);

    i2s_pin_config_t pin_config = {
        .bck_io_num = GPIO_NUM_5,
        .ws_io_num = GPIO_NUM_25,
        .data_out_num = GPIO_NUM_26,
        .data_in_num = GPIO_NUM_35,
    };

    ESP_ERROR_CHECK(i2s_set_pin(I2S_NUM_0, &pin_config));

    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
    WRITE_PERI_REG(PIN_CTRL, 0xFFF0);
}

void play_wav(const char *fp)
{
    drwav wav;
    if (!drwav_init_file(&wav, fp, NULL)) {
        ESP_LOGE(TAG, "%s: File not found, add it on SD card", fp);
        return;
        ESP_ERROR_CHECK(ESP_FAIL);
    }

    const int NB_SAMPLES_TO_READ = 1000;

    const size_t sampleDataSize = NB_SAMPLES_TO_READ * wav.channels * sizeof(int16_t);
    int16_t sampleData[sampleDataSize];

    while (drwav_read_pcm_frames_s16le(&wav, NB_SAMPLES_TO_READ, sampleData) == NB_SAMPLES_TO_READ) {
        size_t written;
        i2s_write(I2S_NUM_0, sampleData, sampleDataSize, &written, portMAX_DELAY);
        ESP_LOGV(TAG, "Write %zu bytes", written);
    }
    drwav_uninit(&wav);
}

void app_main(void)
{
    audio_board_handle_t board_handle = audio_board_init();
    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_DECODE, AUDIO_HAL_CTRL_START);

    init_i2s();
    init_sdcard();
    play_wav(PATH_TO_WAV_FILE);
    i2s_stop(I2S_NUM_0);
}
