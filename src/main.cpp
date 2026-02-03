// TB6612 Motor Driver Test - ESP32-C6
// Tests motor forward/reverse and PWM speed control

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"

// Motor control pins (TB6612)
#define MOTOR_AIN1  GPIO_NUM_0  // D0 - Direction control 1
#define MOTOR_AIN2  GPIO_NUM_1  // D1 - Direction control 2
#define MOTOR_PWMA  GPIO_NUM_2  // D2 - Speed control (PWM)

// PWM configuration
#define PWM_TIMER       LEDC_TIMER_0
#define PWM_MODE        LEDC_LOW_SPEED_MODE
#define PWM_CHANNEL     LEDC_CHANNEL_0
#define PWM_FREQUENCY   1000    // 1kHz
#define PWM_RESOLUTION  LEDC_TIMER_8_BIT  // 0-255

static const char *TAG = "MotorTest";

// Motor control functions
void motor_init(void) {
    // Configure direction pins as outputs
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << MOTOR_AIN1) | (1ULL << MOTOR_AIN2);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    // Configure PWM for speed control
    ledc_timer_config_t timer_conf = {
        .speed_mode = PWM_MODE,
        .duty_resolution = PWM_RESOLUTION,
        .timer_num = PWM_TIMER,
        .freq_hz = PWM_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer_conf);

    ledc_channel_config_t channel_conf = {
        .gpio_num = MOTOR_PWMA,
        .speed_mode = PWM_MODE,
        .channel = PWM_CHANNEL,
        .timer_sel = PWM_TIMER,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&channel_conf);

    ESP_LOGI(TAG, "Motor driver initialized");
}

void motor_forward(uint8_t speed) {
    gpio_set_level(MOTOR_AIN1, 1);  // HIGH
    gpio_set_level(MOTOR_AIN2, 0);  // LOW
    ledc_set_duty(PWM_MODE, PWM_CHANNEL, speed);
    ledc_update_duty(PWM_MODE, PWM_CHANNEL);
    ESP_LOGI(TAG, "Motor FORWARD at speed %d", speed);
}

void motor_reverse(uint8_t speed) {
    gpio_set_level(MOTOR_AIN1, 0);  // LOW
    gpio_set_level(MOTOR_AIN2, 1);  // HIGH
    ledc_set_duty(PWM_MODE, PWM_CHANNEL, speed);
    ledc_update_duty(PWM_MODE, PWM_CHANNEL);
    ESP_LOGI(TAG, "Motor REVERSE at speed %d", speed);
}

void motor_stop(void) {
    gpio_set_level(MOTOR_AIN1, 0);  // LOW
    gpio_set_level(MOTOR_AIN2, 0);  // LOW
    ledc_set_duty(PWM_MODE, PWM_CHANNEL, 0);
    ledc_update_duty(PWM_MODE, PWM_CHANNEL);
    ESP_LOGI(TAG, "Motor STOPPED (coast)");
}

void motor_brake(void) {
    gpio_set_level(MOTOR_AIN1, 1);  // HIGH
    gpio_set_level(MOTOR_AIN2, 1);  // HIGH
    ledc_set_duty(PWM_MODE, PWM_CHANNEL, 255);
    ledc_update_duty(PWM_MODE, PWM_CHANNEL);
    ESP_LOGI(TAG, "Motor BRAKE (active)");
}

// Smooth speed ramp (soft start/stop)
void motor_ramp_speed(bool forward, uint8_t target_speed, uint16_t ramp_time_ms) {
    const uint8_t steps = 50;
    const uint16_t step_delay = ramp_time_ms / steps;

    for (int i = 0; i <= steps; i++) {
        uint8_t speed = (target_speed * i) / steps;
        if (forward) {
            motor_forward(speed);
        } else {
            motor_reverse(speed);
        }
        vTaskDelay(step_delay / portTICK_PERIOD_MS);
    }
}

extern "C" void app_main(void) {
    printf("\n╔═══════════════════════════════╗\n");
    printf("║  Simple Motor Test Program   ║\n");
    printf("║  Forward → Reverse → Repeat   ║\n");
    printf("╚═══════════════════════════════╝\n\n");

    // Initialize motor driver
    motor_init();

    printf("Starting motor test loop...\n\n");

    // Simple test loop
    while (1) {
        // Forward at full speed
        ESP_LOGI(TAG, "→ FORWARD (full speed)");
        motor_forward(255);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        // 1 second break
        motor_stop();
        ESP_LOGI(TAG, "○ STOP");
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        // Reverse at full speed
        ESP_LOGI(TAG, "← REVERSE (full speed)");
        motor_reverse(255);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        // 1 second break
        motor_stop();
        ESP_LOGI(TAG, "○ STOP");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
