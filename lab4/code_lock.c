#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED     GPIO_NUM_2
#define RELAY   GPIO_NUM_26

// Кнопки
#define BTN1    GPIO_NUM_13
#define BTN2    GPIO_NUM_12
#define BTN3    GPIO_NUM_14
#define BTN4    GPIO_NUM_27

const int correct_code[4] = {1,2,3,4};

void setup_pins() {
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_direction(RELAY, GPIO_MODE_OUTPUT);

    gpio_set_direction(BTN1, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BTN1, GPIO_PULLUP_ONLY);
    gpio_set_direction(BTN2, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BTN2, GPIO_PULLUP_ONLY);
    gpio_set_direction(BTN3, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BTN3, GPIO_PULLUP_ONLY);
    gpio_set_direction(BTN4, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BTN4, GPIO_PULLUP_ONLY);

    gpio_set_level(LED, 1);   // светодиод включен (дверь закрыта)
    gpio_set_level(RELAY, 0); // реле выключено
}

int read_button() {
    if (!gpio_get_level(BTN1)) return 1;
    if (!gpio_get_level(BTN2)) return 2;
    if (!gpio_get_level(BTN3)) return 3;
    if (!gpio_get_level(BTN4)) return 4;
    return 0;
}

void blink_led(uint32_t duration_ms, uint32_t interval_ms) {
    uint32_t cycles = duration_ms / interval_ms;
    int led_state = 0;
    for (uint32_t i=0; i<cycles; i++) {
        gpio_set_level(LED, led_state);
        led_state = !led_state;
        vTaskDelay(interval_ms / portTICK_PERIOD_MS);
    }
}

void app_main(void) {
    setup_pins();

    int input_code[4] = {0,0,0,0};
    int idx = 0;

    while(1) {
        int btn = read_button();
        if(btn != 0) {
            input_code[idx++] = btn;
            printf("Button %d pressed\n", btn);
            while(read_button() != 0) vTaskDelay(10 / portTICK_PERIOD_MS);

            if(idx >= 4) {
                int correct = 1;
                for(int i=0;i<4;i++)
                    if(input_code[i] != correct_code[i])
                        correct = 0;

                if(correct) {
                    printf("Code correct! Door opens.\n");
                    gpio_set_level(RELAY, 1); // открыть дверь

                    printf("Door will stay open for 10 seconds.\n");
                    blink_led(10000, 200);    // мигаем LED 10 секунд

                    gpio_set_level(RELAY, 0); // закрыть дверь
                    gpio_set_level(LED, 1);   // LED горит постоянно
                    printf("10 seconds passed. Door closed.\n");
                } else {
                    printf("Incorrect code\n");
                }
                idx = 0;
            }
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}
