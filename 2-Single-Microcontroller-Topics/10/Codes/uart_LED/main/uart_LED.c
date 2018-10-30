/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "driver/uart.h"

/* Can run 'make menuconfig' to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define ECHO_TEST_TXD  (GPIO_NUM_4)
#define ECHO_TEST_RXD  UART_NUM_0
#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)

#define LED0_GPIO 13
#define LED1_GPIO 12
#define LED2_GPIO 27
#define LED3_GPIO 33

#define BUF_SIZE        (1024)

static void echo_task(void *arg)
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_0, BUF_SIZE * 2, 0, 0, NULL, 0);

    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

    while (1) {
        // Read data from the UART
        int len = uart_read_bytes(UART_NUM_0, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        // Write data back to the UART
        uart_write_bytes(UART_NUM_0, (const char *) data, len);
        if (len > 0){
           gpio_pad_select_gpio(LED0_GPIO);
           gpio_pad_select_gpio(LED1_GPIO);
           gpio_pad_select_gpio(LED2_GPIO);
           gpio_pad_select_gpio(LED3_GPIO);

           gpio_set_direction(LED0_GPIO, GPIO_MODE_OUTPUT);
           gpio_set_direction(LED1_GPIO, GPIO_MODE_OUTPUT);
           gpio_set_direction(LED2_GPIO, GPIO_MODE_OUTPUT);
           gpio_set_direction(LED3_GPIO, GPIO_MODE_OUTPUT);

           if (data[0] == '0'){
             gpio_set_level(LED0_GPIO, 0);
             gpio_set_level(LED1_GPIO, 0);
             gpio_set_level(LED2_GPIO, 0);
             gpio_set_level(LED3_GPIO, 0);
           }
           else if(data[0] == '1'){
             gpio_set_level(LED0_GPIO, 1);
             gpio_set_level(LED1_GPIO, 0);
             gpio_set_level(LED2_GPIO, 0);
             gpio_set_level(LED3_GPIO, 0);
           }
           else if(data[0] == '2'){
             gpio_set_level(LED0_GPIO, 0);
             gpio_set_level(LED1_GPIO, 1);
             gpio_set_level(LED2_GPIO, 0);
             gpio_set_level(LED3_GPIO, 0);
           }
           else if(data[0] == '3'){
             gpio_set_level(LED0_GPIO, 1);
             gpio_set_level(LED1_GPIO, 1);
             gpio_set_level(LED2_GPIO, 0);
             gpio_set_level(LED3_GPIO, 0);
           }
           else if(data[0] == '4'){
             gpio_set_level(LED0_GPIO, 0);
             gpio_set_level(LED1_GPIO, 0);
             gpio_set_level(LED2_GPIO, 1);
             gpio_set_level(LED3_GPIO, 0);
           }
           else if(data[0] == '5'){
             gpio_set_level(LED0_GPIO, 1);
             gpio_set_level(LED1_GPIO, 0);
             gpio_set_level(LED2_GPIO, 1);
             gpio_set_level(LED3_GPIO, 0);
           }
           else if(data[0] == '6'){
             gpio_set_level(LED0_GPIO, 0);
             gpio_set_level(LED1_GPIO, 1);
             gpio_set_level(LED2_GPIO, 1);
             gpio_set_level(LED3_GPIO, 0);
           }
           else if(data[0] == '7'){
             gpio_set_level(LED0_GPIO, 1);
             gpio_set_level(LED1_GPIO, 1);
             gpio_set_level(LED2_GPIO, 1);
             gpio_set_level(LED3_GPIO, 0);
           }
           else if(data[0] == '8'){
             gpio_set_level(LED0_GPIO, 0);
             gpio_set_level(LED1_GPIO, 0);
             gpio_set_level(LED2_GPIO, 0);
             gpio_set_level(LED3_GPIO, 1);
           }
           else if(data[0] == '9'){
             gpio_set_level(LED0_GPIO, 1);
             gpio_set_level(LED1_GPIO, 0);
             gpio_set_level(LED2_GPIO, 0);
             gpio_set_level(LED3_GPIO, 1);
           }
           else if(data[0] == 'A' || data[0] == 'a'){
             gpio_set_level(LED0_GPIO, 0);
             gpio_set_level(LED1_GPIO, 1);
             gpio_set_level(LED2_GPIO, 0);
             gpio_set_level(LED3_GPIO, 1);
           }
           else if(data[0] == 'B'||data[0] == 'b'){
             gpio_set_level(LED0_GPIO, 1);
             gpio_set_level(LED1_GPIO, 1);
             gpio_set_level(LED2_GPIO, 0);
             gpio_set_level(LED3_GPIO, 1);
           }
           else if(data[0] == 'C'|| data[0] == 'c'){
             gpio_set_level(LED0_GPIO, 0);
             gpio_set_level(LED1_GPIO, 0);
             gpio_set_level(LED2_GPIO, 1);
             gpio_set_level(LED3_GPIO, 1);
           }
           else if(data[0] == 'D'||data[0] == 'd'){
             gpio_set_level(LED0_GPIO, 1);
             gpio_set_level(LED1_GPIO, 0);
             gpio_set_level(LED2_GPIO, 1);
             gpio_set_level(LED3_GPIO, 1);
           }
           else if(data[0] == 'E' || data[0] == 'e'){
             gpio_set_level(LED0_GPIO, 0);
             gpio_set_level(LED1_GPIO, 1);
             gpio_set_level(LED2_GPIO, 1);
             gpio_set_level(LED3_GPIO, 1);
           }
           else if(data[0] == 'F' || data[0] == 'f'){
             gpio_set_level(LED0_GPIO, 1);
             gpio_set_level(LED1_GPIO, 1);
             gpio_set_level(LED2_GPIO, 1);
             gpio_set_level(LED3_GPIO, 1);
           }
        }
    }
}


void app_main()
{
    xTaskCreate(echo_task, "uart_echo_task", 1024, NULL, 5, NULL);
}
