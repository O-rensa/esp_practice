#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"

#define LED_GPIO GPIO_NUM_4

char rx_data[128];
TaskHandle_t UARTTaskHandle = NULL;

void UART_task(void *arg)
{
    while (true)
    {
        int len = uart_read_bytes(UART_NUM_0, rx_data, sizeof(rx_data) - 1, pdMS_TO_TICKS(100));
        if (len > 0) {
            rx_data[len] = '\0'; // add null terminator
            if (strstr(rx_data, "LED ON") != NULL) {
                gpio_set_level(LED_GPIO, 1);
            }
            if (strstr(rx_data, "LED OFF") != NULL) {
                gpio_set_level(LED_GPIO, 0);
            }
        }
        vTaskDelay(500/portTICK_PERIOD_MS);
    }
    
}

void init_uart() 
{
    uart_config_t uart_cfg = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    uart_param_config(UART_NUM_0, &uart_cfg);
    uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_0, 1024, 1024, 0, NULL, 0);
}

void init_gpio() 
{
    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
}

void app_main(void)
{
    init_uart();
    init_gpio();
    xTaskCreatePinnedToCore(UART_task, "UART task", 4096, NULL, 10, &UARTTaskHandle, 0);
}