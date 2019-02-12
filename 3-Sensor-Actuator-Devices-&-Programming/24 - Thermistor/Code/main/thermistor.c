#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "soc/uart_struct.h"
#include "string.h"
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include <math.h>

#define SAMPLE_NUM 64
#define DEFAULT_VREF 1100

static esp_adc_cal_characteristics_t  *adc_chars;
static const adc1_channel_t channel = ADC1_CHANNEL_3; //GPIO 39
static const adc_atten_t  atten = ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_1;

void init_thermistor() {
  //configure ADC
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(channel, atten);
  adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
}

float gettemp() {
  float temp;
  //Initialize reading variable
  uint32_t val = 0;
  for (int i = 0; i < SAMPLE_NUM; i++){
    val += adc1_get_raw(channel);
  }
  val /= SAMPLE_NUM;
  printf("Val: %d\n", val);
  float rtherm;
  rtherm = val;
  rtherm = (5700 * ((4096 / rtherm) - 1));
  printf("rtherm: %.6f\n", rtherm);
  temp = rtherm / 10000;     // (R/Ro)
  temp = log(temp);                  // ln(R/Ro)
  temp /= 3435;                   // 1/B * ln(R/Ro)
  temp += 1.0 / (20 + 273.15); // + (1/To)
  temp = 1.0 / temp;                 // Invert
  temp -= 273.15;
  return temp;
}
void app_main() {
  init_thermistor();
  while (1) {
    float temp = gettemp();
    printf("Temp: %.6f\n", temp);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}
