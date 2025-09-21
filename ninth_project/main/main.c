#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

volatile uint16_t count = 0;
TaskHandle_t DisplayCountTaskHandle = NULL;

void DisplayCount_Task(void *args) 
{
    while(true) {
        printf("%d\n", count);
        vTaskDelay(10/portTICK_PERIOD_MS); 
    }
}

void timer_callback(void *args) 
{
    count++;
}

void init_timer() 
{
    const esp_timer_create_args_t timer_args = {
        .callback = &timer_callback,
        .name = "Timer Interrupt,"
    };

    esp_timer_handle_t timer_handler;
    esp_timer_create(&timer_args, &timer_handler);
    esp_timer_start_periodic(timer_handler, 10000); // 10000 micro second = 10 millisecond
}

void app_main(void)
{
    init_timer();
    xTaskCreatePinnedToCore(DisplayCount_Task, "Display Count", 4096, NULL, 10, &DisplayCountTaskHandle, 0);
}