#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#define INPUT_PIN (5) //A5
#define OUTPUT_PIN (27)

void app_main()
{
  gpio_pad_select_gpio(INPUT_PIN);
  gpio_pad_select_gpio(OUTPUT_PIN);
  gpio_set_direction(INPUT_PIN, GPIO_MODE_INPUT);
  gpio_set_direction(OUTPUT_PIN, GPIO_MODE_OUTPUT);
  while(1) {
    int level =  gpio_get_level(INPUT_PIN);
    gpio_set_level(OUTPUT_PIN, level);
    printf("%d\n", level);
    fflush(stdout);
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}
