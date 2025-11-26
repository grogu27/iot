//https://wokwi.com/projects/448660765595859969

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc.h"
#include "soc/gpio_reg.h"
#include <inttypes.h>

#define GPIO(gpio_num) (1ULL << (gpio_num))

#define LED GPIO_NUM_2

#define BUTTON1 GPIO_NUM_25
#define BUTTON2 GPIO_NUM_26
#define BUTTON3 GPIO_NUM_32
#define BUTTON4 GPIO_NUM_33

inline bool is_pin_set(uint64_t data, uint64_t pin)
{
    return (data & pin) != 0;
}

void app_main()
{
    // Конфигурация для пинов 25 и 26 (GPIO0-31)
    gpio_config_t io_config_low = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = GPIO(BUTTON1) | GPIO(BUTTON2),
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };
    gpio_config(&io_config_low);

    // Конфигурация для пинов 32 и 33 (GPIO32-39)
    gpio_config_t io_config_high = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = GPIO(BUTTON3) | GPIO(BUTTON4),
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };
    gpio_config(&io_config_high);

    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    bool led_state = false;
    bool btn1_active = false;
    bool btn2_active = false;
    bool btn3_active = false;
    bool btn4_active = false;


    while (1)
    {
        uint64_t io_data_low = REG_READ(GPIO_IN_REG);
        uint64_t io_data_high = REG_READ(GPIO_IN1_REG);

        uint64_t buttons_state = io_data_low;
        buttons_state |= ((io_data_high & 0x01) << 32);
        buttons_state |= ((io_data_high & 0x02) << 32);

        btn1_active = !is_pin_set(buttons_state, GPIO(BUTTON1));
        btn2_active = !is_pin_set(buttons_state, GPIO(BUTTON2));
        btn3_active = !is_pin_set(buttons_state, GPIO(BUTTON3));
        btn4_active = !is_pin_set(buttons_state, GPIO(BUTTON4));



        uint32_t final_delay = 2000; 

        int active_count = 0;
        if (btn1_active) active_count++;
        if (btn2_active) active_count++;
        if (btn3_active) active_count++;
        if (btn4_active) active_count++;

        switch(active_count) {
            case 1: final_delay = 1000; break;
            case 2: final_delay = 500; break;
            case 3: final_delay = 250; break;
            case 4: final_delay = 100; break;
            default: final_delay = 2000; break;
        }

        printf("Активные кнопки: %d%d%d%d, Задержка: %"PRIu32"ms\n",
               btn1_active, btn2_active, btn3_active, btn4_active, final_delay);

        gpio_set_level(LED, led_state);
        led_state = !led_state;
        
        vTaskDelay(100 / portTICK_PERIOD_MS);
        
        vTaskDelay(final_delay / portTICK_PERIOD_MS);

    }
}



















/*#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc.h"
#include "soc/gpio_reg.h"

// Макрос для перевода номера GPIO в битовую маску
#define GPIO(gpio_num) (1ULL << (gpio_num))

#define LED GPIO_NUM_13

inline bool is_pin_set(uint64_t data, uint64_t pin)
{
    return (data & pin) != 0;
}

void app_main()
{
    gpio_config_t io_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = GPIO(25) | GPIO(26),
        .pull_up_en = GPIO_PULLUP_DISABLE, // Установите GPIO_PULLUP_ENABLE, если НЕ используете внешние резисторы
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };

    gpio_config(&io_config);

    gpio_config_t io_config2 = {
    .intr_type = GPIO_INTR_DISABLE,
    .mode = GPIO_MODE_INPUT,
    .pin_bit_mask = GPIO(32) | GPIO(33),
    .pull_up_en = GPIO_PULLUP_DISABLE, // Установите GPIO_PULLUP_ENABLE, если НЕ используете внешние резисторы
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };

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


        printf("Is GPIO25 set? %d\r\n", is_pin_set(io_data, GPIO(25)));
        printf("Is GPIO26 set? %d\r\n", is_pin_set(io_data, GPIO(26)));
        printf("Is GPIO32 set? %lld\r\n", io_data2 & (1ULL << 0));
        printf("Is GPIO33 set? %lld\r\n", io_data2 & (1ULL << 1));

        if (!is_pin_set(io_data, GPIO(25)))
            delay = 2000;
         if (!is_pin_set(io_data, GPIO(26)))
            delay = 500;
         if (!(io_data2 & (1ULL << 0)))
            delay = 250;
         if (!(io_data2 & (1ULL << 1)))
            delay = 100;


        gpio_set_level(LED, led_state);
        led_state = !led_state;
        vTaskDelay(delay / portTICK_PERIOD_MS);
    }
} */
