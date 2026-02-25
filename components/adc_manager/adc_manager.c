#include "adc_manager.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"

#define ENGINE_ADC_CHANNEL ADC_CHANNEL_6  // GPIO34
#define LIGHT_ADC_CHANNEL  ADC_CHANNEL_7  // GPIO35

static const char *TAG = "ADC_MANAGER";

static adc_oneshot_unit_handle_t adc_handle;

static int engine_raw = 0;
static int light_raw  = 0;

void adc_manager_init(void)
{
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_new_unit(&init_config, &adc_handle);

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };

    adc_oneshot_config_channel(adc_handle, ENGINE_ADC_CHANNEL, &config);
    adc_oneshot_config_channel(adc_handle, LIGHT_ADC_CHANNEL, &config);

    ESP_LOGI(TAG, "ADC Initialized (ESP-IDF 5.x)");
}

void adc_manager_update(void)
{
    adc_oneshot_read(adc_handle, ENGINE_ADC_CHANNEL, &engine_raw);
    adc_oneshot_read(adc_handle, LIGHT_ADC_CHANNEL, &light_raw);

    float engine_v = engine_raw * 3.3 / 4095.0;
    float light_v  = light_raw  * 3.3 / 4095.0;

    ESP_LOGI(TAG,
        "Engine: %.2fV | Light: %.2fV",
        engine_v, light_v);
}