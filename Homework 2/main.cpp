/*
 * Solo Canal 5 -> % de Humedad
 * Ecuación: HR = 3.0504 * exp(0.0012 * mV)
 */
#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#define ADC_ATTEN_USED      ADC_ATTEN_DB_12
#define CLAMP_HUMIDITY_0_100 1   // pon 0 si no quieres limitar 0–100%

// ---- Selección del canal físico para “canal 5” en ADC1 ----
#if CONFIG_IDF_TARGET_ESP32
  #define SENSOR_CH   ADC_CHANNEL_5   // ADC1_CH5 en ESP32
#else
  #define SENSOR_CH   ADC_CHANNEL_3   // ajusta si tu target difiere
#endif

static inline float humidity_from_mv(float mv)
{
    const float A = 3.0504f, B = 0.0012f;
    float y = A * expf(B * mv);
#if CLAMP_HUMIDITY_0_100
    if (y < 0.0f) y = 0.0f;
    if (y > 100.0f) y = 100.0f;
#endif
    return y;
}

static bool adc_calib_init(adc_unit_t unit, adc_channel_t ch, adc_atten_t att, adc_cali_handle_t *out)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool ok = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!ok) {
        adc_cali_curve_fitting_config_t cfg = {
            .unit_id = unit, .chan = ch, .atten = att, .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cfg, &handle);
        if (ret == ESP_OK) ok = true;
    }
#endif
#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!ok) {
        adc_cali_line_fitting_config_t cfg = {
            .unit_id = unit, .atten = att, .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cfg, &handle);
        if (ret == ESP_OK) ok = true;
    }
#endif
    *out = handle;
    return ok;
}

static void adc_calib_deinit(adc_cali_handle_t handle)
{
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    adc_cali_delete_scheme_curve_fitting(handle);
#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    adc_cali_delete_scheme_line_fitting(handle);
#endif
}

void app_main(void)
{
    // Unidad ADC1
    adc_oneshot_unit_handle_t adc1;
    adc_oneshot_unit_init_cfg_t ucfg = { .unit_id = ADC_UNIT_1 };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&ucfg, &adc1));

    // Config canal 5
    adc_oneshot_chan_cfg_t ccfg = { .atten = ADC_ATTEN_USED, .bitwidth = ADC_BITWIDTH_DEFAULT };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1, SENSOR_CH, &ccfg));

    // Calibración para obtener mV
    adc_cali_handle_t cali = NULL;
    bool have_cal = adc_calib_init(ADC_UNIT_1, SENSOR_CH, ADC_ATTEN_USED, &cali);

    // Bucle: solo imprimir % de humedad
    while (1) {
        int raw = 0, mv = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(adc1, SENSOR_CH, &raw));
        if (have_cal) {
            ESP_ERROR_CHECK(adc_cali_raw_to_voltage(cali, raw, &mv));
            float rh = humidity_from_mv((float)mv);
            // ÚNICA SALIDA:
            printf("Humedad: %.1f %%\n", rh);
        } else {
            // Sin calibración no conocemos mV con precisión.
            // Si necesitas una aproximación, dímelo y la añadimos.
            printf("Humedad: N/A (falta calibración)\n");
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // Nunca se alcanza, pero lo correcto sería limpiar:
    // adc_oneshot_del_unit(adc1);
    // if (have_cal) adc_calib_deinit(cali);
}
