// #include "adc_manager.h"
// #include "esp_log.h"
// #include "esp_adc/adc_oneshot.h"

// #define ENGINE_ADC_CHANNEL ADC_CHANNEL_6  // GPIO34
// #define LIGHT_ADC_CHANNEL  ADC_CHANNEL_7  // GPIO35

// static const char *TAG = "ADC_MANAGER";

// static adc_oneshot_unit_handle_t adc_handle;

// static int engine_raw = 0;
// static int light_raw  = 0;

// void adc_manager_init(void)
// {
//     adc_oneshot_unit_init_cfg_t init_config = {
//         .unit_id = ADC_UNIT_1,
//     };
//     adc_oneshot_new_unit(&init_config, &adc_handle);

//     adc_oneshot_chan_cfg_t config = {
//         .bitwidth = ADC_BITWIDTH_DEFAULT,
//         .atten = ADC_ATTEN_DB_12,
//     };

//     adc_oneshot_config_channel(adc_handle, ENGINE_ADC_CHANNEL, &config);
//     adc_oneshot_config_channel(adc_handle, LIGHT_ADC_CHANNEL, &config);

//     ESP_LOGI(TAG, "ADC Initialized (ESP-IDF 5.x)");
// }

// void adc_manager_update(void)
// {
//     adc_oneshot_read(adc_handle, ENGINE_ADC_CHANNEL, &engine_raw);
//     adc_oneshot_read(adc_handle, LIGHT_ADC_CHANNEL, &light_raw);

//     float engine_v = engine_raw * 3.3 / 4095.0;
//     float light_v  = light_raw  * 3.3 / 4095.0;

//     ESP_LOGI(TAG,
//         "Engine: %.2fV | Light: %.2fV",
//         engine_v, light_v);
// }

#include "adc_manager.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"

#define ENGINE_ADC_CHANNEL ADC_CHANNEL_6  // GPIO34
#define LIGHT_ADC_CHANNEL  ADC_CHANNEL_7  // GPIO35

#define SAMPLE_COUNT 16

// 根据你分压比约 0.2
// 13V ≈ 2.6V
// 12V ≈ 2.4V

#define ENGINE_ON_THRESHOLD   2.6f
#define ENGINE_OFF_THRESHOLD  2.4f

#define LIGHT_ON_THRESHOLD    2.5f
#define LIGHT_OFF_THRESHOLD   2.2f

static const char *TAG = "ADC_MANAGER";

static adc_oneshot_unit_handle_t adc_handle;

static float engine_voltage = 0;
static float light_voltage  = 0;

static bool engine_state = false;
static bool light_state  = false;

static float read_voltage(adc_channel_t channel)
{
    int raw = 0;
    int sum = 0;

    for (int i = 0; i < SAMPLE_COUNT; i++)
    {
        adc_oneshot_read(adc_handle, channel, &raw);
        sum += raw;
    }

    float avg = (float)sum / SAMPLE_COUNT;
    // 电阻值为100K+20K
    // return avg * 3.3f / 4095.0f;
    return avg * 6.0f;
}

void adc_manager_init(void)
{
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_new_unit(&init_config, &adc_handle);

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };

    adc_oneshot_config_channel(adc_handle, ENGINE_ADC_CHANNEL, &config);
    adc_oneshot_config_channel(adc_handle, LIGHT_ADC_CHANNEL, &config);

    ESP_LOGI(TAG, "ADC Manager Initialized");
}

void adc_manager_update(void)
{
    engine_voltage = read_voltage(ENGINE_ADC_CHANNEL);
    light_voltage  = read_voltage(LIGHT_ADC_CHANNEL);

    // ===== 发动机滞回判断 =====
    if (!engine_state && engine_voltage > ENGINE_ON_THRESHOLD)
        engine_state = true;
    else if (engine_state && engine_voltage < ENGINE_OFF_THRESHOLD)
        engine_state = false;

    // ===== 大灯滞回判断 =====
    if (!light_state && light_voltage > LIGHT_ON_THRESHOLD)
        light_state = true;
    else if (light_state && light_voltage < LIGHT_OFF_THRESHOLD)
        light_state = false;

    ESP_LOGI(TAG,
             "Engine: %.2fV (%d) | Light: %.2fV (%d)",
             engine_voltage, engine_state,
             light_voltage, light_state);
}

bool adc_engine_running(void)
{
    return engine_state;
}

bool adc_light_on(void)
{
    return light_state;
}

float adc_get_engine_voltage(void)
{
    return engine_voltage;
}

float adc_get_light_voltage(void)
{
    return light_voltage;
}