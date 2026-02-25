#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "adc_manager.h"

void app_main(void)
{
    adc_manager_init();

    while (1)
    {
        adc_manager_update();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}