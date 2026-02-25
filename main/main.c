// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "adc_manager.h"
// #include "mpu6050_manager.h"
// #define SYSTEM_FORCE_ENABLE  1   // 1 = 忽略电压条件

// void app_main(void)
// {
//     adc_manager_init();
//     mpu6050_init();

//     while (1)
//     {
//         adc_manager_update();
//         vTaskDelay(pdMS_TO_TICKS(500));
//         mpu6050_update();
//         vTaskDelay(pdMS_TO_TICKS(200));
//     }
// }

// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "mpu6050_manager.h"

// void app_main(void)
// {
//     mpu6050_init();

//     while (1)
//     {
//         mpu6050_update();
//         vTaskDelay(pdMS_TO_TICKS(200));
//     }
// }


// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "mpu6050_manager.h"
// #include "esp_log.h"
// #include <math.h>

// #define ANGLE_THRESHOLD 10.0
// #define TRIGGER_TIME_MS 400

// static const char *TAG = "MAIN";

// void app_main(void)
// {
//     mpu6050_init();

//     int trigger_counter = 0;
//     int release_counter = 0;
//     bool light_on = false;

//     while (1)
//     {
//         mpu6050_update();

//         float roll = mpu_get_roll();

//         if (fabs(roll) > ANGLE_THRESHOLD)
//         {
//             trigger_counter += 50;
//             release_counter = 0;

//             if (trigger_counter >= TRIGGER_TIME_MS && !light_on)
//             {
//                 light_on = true;
//                 ESP_LOGI(TAG, ">>> LIGHT ON <<<");
//             }
//         }
//         else
//         {
//             release_counter += 50;
//             trigger_counter = 0;

//             if (release_counter >= TRIGGER_TIME_MS && light_on)
//             {
//                 light_on = false;
//                 ESP_LOGI(TAG, ">>> LIGHT OFF <<<");
//             }
//         }

//         vTaskDelay(pdMS_TO_TICKS(50));
//     }
// }

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mpu6050_manager.h"
#include "mos_control.h"
#include <math.h>

#define ANGLE_THRESHOLD 10.0
#define TRIGGER_TIME_MS 400

void app_main(void)
{
    mpu6050_init();
    mos_init();

    int left_trigger = 0;
    int right_trigger = 0;
    int left_release = 0;
    int right_release = 0;

    bool left_on = false;
    bool right_on = false;

    while (1)
    {
        mpu6050_update();
        float roll = mpu_get_roll();

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

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}