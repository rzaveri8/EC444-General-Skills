/* brushed dc motor control example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/*
 * This example will show you how to use MCPWM module to control brushed dc motor.
 * This code is tested with L298 motor driver.
 * User may need to make changes according to the motor driver they use.
*/

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_attr.h"
#include "driver/gpio.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

#define GPIO_PWM0A_OUT 32   //Set GPIO 32 as PWM0A
#define GPIO_PWM0B_OUT 14   //Set GPIO 14 as PWM0B
#define GPIO_MOTOR1_0  13
#define GPIO_MOTOR1_1  12
#define GPIO_MOTOR2_0  27
#define GPIO_MOTOR2_1  33

static void mcpwm_example_gpio_initialize()
{
	//only using one of the MCPWM units
    printf("initializing mcpwm gpio...\n");
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, GPIO_PWM0A_OUT);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, GPIO_PWM0B_OUT);
}

/**
 * @brief motor moves in forward direction, with duty cycle = duty %
 */
static void brushed_motor_forward(float duty_cycle)
{

    gpio_set_level(GPIO_MOTOR1_0, 1);
    gpio_set_level(GPIO_MOTOR1_1, 0);
    gpio_set_level(GPIO_MOTOR2_0, 0);
    gpio_set_level(GPIO_MOTOR2_1, 1);

   // mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0); //call this each time, if operator was previously in low/high state
   // mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, MCPWM_DUTY_MODE_0); //call this each time, if operator was previously in low/high state

    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, duty_cycle);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, duty_cycle);

}

/**
 * @brief motor moves in backward direction, with duty cycle = duty %
 */
static void brushed_motor_backward(float duty_cycle)
{
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0); //call this each time, if operator was previously in low/high state
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, MCPWM_DUTY_MODE_0); //call this each time, if operator was previously in low/high state
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, duty_cycle);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, duty_cycle);

    gpio_set_level(GPIO_MOTOR1_0, 0);
    gpio_set_level(GPIO_MOTOR1_1, 1);
    gpio_set_level(GPIO_MOTOR2_0, 1);
    gpio_set_level(GPIO_MOTOR2_1, 0);
}

/**
 * @brief motor stop
 */
static void brushed_motor_stop()
{
   // mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0); //call this each time, if operator was previously in low/high state
   // mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, MCPWM_DUTY_MODE_0); //call this each time, if operator was previously in low/high state

	mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 0.0);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, 0.0);

}

 static void brushed_motor_turn_left(float duty_cycle)
 {
  // mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0); //call this each time, if operator was previously in low/high state
  // mcpwm_set_duty_type(MCPWM_UNIT_1, MCPWM_TIMER_1, MCPWM_OPR_A, MCPWM_DUTY_MODE_0); //call this each time, if operator was previously in low/high state

   mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 0.0);
   mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, duty_cycle);

   gpio_set_level(GPIO_MOTOR2_0, 1);
   gpio_set_level(GPIO_MOTOR2_1, 0);
 }


  static void brushed_motor_turn_right(float duty_cycle)
  {
   // mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0); //call this each time, if operator was previously in low/high state
   // mcpwm_set_duty_type(MCPWM_UNIT_1, MCPWM_TIMER_1, MCPWM_OPR_A, MCPWM_DUTY_MODE_0); //call this each time, if operator was previously in low/high state

    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, 0.0);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, duty_cycle);

    gpio_set_level(GPIO_MOTOR2_0, 1);
    gpio_set_level(GPIO_MOTOR2_1, 0);
  }
/**
 * @brief Configure MCPWM module for brushed dc motor
 */
static void mcpwm_example_brushed_motor_control(void *arg)
{
    //1. mcpwm gpio initialization
    mcpwm_example_gpio_initialize();

    gpio_pad_select_gpio(GPIO_MOTOR1_0);
    gpio_pad_select_gpio(GPIO_MOTOR1_1);
    gpio_pad_select_gpio(GPIO_MOTOR2_0);
    gpio_pad_select_gpio(GPIO_MOTOR2_1);

    gpio_set_direction(GPIO_MOTOR1_0, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_MOTOR1_1, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_MOTOR2_0, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_MOTOR2_1, GPIO_MODE_OUTPUT);

    gpio_set_level(GPIO_MOTOR1_0, 0);
    gpio_set_level(GPIO_MOTOR1_1, 1);
    gpio_set_level(GPIO_MOTOR2_0, 1);
    gpio_set_level(GPIO_MOTOR2_1, 0);

    //2. initial mcpwm configuration
    printf("Configuring Initial Parameters of mcpwm...\n");
    mcpwm_config_t pwm_config;

	//only need to initialize one unit
    pwm_config.frequency = 100;    //frequency = 500Hz,
    pwm_config.cmpr_a = 0;    //duty cycle of PWMxA = 0
   	pwm_config.cmpr_b = 0;	//duty cycle of PWMxB = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);    //Configure PWM with above settings

    while (1) {
		mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0); //sets duty type of PWM0A
		mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, MCPWM_DUTY_MODE_0); //sets duty type of PWM0B
		brushed_motor_forward(90.0); //drive forward with duty cycle of 90
		vTaskDelay(1000/portTICK_RATE_MS);
		brushed_motor_backward(50.0); //drive backward with duty cycle of 50%
		vTaskDelay(1000/portTICK_RATE_MS);//delay 1 second
    }
}

void app_main()
{
    printf("Testing brushed motor...\n");
    xTaskCreate(mcpwm_example_brushed_motor_control, "mcpwm_example_brushed_motor_control", 4096, NULL, 5, NULL);
}
