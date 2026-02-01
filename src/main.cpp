// WiFi Connection Test with LED Status Indicators
// XIAO ESP32C6 - GPIO 15 LED (active-LOW)

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#define LED_PIN GPIO_NUM_15
#define LED_ON 0   // Active-low
#define LED_OFF 1

// WiFi credentials from .env file (injected at build time)
#ifndef WIFI_SSID
#define WIFI_SSID "www-iot"
#endif
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD ""
#endif

static const char *TAG = "WiFi";

// WiFi connection status
typedef enum {
    WIFI_CONNECTING,
    WIFI_CONNECTED,
    WIFI_FAILED
} wifi_status_t;

static wifi_status_t wifi_status = WIFI_CONNECTING;

// LED blink patterns
void led_pattern_connecting(void) {
    // Continuous medium blink (500ms on/off)
    gpio_set_level(LED_PIN, LED_ON);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    gpio_set_level(LED_PIN, LED_OFF);
    vTaskDelay(500 / portTICK_PERIOD_MS);
}

void led_pattern_connected(void) {
    // Solid on - connected successfully
    gpio_set_level(LED_PIN, LED_ON);
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Just delay to keep task running
}

void led_pattern_failed(void) {
    // SOS pattern: · · · — — — · · ·
    // 3 short
    for (int i = 0; i < 3; i++) {
        gpio_set_level(LED_PIN, LED_ON);
        vTaskDelay(200 / portTICK_PERIOD_MS);
        gpio_set_level(LED_PIN, LED_OFF);
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
    // 3 long
    for (int i = 0; i < 3; i++) {
        gpio_set_level(LED_PIN, LED_ON);
        vTaskDelay(600 / portTICK_PERIOD_MS);
        gpio_set_level(LED_PIN, LED_OFF);
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
    // 3 short
    for (int i = 0; i < 3; i++) {
        gpio_set_level(LED_PIN, LED_ON);
        vTaskDelay(200 / portTICK_PERIOD_MS);
        gpio_set_level(LED_PIN, LED_OFF);
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Pause before repeat
}

// LED task
void led_task(void *pvParameters) {
    while (1) {
        switch (wifi_status) {
            case WIFI_CONNECTING:
                led_pattern_connecting();
                break;
            case WIFI_CONNECTED:
                led_pattern_connected();
                break;
            case WIFI_FAILED:
                led_pattern_failed();
                break;
        }
    }
}

// WiFi event handler
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                              int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "Connecting to WiFi...");
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Disconnected, retrying...");
        wifi_status = WIFI_CONNECTING;
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Connected! IP: " IPSTR, IP2STR(&event->ip_info.ip));
        wifi_status = WIFI_CONNECTED;
    }
}

// Initialize WiFi
void wifi_init(void) {
    // Initialize NVS (required for WiFi)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize network interface
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    // Initialize WiFi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Register event handlers
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));

    // Configure WiFi
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi init complete. SSID: %s", WIFI_SSID);
}

extern "C" void app_main(void) {
    // Configure LED GPIO
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << LED_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    printf("\n=== WiFi Connection Test ===\n");
    printf("SSID: %s\n", WIFI_SSID);
    printf("LED Patterns:\n");
    printf("  - Medium blink (500ms): Connecting...\n");
    printf("  - Solid ON: Connected!\n");
    printf("  - SOS pattern: Failed\n\n");

    // Initialize WiFi
    wifi_init();

    // Start LED task
    xTaskCreate(led_task, "led_task", 2048, NULL, 5, NULL);
}
