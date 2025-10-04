#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

#define BLINK_GPIO GPIO_NUM_4

TaskHandle_t HelloWorldTaskHandle = NULL;
TaskHandle_t BlinkTaskHandle = NULL;

void HelloWorld_Task(void *arg) 
{
    while(1) {
        printf("Task running: Hello World ..\n");
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

void Blink_Task(void *arg) 
{
    gpio_reset_pin(BLINK_GPIO);
    esp_rom_gpio_pad_select_gpio(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    int count_second = 0;
    while(1) {
        switch (count_second)
        {
        case 10:
            vTaskSuspend(HelloWorldTaskHandle);
            printf("Hello World Task Suspended .. \n");
            break;
        case 14:
            vTaskResume(HelloWorldTaskHandle);
            printf("Hello World Task Resumed .. \n");
            break;
        case 20:
            vTaskDelete(HelloWorldTaskHandle);
            printf("Hello World Task Deleted .. \n");
            break;
        default:
            printf("%d second.. \n", count_second);
            break;
        }
        gpio_set_level(BLINK_GPIO, true);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        count_second++;
        gpio_set_level(BLINK_GPIO, false);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        count_second++;
    }
}

void app_main(void)
{
    xTaskCreatePinnedToCore(Blink_Task, "Blink Task", 4096, NULL, 10, &BlinkTaskHandle, 0);
    xTaskCreatePinnedToCore(HelloWorld_Task, "Hello World", 4096, NULL, 10, &HelloWorldTaskHandle, 1);
}