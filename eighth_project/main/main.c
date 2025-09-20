#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "sdkconfig.h"
#include "driver/ledc.h"

TaskHandle_t ServoRotateTaskHandle = NULL;

void ServoRotate_task(void *args) 
{
    int duty = 1648; // duty will vary from 1638 to 3276
    int step = 14; // each iteration the duty increments itself by this step
    int total_cycles = 117; // 117 iterations in total
    bool pos_direction = true; // true = up, false = down
    int iteration_time = 10; // msecs

    ledc_timer_config_t timer_config = {
        .duty_resolution = LEDC_TIMER_14_BIT,
        .freq_hz = 50,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .clk_cfg = LEDC_AUTO_CLK,
    };

    ledc_timer_config(&timer_config);

    ledc_channel_config_t ledc_conf = {
        .channel = LEDC_CHANNEL_0,
        .duty = duty,
        .gpio_num = 4,
        .intr_type = LEDC_INTR_DISABLE,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_sel = LEDC_TIMER_0,
    };

    ledc_channel_config(&ledc_conf);

    int i; 
    while (1)
    {
        for(i = 0; i < total_cycles; i++) {
            // if direction is positive, the duty increments itself, otherwise it will be reduced in value;
            pos_direction ? (duty += step) : (duty -= step);

            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

            vTaskDelay(iteration_time/portTICK_PERIOD_MS);
        }
        // change direction
        pos_direction = !pos_direction;
    }
    
}

void app_main(void)
{
    xTaskCreatePinnedToCore(ServoRotate_task, "ServoRotate", 4096, NULL, 10, &ServoRotateTaskHandle, 0);
}