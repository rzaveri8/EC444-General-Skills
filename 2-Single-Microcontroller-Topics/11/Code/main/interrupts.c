// Rubeena Zaveri Skill 17 2018-09-26


#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include <esp_log.h>
#include "sdkconfig.h"
#include "esp_intr_alloc.h"
//#include <stdlib.h.>

#define LED1_GPIO (12)
#define LED2_GPIO (13)
#define LED3_GPIO (33)
#define LED4_GPIO (15)
#define BUTTON_GPIO (4)
#define ESP_INTR_FLAG_DEFAULT 0
int btn1_state = 0;
int btn2_state = 0;
int btn3_state = 0;
int btn4_state = 0;
int count = 0;


static void IRAM_ATTR isr_button_pressed(void *args)
{
  count= count +1;
  if(count ==1) {
  btn1_state = 1;
  btn2_state = 0;
  btn3_state = 0;
  btn4_state = 0;
  //count = count+1;
}
else if(count ==2) {
  btn1_state = 0;
  btn2_state = 1;
  btn3_state = 0;
  btn4_state = 0;
//  count = count +1;
}

 else if(count ==3) {
  btn1_state = 0;
  btn2_state = 0;
  btn3_state = 1;
  btn4_state = 0;
  //  count = count +1;
}

 else if(count ==4) {
  btn1_state = 0;
  btn2_state = 0;
  btn3_state = 0;
  btn4_state = 1;
  //  count = count +1;
}

//count =0;
}




static void button_task() {
  gpio_set_level(LED1_GPIO, btn1_state);
  gpio_set_level(LED2_GPIO, btn2_state);
  gpio_set_level(LED3_GPIO, btn3_state);
  gpio_set_level(LED4_GPIO, btn4_state);
  // btn1_state = 0;
  // btn2_state = 0;
  // btn3_state = 0;
  // btn4_state = 0;
  if (count >4) {
    count = 0;
  }
//  vTaskDelay(500/portTICK_PERIOD_MS);
}

void config () {

  //Configure button
  gpio_config_t btn_config;
  btn_config.intr_type = GPIO_PIN_INTR_POSEDGE;    //Enable interrupt on both rising and falling edges
  btn_config.mode = GPIO_MODE_INPUT;           //Set as Input
  btn_config.pin_bit_mask = (1ULL << BUTTON_GPIO); //Bitmask
  btn_config.pull_up_en = 1;    //Disable pullup
//  btn_config.pull_down_en = GPIO_PULLDOWN_DISABLE; //Enable pulldown
  gpio_config(&btn_config);
  printf("Button configured\n");
  gpio_intr_enable(BUTTON_GPIO);

  //Configure LEDS
  gpio_pad_select_gpio(LED1_GPIO);               //Set pin as led 1 GPIO
  gpio_set_direction(LED1_GPIO, GPIO_MODE_OUTPUT);   //Set as Output

   gpio_pad_select_gpio(LED2_GPIO);               //Set pin as led 2 GPIO
  gpio_set_direction(LED2_GPIO, GPIO_MODE_OUTPUT);   //Set as Output

  gpio_pad_select_gpio(LED3_GPIO);               //Set pin as led 3 GPIO
  gpio_set_direction(LED3_GPIO, GPIO_MODE_OUTPUT);   //Set as Output

  gpio_pad_select_gpio(LED4_GPIO);               //Set pin as led 4 GPIO
  gpio_set_direction(LED4_GPIO, GPIO_MODE_OUTPUT);   //Set as Output */
  printf("ALL LEDs configured\n");

}

void interrupt () {
//Configure interrupt and add handler
                  //Start Interrupt Service Routine service
gpio_install_isr_service(ESP_INTR_FLAG_LEVEL3);
gpio_isr_handler_add(BUTTON_GPIO, isr_button_pressed, (void*)BUTTON_GPIO);

}

void app_main()
{
  config();
  interrupt ();


  printf("Interrupt configured\n");
  while(1) {
  //  gpio_isr_handler_remove(BUTTON_GPIO);
     //interrupt ();
      vTaskDelay(100/portTICK_PERIOD_MS);

     button_task();



     //vTaskDelay(500/portTICK_PERIOD_MS);
     //button_task11();
    //  interrupt ();
    //  vTaskDelay(1000/portTICK_PERIOD_MS);
    //  button_task2();
    //
    // // interrupt ();
    //  //vTaskDelay(500/portTICK_PERIOD_MS);
    // // button_task22();
    //  interrupt ();
    //  vTaskDelay(1000/portTICK_PERIOD_MS);
    //  button_task3();
    //
    // // interrupt ();
    // // vTaskDelay(500/portTICK_PERIOD_MS);
    //  //button_task33();
    //  interrupt ();
    //  vTaskDelay(1000/portTICK_PERIOD_MS);
    //  button_task4();
    //
    //  // interrupt ();
    //   //vTaskDelay(500/portTICK_PERIOD_MS);
    //   //button_task44();
    //   //vTaskDelay(100/portTICK_PERIOD_MS);




  }
}
