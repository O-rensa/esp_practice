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
#include "freertos/queue.h"
#include "sdkconfig.h"
#include "driver/ledc.h"

int servo_min_duty = 819;
int servo_max_duty = 1638;

volatile const int rx_servo_pin = 4;
volatile const int ry_servo_pin = 5;
volatile const int step = 39;

volatile int rx_duty = 1229;
volatile int ry_duty = 1229;

TaskHandle_t ADCTaskHandle = NULL;

adc_oneshot_unit_handle_t unit_handle = NULL;
adc_cali_handle_t cali_handle = NULL;

void app_init();
void ADCTask(void *arg);

void app_main(void)
{
    app_init();
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

    ESP_ERROR_CHECK(adc_oneshot_config_channel(unit_handle, ADC_CHANNEL_5, &chan_config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(unit_handle, ADC_CHANNEL_6, &chan_config));

    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };

    ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cali_config, &cali_handle));

    ledc_timer_config_t timer_config = {
        .duty_resolution = LEDC_TIMER_14_BIT,
        .freq_hz = 50,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .clk_cfg = LEDC_AUTO_CLK,
    };

    ledc_timer_config(&timer_config);

    ledc_channel_config_t rx_ledc_channel_config = {
        .channel = LEDC_CHANNEL_0,
        .duty = rx_duty,
        .gpio_num = rx_servo_pin,
        .intr_type = LEDC_INTR_DISABLE,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_sel = LEDC_TIMER_0,
    };

    ledc_channel_config_t ry_ledc_channel_config = {
        .channel = LEDC_CHANNEL_1,
        .duty = ry_duty,
        .gpio_num = ry_servo_pin,
        .intr_type = LEDC_INTR_DISABLE,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_sel = LEDC_TIMER_0,
    };

    ledc_channel_config(&rx_ledc_channel_config);
    ledc_channel_config(&ry_ledc_channel_config);
}

void ADCTask(void *arg) {
    int rx_read, rx_output, ry_read, ry_output;
    while(1) 
    {
        ESP_ERROR_CHECK(adc_oneshot_read(unit_handle, ADC_CHANNEL_5, &rx_read));
        adc_cali_raw_to_voltage(cali_handle, rx_read, &rx_output);
        ESP_ERROR_CHECK(adc_oneshot_read(unit_handle, ADC_CHANNEL_6, &ry_read));
        adc_cali_raw_to_voltage(cali_handle, ry_read, &ry_output);

        // rx servo
        if (rx_output <= 100) // joystick left
        {
            printf("rx_duty: %d\n", rx_duty);
            if (rx_duty > servo_min_duty)
            {
                rx_duty -= step;
            }
        }

        if (rx_output >= 3000) // joystick right
        {
            printf("rx_duty: %d\n", rx_duty);
            if (rx_duty < servo_max_duty) 
            {
                rx_duty += step;
            }
        }

        // ry servo
        if (ry_output <= 100) // joystick up 
        {
            printf("ry_duty: %d\n", ry_duty);
            if (ry_duty < servo_max_duty) 
            {
                ry_duty += step;
            }
        }

        if (ry_output >= 3000) // joystick down
        {
            printf("ry_duty: %d\n", ry_duty);
            if (ry_duty > servo_min_duty) 
            {
                ry_duty -= step;
            }
        }

        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, rx_duty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, ry_duty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);

        vTaskDelay(pdMS_TO_TICKS(100));
    }

    adc_oneshot_del_unit(unit_handle);
    adc_cali_delete_scheme_curve_fitting(cali_handle);
    vTaskDelete(NULL);
}