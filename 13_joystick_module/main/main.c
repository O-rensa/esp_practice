#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_adc/adc_oneshot.h"
#include "hal/adc_types.h"

#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_cali.h"

TaskHandle_t ADCTaskHandle = NULL;

adc_oneshot_unit_handle_t unit_handle = NULL;
adc_cali_handle_t cali_handle = NULL;

void app_init();
void ADCTask(void *arg);

void app_main(void)
{
    app_init();
    printf("Hello world\n");
    xTaskCreatePinnedToCore(ADCTask, "ADCTask", 4096, NULL, 10, &ADCTaskHandle, 0);
}

void app_init() {
    adc_oneshot_unit_init_cfg_t unit_config = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_config, &unit_handle));

    adc_oneshot_chan_cfg_t chan_config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_12,
    };

    ESP_ERROR_CHECK(adc_oneshot_config_channel(unit_handle, ADC_CHANNEL_3, &chan_config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(unit_handle, ADC_CHANNEL_5, &chan_config));

    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };

    ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cali_config, &cali_handle));
}

void ADCTask(void *arg) {
    int rx_read, rx_output, ry_read, ry_output;
    while(1) 
    {
        ESP_ERROR_CHECK(adc_oneshot_read(unit_handle, ADC_CHANNEL_3, &rx_read));
        //printf("Rx read from joystick_module: %d\n", rx_read);
        adc_cali_raw_to_voltage(cali_handle, rx_read, &rx_output);
        //printf("Rx calibrated value: %d\n", rx_output);
        ESP_ERROR_CHECK(adc_oneshot_read(unit_handle, ADC_CHANNEL_5, &ry_read));
        //printf("Ry read from joystick_module: %d\n", ry_read);
        adc_cali_raw_to_voltage(cali_handle, ry_read, &ry_output);
        //printf("Ry` calibrated value: %d\n", ry_output);
        //print("\n");
        printf("%d %d\n", rx_output, ry_output);

        vTaskDelay(pdMS_TO_TICKS(100));
    }

    adc_oneshot_del_unit(unit_handle);
    adc_cali_delete_scheme_curve_fitting(cali_handle);
    vTaskDelete(NULL);
}