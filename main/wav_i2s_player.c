/**
 * @file wav_i2s_player.c
 * @brief example wav player with i2s for lyrat 4.3
 * @author Assouline Yohann
 * @date 2022-01-18
 */

#define DR_WAV_IMPLEMENTATION
#include "lib/dr_libs/dr_wav.h"
//

#include "board.h"
#include "driver/i2s.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_peripherals.h"
#include "periph_sdcard.h"

#include <stddef.h>
#include <stdlib.h>

void app_main(void)
{
    // init sd
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    audio_board_sdcard_init(set, SD_MODE_1_LINE);

    // init codec
    audio_board_handle_t handle = audio_board_init();
    audio_hal_ctrl_codec(handle->audio_hal, AUDIO_HAL_CODEC_MODE_DECODE, AUDIO_HAL_CTRL_START);

    // load wav file
    drwav wav;
    if (!drwav_init_file(&wav, "/sdcard/my_sound.wav", NULL)) {
        ESP_ERROR_CHECK(ESP_FAIL);
    }

    const size_t sampleDataSize = (size_t)wav.totalPCMFrameCount * wav.channels * sizeof(int32_t);
    int32_t *pSampleData = (int32_t *)malloc(sampleDataSize);
    drwav_read_pcm_frames_s32(&wav, wav.totalPCMFrameCount, pSampleData);

    // play pcm data with i2s
    size_t written;
    i2s_write(I2S_NUM_0, pSampleData, sampleDataSize, &written, portMAX_DELAY);

    drwav_uninit(&wav);
}
