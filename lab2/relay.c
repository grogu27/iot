
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED GPIO_NUM_32
#define RELAY GPIO_NUM_33

void app_main(void)
{
  gpio_set_direction(LED, GPIO_MODE_OUTPUT);
  gpio_set_direction(RELAY, GPIO_MODE_INPUT_OUTPUT);

  uint32_t ticks = 0;
  while (true)
  {
    gpio_set_level(LED, ticks % 2);
    if (ticks % 20 == 0)
      gpio_set_level(RELAY, !gpio_get_level(RELAY));
    
    ticks++;
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}
