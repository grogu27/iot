#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED     GPIO_NUM_14   
#define BUTTON  GPIO_NUM_13   

#define DEBOUNCE_DELAY_MS 20

uint8_t read_button_debounced(gpio_num_t pin)
{
    uint8_t first = gpio_get_level(pin);
    vTaskDelay(DEBOUNCE_DELAY_MS / portTICK_PERIOD_MS);
    uint8_t second = gpio_get_level(pin);
    return (first == second) ? first : read_button_debounced(pin);
}

// void app_main(void)
// {
//     gpio_set_direction(LED, GPIO_MODE_OUTPUT);
//     gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
//     gpio_set_pull_mode(BUTTON, GPIO_FLOATING); 

//     int delay_ms = 500; // 1 Гц мигание
//     int led_state = 0;
//     uint8_t button;
//     uint8_t last_button = 1;
//     while (1) {
//         button = gpio_get_level(BUTTON);
//         //button = read_button_debounced(BUTTON);
//         printf("GPIO state is %d\n", button);

//         if ((last_button == 1 && button == 0) || (last_button == 0 && button == 1)) 
//             printf("Кнопка была нажата!\n");
        
//         // Обновляем состояние
//         last_button = button;

//         if (button == 0) 
//           delay_ms = 100; // 5 Гц
//         else
//             delay_ms = 500; // 1 Гц

//         led_state = !led_state;
//         gpio_set_level(LED, led_state);

//         vTaskDelay(delay_ms / portTICK_PERIOD_MS);
//     }
// }

// после отпускания кнопки пин имеет неопределенное состояние
// void app_main(void)
// {
//     gpio_set_direction(LED, GPIO_MODE_OUTPUT);
//     gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
//     gpio_set_pull_mode(BUTTON, GPIO_FLOATING); 

//     int delay_ms = 500; // 1 Гц мигание
//     uint8_t button;
//     uint8_t last_button = 1;
//     while (1) {

//       //button = gpio_get_level(BUTTON);
//       button = read_button_debounced(BUTTON);
//       printf("GPIO state is %d\n", button);
//       if ((last_button == 1 && button == 0) || (last_button == 0 && button == 1)) 
//         printf("Кнопка была нажата!\n");
//           //Обновляем состояние
//           last_button = button;

//       if(button)
//         gpio_set_level(LED, 0);
//       else
//         gpio_set_level(LED, 1);
      
//       vTaskDelay(delay_ms / portTICK_PERIOD_MS);
//       }
// }

// через внутренний под регистр, он подтягивает пин к 1 когда кнопка не замкнута
void app_main(void)
{
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON, GPIO_PULLUP_ONLY); 

    int delay_ms = 500; // 1 Гц мигание
    uint8_t button;
    uint8_t last_button = 1;
    uint8_t fast_mod = 0;
    while (1) {

      //button = gpio_get_level(BUTTON);
      button = read_button_debounced(BUTTON);
      printf("GPIO state is %d\n", button);
      if ((last_button == 1 && button == 0) ){
        fast_mod = !fast_mod;
        printf("Режим поменялся!\n");
        printf("Кнопка была нажата!\n");
      } 
        
          //Обновляем состояние
          last_button = button;

      if(fast_mod)
        gpio_set_level(LED, 0);
      else
        gpio_set_level(LED, 1);
      
      vTaskDelay(delay_ms / portTICK_PERIOD_MS);
      }
}
