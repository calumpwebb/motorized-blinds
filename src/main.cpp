// Hello World - Fast LED Blink Test
// XIAO ESP32C6 onboard LED on GPIO 15

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED_PIN GPIO_NUM_15
#define BLINK_DELAY 500  // milliseconds

extern "C" void app_main(void)
{
    // Configure GPIO
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << LED_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    printf("LED blink test - 500ms interval\n");

    // Main loop
    while(1) {
        gpio_set_level(LED_PIN, 1);  // LED on
        vTaskDelay(BLINK_DELAY / portTICK_PERIOD_MS);
        gpio_set_level(LED_PIN, 0);  // LED off
        vTaskDelay(BLINK_DELAY / portTICK_PERIOD_MS);
    }
}
