#include <stdio.h>
#include "Freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "inttypes.h"
#include "esp_timer.h"

#define INTERRUPT 5
#define LED 2
#define DEBOUNCE_TIME 50000 // 50 Thousand microseconds == 50 milliseconds
volatile uint16_t interrupt_count = 0;
volatile bool button_state = false;

static void IRAM_ATTR gpio_isr_handler(void *arg) 
{
    static int64_t last_isr_time = 0;
    int64_t now = esp_timer_get_time();
    // debouncer
    // safe value for debouncer is 50 milliseconds
    if (now - last_isr_time > DEBOUNCE_TIME) 
    {
        interrupt_count++;
        button_state = true;
        last_isr_time = now;
    }
}

void app_main(void)
{
    // reset or cleanup
    gpio_reset_pin(INTERRUPT);
    gpio_reset_pin(LED);

    // set direction
    gpio_set_direction(INTERRUPT, GPIO_MODE_INPUT);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    // set pull mode
    // - standard is GPIO PULL UP ONLY
    // - HIGH when up 
    // - LOW when down 
    gpio_set_pull_mode(INTERRUPT, GPIO_PULLUP_ONLY);

    // set interrupt type
    // POSEDGE means if there is change from LOW to HIGH
    gpio_set_intr_type(INTERRUPT, GPIO_INTR_POSEDGE);

    // install isr service
    gpio_install_isr_service(0);
    gpio_isr_handler_add(INTERRUPT, gpio_isr_handler, NULL);

    // enable interrupt
    gpio_intr_enable(INTERRUPT);

    while(1) 
    {
        if (button_state == true) 
        {
            printf("%d\n", interrupt_count);
            button_state = false;
            gpio_set_level(LED, true);
        }
        else 
        {
            gpio_set_level(LED, false);
        }
        vTaskDelay(100/ portTICK_PERIOD_MS);
    }
}

// Difference between get_level and interrupts
// - get_level checks the value of get_level every loop --> wastes cpu usage
// - interrupts run event every time interrupt type is satisfied --> more efficient