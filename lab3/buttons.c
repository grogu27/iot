//https://wokwi.com/projects/448616876861711361
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc.h"
#include "soc/gpio_reg.h"

// Макрос для перевода номера GPIO в битовую маску
#define GPIO(gpio_num) (1ULL << (gpio_num))

#define LED GPIO_NUM_4

inline bool is_pin_set(uint64_t data, uint64_t pin)
{
    return (data & pin) != 0;
}

void app_main()
{
    gpio_config_t io_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = GPIO(25) | GPIO(26) ,
        .pull_up_en = GPIO_PULLUP_DISABLE, // Установите GPIO_PULLUP_ENABLE, если НЕ используете внешние резисторы
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };
       gpio_config_t io_config2 = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask =GPIO(32) | GPIO(33),
        .pull_up_en = GPIO_PULLUP_DISABLE, // Установите GPIO_PULLUP_ENABLE, если НЕ используете внешние резисторы
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };
    gpio_config(&io_config);
    gpio_config(&io_config2);


    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    bool led_state = false;
    uint32_t delay = 1000;
    while (1)
    {
        uint64_t io_data = REG_READ(GPIO_IN_REG);

        uint64_t io_data2 = REG_READ(GPIO_IN1_REG);

        printf("GPIO_IN_REG: %llu\r\n", io_data);
                printf("GPIO_IN1_REG: %llu\r\n", io_data2);
        printf("Is GPIO14 set? %d\r\n", is_pin_set(io_data, GPIO(25)));
        printf("Is GPIO25 set? %d\r\n", is_pin_set(io_data, GPIO(26)));
        printf("Is GPIO26 set? %d\r\n", is_pin_set(io_data2, GPIO(32)));
        printf("Is GPIO27 set? %d\r\n", is_pin_set(io_data2, GPIO(33)));

        if (!is_pin_set(io_data, GPIO(25)))
            delay = 1000;
        if (!is_pin_set(io_data, GPIO(26)))
            delay = 500;
        if (!is_pin_set(io_data2, GPIO(32)))
            delay = 250;
        if (!is_pin_set(io_data2, GPIO(33)))
            delay = 100;
        gpio_set_level(LED, led_state);
        led_state = !led_state;
        vTaskDelay(delay / portTICK_PERIOD_MS);

    }
}
