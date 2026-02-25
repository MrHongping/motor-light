#ifndef ADC_MANAGER_H
#define ADC_MANAGER_H

#include <stdbool.h>

void adc_manager_init(void);
void adc_manager_update(void);

bool adc_engine_running(void);
bool adc_light_on(void);

float adc_get_engine_voltage(void);
float adc_get_light_voltage(void);

#endif