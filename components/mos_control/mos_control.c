#include "mos_control.h"
#include "driver/gpio.h"

#define MOS_LEFT_GPIO   18
#define MOS_RIGHT_GPIO  19

void mos_init(void)
{
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << MOS_LEFT_GPIO) | (1ULL << MOS_RIGHT_GPIO),
    };

    gpio_config(&io_conf);

    gpio_set_level(MOS_LEFT_GPIO, 0);
    gpio_set_level(MOS_RIGHT_GPIO, 0);
}

void mos_left_on(void)
{
    gpio_set_level(MOS_LEFT_GPIO, 1);
}

void mos_left_off(void)
{
    gpio_set_level(MOS_LEFT_GPIO, 0);
}

void mos_right_on(void)
{
    gpio_set_level(MOS_RIGHT_GPIO, 1);
}

void mos_right_off(void)
{
    gpio_set_level(MOS_RIGHT_GPIO, 0);
}