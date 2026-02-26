#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mpu6050_manager.h"
#include "mos_control.h"
// #include <math.h>
#include "esp_task_wdt.h"
#include "adc_manager.h"

#define ANGLE_THRESHOLD 10.0
#define TRIGGER_TIME_MS 400

void app_main(void)
{
    // 初始化看门狗（3秒超时）
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = 3000,
        .idle_core_mask = 0,
        .trigger_panic = true,
    };
    // 初始化看门狗
    esp_task_wdt_init(&twdt_config);
    esp_task_wdt_add(NULL);  // 添加当前任务
    // 初始化ADC
    adc_manager_init();
    // 初始化MPU6050
    mpu6050_init();
    // 初始化MOS管
    mos_init();

    int left_trigger = 0;
    int right_trigger = 0;
    int left_release = 0;
    int right_release = 0;

    bool left_on = false;
    bool right_on = false;

    while (1)
    {
        // 更新ADC
        adc_manager_update();
        // 
        mpu6050_update();
        // 定义状态
        bool engine = adc_engine_running();
        bool light  = adc_light_on();
        // 获取Roll角度
        float roll = mpu_get_roll();
        // ===== 系统是否允许工作 =====
        bool system_enabled = engine && light;
        if (!system_enabled)
        {
            mos_left_off();
            mos_right_off();
        }
        else
        {
            // ===== 左侧 =====
            if (roll > ANGLE_THRESHOLD)
            {
                left_trigger += 50;
                left_release = 0;

                if (left_trigger >= TRIGGER_TIME_MS && !left_on)
                {
                    mos_left_on();
                    left_on = true;
                }
            }
            else
            {
                left_release += 50;
                left_trigger = 0;

                if (left_release >= TRIGGER_TIME_MS && left_on)
                {
                    mos_left_off();
                    left_on = false;
                }
            }

            // ===== 右侧 =====
            if (roll < -ANGLE_THRESHOLD)
            {
                right_trigger += 50;
                right_release = 0;

                if (right_trigger >= TRIGGER_TIME_MS && !right_on)
                {
                    mos_right_on();
                    right_on = true;
                }
            }
            else
            {
                right_release += 50;
                right_trigger = 0;

                if (right_release >= TRIGGER_TIME_MS && right_on)
                {
                    mos_right_off();
                    right_on = false;
                }
            }
        }
       
        // 喂狗
        esp_task_wdt_reset();
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}