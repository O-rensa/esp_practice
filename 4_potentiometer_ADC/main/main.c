#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "driver/adc.h"

volatile int adc_value = 0;

void app_main(void)
{
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_12);
    adc1_config_width(ADC_WIDTH_BIT_12);

    while(true) {
        adc_value = adc1_get_raw(ADC1_CHANNEL_3);
        printf("ADC value is %d\n", adc_value);
        vTaskDelay(100/ portTICK_PERIOD_MS);
    }
}