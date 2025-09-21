#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"

const int LED_GPIO = GPIO_NUM_4;
const uint32_t FREQ_HZ = 5000;

TaskHandle_t LEDFadeTaskHandle = NULL;

void  LEDFade_Task(void *arg) 
{
    int i;
    while(true){
        for (i = 0; i < 255; i++) {
            ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, i, 0);
            printf("i = %d\n", i);
            vTaskDelay(10/portTICK_PERIOD_MS);
        };

        for (i = 255; i >= 0; i--) {
            ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, i, 0);
            printf("i = %d\n", i);
            vTaskDelay(10/portTICK_PERIOD_MS);
        };
    }
}

void init_timer() 
{
    ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = FREQ_HZ,
        .clk_cfg = LEDC_APB_CLK,
    };

    ledc_timer_config(&timer_config);

    ledc_channel_config_t channel_config = {
        .gpio_num = LED_GPIO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
    };

    ledc_channel_config(&channel_config);
    ledc_fade_func_install(0);
}

void app_main(void)
{
    init_timer();
    xTaskCreatePinnedToCore(LEDFade_Task, "LEDFADE", 4096, NULL, 10, &LEDFadeTaskHandle, 0);
}