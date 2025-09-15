#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"

#define led_pin 2

void app_main(void)
{
    gpio_reset_pin(led_pin);
    gpio_set_direction(led_pin, GPIO_MODE_OUTPUT);

    while (1)
    {
        const TickType_t delay = 1000/portTICK_PERIOD_MS;
        gpio_set_level(led_pin, true);
        vTaskDelay(delay);

        gpio_set_level(led_pin, false);
        vTaskDelay(delay);
    }
}