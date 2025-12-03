
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

// Настройки АЦП
#define ADC_UNIT                    ADC_UNIT_1
#define ADC_CHANNEL_1               ADC_CHANNEL_4
#define ADC_CHANNEL_2               ADC_CHANNEL_7
#define ADC_ATTENUATION             ADC_ATTEN_DB_12
#define ADC_MAX_RAW                 4095    // 12-bit АЦП
#define VREF_mV                     3300    // опорное напряжение мВ для расчета вручную

static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle);
static void adc_calibration_deinit(adc_cali_handle_t handle);

void app_main(void)
{
    static int raw1, raw2;
    static int voltage1, voltage2;

    // Инициализация АЦП
    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

    // Настройка двух каналов
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTENUATION,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_1, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_2, &config));
    
    // Калибровка АЦП для первого канала
    adc_cali_handle_t adc_calibration_handle = NULL;
    bool do_calibration = adc_calibration_init(ADC_UNIT, ADC_CHANNEL_1, ADC_ATTENUATION, &adc_calibration_handle);

    while (1) {
        // Чтение сырых данных
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL_1, &raw1));
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL_2, &raw2));

        // Перевод в напряжение вручную
        float voltage_manual1 = ((float)raw1 / ADC_MAX_RAW) * VREF_mV;
        float voltage_manual2 = ((float)raw2 / ADC_MAX_RAW) * VREF_mV;

        printf("Channel 1: Raw = %d, Voltage (manual) = %.2f mV", raw1, voltage_manual1);
        printf(", Voltage (calibrated) = ");
        if (do_calibration) {
            ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc_calibration_handle, raw1, &voltage1));
            printf("%d mV\n", voltage1);
        } else {
            printf("N/A\n");
        }

        printf("Channel 2: Raw = %d, Voltage (manual) = %.2f mV", raw2, voltage_manual2);
        printf(", Voltage (calibrated) = ");
        if (do_calibration) {
            ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc_calibration_handle, raw2, &voltage2));
            printf("%d mV\n", voltage2);
        } else {
            printf("N/A\n");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // Очистка ресурсов
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc_handle));
    if (do_calibration) {
        adc_calibration_deinit(adc_calibration_handle);
    }
}

/*---------------------------------------------------------------
        Калибровка АЦП
---------------------------------------------------------------*/
static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!calibrated) {
        printf("Calibration scheme is \"Curve Fitting\"\n");
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit,
            .chan = channel,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
        if (ret == ESP_OK) calibrated = true;
    }
#endif
#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!calibrated) {
        printf("Calibration scheme is \"Line Fitting\"\n");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK) calibrated = true;
    }
#endif

    *out_handle = handle;
    if (ret == ESP_OK) {
        printf("Calibration Success\n");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        printf("eFuse not burnt, skip software calibration\n");
    } else {
        printf("Invalid arg or no memory\n");
    }

    return calibrated;
}

static void adc_calibration_deinit(adc_cali_handle_t handle)
{
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    printf("deregister \"Curve Fitting\" calibration scheme\n");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(handle));
#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    printf("deregister \"Line Fitting\" calibration scheme\n");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(handle));
#endif
}


// #include <stdio.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "driver/ledc.h"
// #include "esp_err.h"

// #define LEDC_TIMER          LEDC_TIMER_0
// #define LEDC_MODE           LEDC_LOW_SPEED_MODE
// #define LEDC_GPIO_R         GPIO_NUM_14
// #define LEDC_CHANNEL_R      LEDC_CHANNEL_0
// #define LEDC_GPIO_G         GPIO_NUM_27
// #define LEDC_CHANNEL_G      LEDC_CHANNEL_1
// #define LEDC_GPIO_B         GPIO_NUM_26
// #define LEDC_CHANNEL_B      LEDC_CHANNEL_2

// #define LEDC_DUTY_RES       LEDC_TIMER_13_BIT
// #define LEDC_FADE_TIME      1000

// #define RGB_TO_DUTY(x)  ((x) * (1 << LEDC_DUTY_RES) / 255)

// typedef struct {
//     uint8_t r;
//     uint8_t g;
//     uint8_t b;
// } rgb_t;

// void app_main(void)
// {
//     // Настройка таймера
//     ledc_timer_config_t ledc_timer = {
//         .duty_resolution = LEDC_DUTY_RES,
//         .freq_hz = 4000,
//         .speed_mode = LEDC_MODE,
//         .timer_num = LEDC_TIMER,
//         .clk_cfg = LEDC_AUTO_CLK,
//     };
//     ledc_timer_config(&ledc_timer);

//     // Настройка каналов
//     ledc_channel_config_t channels[3] = {
//         { .channel=LEDC_CHANNEL_R, .duty=0, .gpio_num=LEDC_GPIO_R, .speed_mode=LEDC_MODE, .hpoint=0, .timer_sel=LEDC_TIMER },
//         { .channel=LEDC_CHANNEL_G, .duty=0, .gpio_num=LEDC_GPIO_G, .speed_mode=LEDC_MODE, .hpoint=0, .timer_sel=LEDC_TIMER },
//         { .channel=LEDC_CHANNEL_B, .duty=0, .gpio_num=LEDC_GPIO_B, .speed_mode=LEDC_MODE, .hpoint=0, .timer_sel=LEDC_TIMER }
//     };
//     for(int i = 0; i < 3; i++)
//         ledc_channel_config(&channels[i]);

//     ledc_fade_func_install(0);

//     rgb_t colors[7] = {
//         {255,   0,   0}, // Красный
//         {255, 127,   0}, // Оранжевый
//         {255, 255,   0}, // Желтый
//         {  0, 255,   0}, // Зеленый
//         {  0, 255, 255}, // Голубой
//         {  0,   0, 255}, // Синий
//         {255,   0, 255}  // Фиолетовый
//     };

//     uint8_t state = 0;
//     while(1)
//     {
//         // Установка плавного изменения для всех каналов
//         uint32_t duty_values[3] = {
//             RGB_TO_DUTY(colors[state].r),
//             RGB_TO_DUTY(colors[state].g),
//             RGB_TO_DUTY(colors[state].b)
//         };

//         for(int i = 0; i < 3; i++)
//             ledc_set_fade_with_time(channels[i].speed_mode, channels[i].channel, duty_values[i], LEDC_FADE_TIME);

//         // Запуск fade для всех каналов одновременно
//         for(int i = 0; i < 3; i++)
//             ledc_fade_start(channels[i].speed_mode, channels[i].channel, LEDC_FADE_WAIT_DONE);

//         state = (state + 1) % 7;
//     }
// }

