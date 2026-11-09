#ifndef __BSP_ADC_H
#define __BSP_ADC_H

#include "gd32h73x_75x.h"

#define ADC_TEMP_CALIBRATION_VALUE                REG16(0x1FF0F7C8)

float Get_temperature(void);
void adc_config(void);
#endif