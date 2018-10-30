// binary Blink
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

/* Can run 'make menuconfig' to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BIN_1 (12)
#define BIN_2 (27)
#define BIN_4 (13)
#define BIN_8 (14)

void blink_task(void *pvParameter)
{
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    gpio_pad_select_gpio(BIN_2);
    gpio_pad_select_gpio(BIN_1);
    gpio_pad_select_gpio(BIN_4);
    gpio_pad_select_gpio(BIN_8);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BIN_2, GPIO_MODE_OUTPUT);
    gpio_set_direction(BIN_1, GPIO_MODE_OUTPUT);
    gpio_set_direction(BIN_4, GPIO_MODE_OUTPUT);
    gpio_set_direction(BIN_8, GPIO_MODE_OUTPUT);


    int count = 0;
    int temp_count =0;

    while(1) {
        /* Blink off (output low) */
        gpio_set_level(BIN_8, 0);
        gpio_set_level(BIN_4, 0);
        gpio_set_level(BIN_2, 0);
        gpio_set_level(BIN_1, 0);

        temp_count = count;

        if (temp_count / 8 != 0) {
          gpio_set_level(BIN_8, 1);
          temp_count = temp_count % 8;
        }

        if (temp_count / 4 != 0) {
          gpio_set_level(BIN_4, 1);
          temp_count = temp_count % 4;
        }

        if (temp_count / 2 != 0) {
          gpio_set_level(BIN_2, 1);
          temp_count = temp_count % 2;
        }

        if (temp_count / 1 != 0) {
          gpio_set_level(BIN_1, 1);
          temp_count = temp_count % 1;
        }

        count = count +1;
        if (count ==16) {
          count = 0;
        }

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void app_main()
{
    xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
}
