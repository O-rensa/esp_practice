#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"

#define BLINK_GPIO GPIO_NUM_4
#define BUTTON_GPIO GPIO_NUM_5

TaskHandle_t   BlinkTaskHandle = NULL;

bool led_state = false;

void Blink_Task(void *arg)
{
    while(true) {
        gpio_set_level(BLINK_GPIO, led_state);
    }
}

static void IRAM_ATTR gpio_isr_handler(void *arg) 
{
    led_state = !led_state;
}

void app_main(void)
{   
    gpio_reset_pin(BLINK_GPIO);
    gpio_reset_pin(BUTTON_GPIO);

    // select GPIO pin
    esp_rom_gpio_pad_select_gpio(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT); // output mode

    // select GPIO pin
    esp_rom_gpio_pad_select_gpio(BUTTON_GPIO);
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT); // input mode

    // set mode to pull-up only
    gpio_pullup_en(BUTTON_GPIO);   
    gpio_pulldown_dis(BUTTON_GPIO);

    // interrupt type
    gpio_set_intr_type(BUTTON_GPIO, GPIO_INTR_POSEDGE);

    // install isr service
    gpio_install_isr_service(0);

    // add interrupt handler
    gpio_isr_handler_add(BUTTON_GPIO, gpio_isr_handler, NULL);

    // enable the interrupt
    gpio_intr_enable(BUTTON_GPIO);

    xTaskCreatePinnedToCore(Blink_Task, "Blink", 4096, NULL, 10, &BlinkTaskHandle, 0); // core 0
}