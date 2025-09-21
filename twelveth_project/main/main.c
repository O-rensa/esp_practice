#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"

char *tx_data = "Hi I am from ESP32\n";
char rx_data[128];

TaskHandle_t UARTTaskHandle = NULL;

void UART_task(void *arg)
{
    while (true)
    {
        uart_write_bytes(UART_NUM_0, tx_data, strlen(tx_data)); // transmit data
        int len = uart_read_bytes(UART_NUM_0, rx_data, sizeof(rx_data) - 1, pdMS_TO_TICKS(100));// recieve data
        if (len > 0) {
            rx_data[len] = '\0';
            printf("Length: %d, Data: %s \n", len, rx_data);
            fflush(stdout);
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

void app_main(void)
{
    init_uart();
    xTaskCreatePinnedToCore(UART_task, "UART Task", 4096, NULL, 10, &UARTTaskHandle, 0);
};