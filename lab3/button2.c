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
  gpio_set_pull_mode(BUTTON, GPIO_FLOATING);

  uint8_t button;
  while (1) {
    button = gpio_get_level(BUTTON);
    printf("GPIO state is %d\n", button);
    if(button)
      gpio_set_level(LED, 0);
    else
      gpio_set_level(LED, 1);

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
