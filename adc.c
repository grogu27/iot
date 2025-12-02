#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"

// Выбираем АЦП
#define ADC_UNIT                    ADC_UNIT_1
#define ADC_CHANNEL                 ADC_CHANNEL_4
#define ADC_ATTENUATION             ADC_ATTEN_DB_12

float adc_max_voltage(adc_atten_t atten) {
    switch (atten) {
        case ADC_ATTEN_DB_0:  return 1.1f;   // 0 dB ~ 1.1 В
        case ADC_ATTEN_DB_2_5:return 1.5f;   // 2.5 dB ~ 1.5 В
        case ADC_ATTEN_DB_6:  return 2.2f;   // 6 dB ~ 2.2 В
        case ADC_ATTEN_DB_11: return 3.9f;   // 11 dB ~ 3.9 В
        default: return 1.1f;
    }
}

void app_main(void)
{
    static int adc_raw;

    // Инициализация АЦП
    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

    // Настройка АЦП
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTENUATION,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL, &config));
    float voltage;
    float max_voltage = adc_max_voltage(ADC_ATTENUATION);
    int max_adc = (1 << 12) - 1;
    while (1)
    {
        // Чтение "сырых" данных
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL, &adc_raw));
        voltage = ((float)adc_raw / max_adc) * max_voltage;
        printf("ADC%d Channel[%d] Raw Data: %d, Voltage: %.3f V\n",
               ADC_UNIT + 1, ADC_CHANNEL, adc_raw, voltage);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // Никогда не будет выполнено, но удаляем всё, что создали
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc_handle));
}

{
  "version": 1,
  "author": "Anonymous maker",
  "editor": "wokwi",
  "parts": [
    {
      "type": "board-esp32-devkit-c-v4",
      "id": "esp",
      "top": -38.4,
      "left": 139.24,
      "attrs": { "builder": "esp-idf" }
    },
    { "type": "wokwi-potentiometer", "id": "pot1", "top": -1.3, "left": -125, "attrs": {} }
  ],
  "connections": [
    [ "esp:TX", "$serialMonitor:RX", "", [] ],
    [ "esp:RX", "$serialMonitor:TX", "", [] ],
    [ "esp:GND.1", "pot1:GND", "black", [ "h0" ] ],
    [ "esp:5V", "pot1:VCC", "red", [ "h0" ] ],
    [ "esp:32", "pot1:SIG", "green", [ "h0" ] ]
  ],
  "dependencies": {}
}
