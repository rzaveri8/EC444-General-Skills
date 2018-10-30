/* LEDC (LED Controller) fade example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "driver/uart.h"

/*
 * About this example
 *
 * 1. Start with initializing LEDC module:
 *    a. Set the timer of LEDC first, this determines the frequency
 *       and resolution of PWM.
 *    b. Then set the LEDC channel you want to use,
 *       and bind with one of the timers.
 *
 * 2. You need first to install a default fade function,
 *    then you can use fade APIs.
 *
 * 3. You can also set a target duty directly without fading.
 *
 * 4. This example uses GPIO18/19/4/5 as LEDC output,
 *    and it will change the duty repeatedly.
 *
 * 5. GPIO18/19 are from high speed channel group.
 *    GPIO4/5 are from low speed channel group.
 *
 */

 #define ECHO_TEST_TXD  (GPIO_NUM_4)
 #define ECHO_TEST_RXD  UART_NUM_0
 #define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
 #define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)


#define LEDC_HS_TIMER          LEDC_TIMER_0
#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
#define LEDC_HS_CH0_GPIO       (18)
#define LEDC_HS_CH0_CHANNEL    LEDC_CHANNEL_0
#define LEDC_HS_CH1_GPIO       (19)
#define LEDC_HS_CH1_CHANNEL    LEDC_CHANNEL_1

#define LEDC_LS_TIMER          LEDC_TIMER_1
#define LEDC_LS_MODE           LEDC_LOW_SPEED_MODE
#define LEDC_LS_CH2_GPIO       (4)
#define LEDC_LS_CH2_CHANNEL    LEDC_CHANNEL_2
#define LEDC_LS_CH3_GPIO       (5)
#define LEDC_LS_CH3_CHANNEL    LEDC_CHANNEL_3

#define LEDC_TEST_CH_NUM       (4)
#define LEDC_TEST_DUTY1         (800)
#define LEDC_TEST_DUTY2         (1600)
#define LEDC_TEST_DUTY3         (2400)
#define LEDC_TEST_DUTY4         (3200)
#define LEDC_TEST_DUTY5         (4000)
#define LEDC_TEST_FADE_TIME    (3000)

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
    int ch;

    /*
     * Prepare and set configuration of timers
     * that will be used by LED Controller
     */
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_13_BIT, // resolution of PWM duty
        .freq_hz = 5000,                      // frequency of PWM signal
        .speed_mode = LEDC_HS_MODE,           // timer mode
        .timer_num = LEDC_HS_TIMER            // timer index
    };
    // Set configuration of timer0 for high speed channels
    ledc_timer_config(&ledc_timer);

    // Prepare and set configuration of timer1 for low speed channels
    ledc_timer.speed_mode = LEDC_LS_MODE;
    ledc_timer.timer_num = LEDC_LS_TIMER;
    ledc_timer_config(&ledc_timer);

    /*
     * Prepare individual configuration
     * for each channel of LED Controller
     * by selecting:
     * - controller's channel number
     * - output duty cycle, set initially to 0
     * - GPIO number where LED is connected to
     * - speed mode, either high or low
     * - timer servicing selected channel
     *   Note: if different channels use one timer,
     *         then frequency and bit_num of these channels
     *         will be the same
     */
    ledc_channel_config_t ledc_channel[LEDC_TEST_CH_NUM] = {
        {
            .channel    = LEDC_HS_CH0_CHANNEL,
            .duty       = 0,
            .gpio_num   = LEDC_HS_CH0_GPIO,
            .speed_mode = LEDC_HS_MODE,
            .timer_sel  = LEDC_HS_TIMER
        },
        {
            .channel    = LEDC_HS_CH1_CHANNEL,
            .duty       = 0,
            .gpio_num   = LEDC_HS_CH1_GPIO,
            .speed_mode = LEDC_HS_MODE,
            .timer_sel  = LEDC_HS_TIMER
        },
        {
            .channel    = LEDC_LS_CH2_CHANNEL,
            .duty       = 0,
            .gpio_num   = LEDC_LS_CH2_GPIO,
            .speed_mode = LEDC_LS_MODE,
            .timer_sel  = LEDC_LS_TIMER
        },
        {
            .channel    = LEDC_LS_CH3_CHANNEL,
            .duty       = 0,
            .gpio_num   = LEDC_LS_CH3_GPIO,
            .speed_mode = LEDC_LS_MODE,
            .timer_sel  = LEDC_LS_TIMER
        },
    };

    for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
        ledc_channel_config(&ledc_channel[ch]);
    }

    // Initialize fade service.
    ledc_fade_func_install(0);

    while (1) {
      // Read data from the UART
      int len = uart_read_bytes(UART_NUM_0, data, BUF_SIZE, 20 / portTICK_RATE_MS);
      // Write data back to the UART
      uart_write_bytes(UART_NUM_0, (const char *) data, len);



    //    printf("3. LEDC set duty = %d without fade\n", LEDC_TEST_DUTY);
        //for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
            if (len > 0){
              if (data[0] == '0'){
                for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
                printf(" LEDC set to 0\n");
            ledc_set_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, 0 );
            ledc_update_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel);
          }
          }

          else if(data[0] == '1'){
            for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
            printf(" LEDC set to level 1 brightness\n");
            ledc_set_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, LEDC_TEST_DUTY1 );
            ledc_update_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel);
          }
          }

          else if(data[0] == '2'){
            for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
            printf(" LEDC set to level 2 brightness\n");
            ledc_set_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, LEDC_TEST_DUTY2 );
            ledc_update_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel);
          }
          }

          else if(data[0] =='3') {
            for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
            printf(" LEDC set to level 3 brightness\n");
            ledc_set_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, LEDC_TEST_DUTY3 );
            ledc_update_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel);
          }
          }

          else if(data[0] == '4'){
            for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
            printf(" LEDC set to level 4 brightness\n");
            ledc_set_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, LEDC_TEST_DUTY4 );
            ledc_update_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel);
          }
          }

          else if(data[0] =='5' ) {
            for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
            printf(" LEDC set to level 5 brightness\n");
            ledc_set_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, LEDC_TEST_DUTY5 );
            ledc_update_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel);
          }
          }

        }


        // vTaskDelay(1000 / portTICK_PERIOD_MS);
        //
        // printf("4. LEDC set duty = 0 without fade\n");
        // for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
        //     ledc_set_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, 0);
        //     ledc_update_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel);
        // }
        //vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

}

void app_main()
{
    xTaskCreate(echo_task, "uart_echo_task", 1024, NULL, 5, NULL);
}
