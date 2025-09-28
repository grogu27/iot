#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED     GPIO_NUM_14   
#define BUTTON  GPIO_NUM_13   
void app_main(void)
{
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON, GPIO_PULLUP_ONLY); 

    int delay_ms = 500; // 1 Гц мигание
    int led_state = 0;

    while (1) {
        uint8_t button = gpio_get_level(BUTTON);
        printf("GPIO state is %d\n", button);

        if (button == 0) 
            delay_ms = 100; // 5 Гц
        else
            delay_ms = 500; // 1 Гц

        led_state = !led_state;
        gpio_set_level(LED, led_state);

        vTaskDelay(delay_ms / portTICK_PERIOD_MS);
    }
}
