// Rubeena Zaveri Skill 17 2018-09-26

#include <stdio.h>
#include "esp_attr.h"
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "soc/timer_group_struct.h"
#include "driver/mcpwm.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

#define DATA_LENGTH                        512              /*!<Data buffer length for test buffer*/

#define I2C_MASTER_SCL_IO          22               /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO          23               /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM             I2C_NUM_1        /*!< I2C port number for master dev */
#define I2C_MASTER_TX_BUF_DISABLE  0                /*!< I2C master do not need buffer */
#define I2C_MASTER_RX_BUF_DISABLE  0                /*!< I2C master do not need buffer */
#define I2C_MASTER_FREQ_HZ         100000           /*!< I2C master clock frequency */

#define ESP_SLAVE_ADDR                     0x70             /*!< ESP32 slave address, you can set any 7bit value */
#define WRITE_BIT                          I2C_MASTER_WRITE /*!< I2C master write */
#define ACK_CHECK_EN                       0x1              /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS                      0x0              /*!< I2C master will not check ack from slave */
#define ACK_VAL                            0x0              /*!< I2C ack value */
#define NACK_VAL                           0x1              /*!< I2C nack value */

// Hardware timer clock divider
#define TIMER_DIVIDER     (80)
// Convert counter value to seconds
#define TIMER_SCALE    (TIMER_BASE_CLK / TIMER_DIVIDER)
 // 1 second interval for timer
#define TIMER_INTERVAL_SEC    (1)


#define SECOND_INPUT (4)      // A5
#define MINUTE_INPUT (36)   // A4

volatile bool INTR_FLAG = 0;
volatile int SECOND = 0;
int MINUTE = 0;
int HOUR = 12;
volatile int ALARM_CNT;
bool ALARM;

static const uint16_t numbertable[] = {
0x0C3F, // 0 0b0000110000111111
0x0006, // 1
0x00DB, // 2
0x008F, // 3
0x00E6, // 4
0x2069, // 5
0x00FD, // 6
0x0007, // 7
0x00FF, // 8
0x00EF, // 9
};

/*
    Configure MASTER
*/

static void i2c_master_init()
{
		int err;
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    err = i2c_param_config(i2c_master_port, &conf);
		if (err == ESP_OK) { printf("\nParameters okay");}
    err = i2c_driver_install(i2c_master_port, conf.mode,
                       I2C_MASTER_RX_BUF_DISABLE,
                       I2C_MASTER_TX_BUF_DISABLE, 0);
		if (err == ESP_OK) { printf("\nDriver install okay");}
}

//function that creates command link and feeds data
static esp_err_t i2c_master_write_slave(i2c_port_t i2c_num, uint8_t* data_wr)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ( ESP_SLAVE_ADDR << 1 ) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, *data_wr, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

//function that creates command link specifically for display purposes
static esp_err_t i2c_master_write_nums(i2c_port_t i2c_num, uint8_t* data_wr)
{
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (ESP_SLAVE_ADDR << 1) | WRITE_BIT, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, 0x00, ACK_CHECK_EN); //0x00 address specifies that the first digit should be written
	for(int i = 0; i < 8; i++){
		//writes all the digits to the
		i2c_master_write_byte(cmd, data_wr[i], ACK_CHECK_EN);
	}
	i2c_master_stop(cmd);
	esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}

// Print current time to shell
static void timer_task() {
  gpio_pad_select_gpio(SECOND_INPUT);
  gpio_pad_select_gpio(MINUTE_INPUT);



  gpio_set_direction(SECOND_INPUT, GPIO_MODE_INPUT);
  gpio_set_direction(MINUTE_INPUT, GPIO_MODE_INPUT);


  //gpio_set_level(LED_OUTPUT, 0);

  // debounce button clicks
  bool SECOND_CLICK = 0;
  bool MINUTE_CLICK = 0;
 	int WATCH_val = 0;



  while (1) {

    // hour, minute, alarm buttons
    if(gpio_get_level(SECOND_INPUT) == 0) {
      SECOND_CLICK = 1;
    } else if (SECOND_CLICK == 1 && WATCH_val == 0) {
      //SECOND++;
      SECOND = 0;
      MINUTE = 0;
      printf("timer on\n");
      SECOND_CLICK = 0;
      WATCH_val = 1;;
    }
    if(gpio_get_level(SECOND_INPUT) == 0) {
      SECOND_CLICK = 1;
    } else if (SECOND_CLICK == 1  && WATCH_val == 1) {
      //SECOND++;
      timer_pause(0,0);
      printf("timer pause\n");
      SECOND_CLICK = 0;
      WATCH_val = 2;
    }

    if(gpio_get_level(SECOND_INPUT) == 0) {
      SECOND_CLICK = 1;
    } else if (SECOND_CLICK == 1 && WATCH_val == 2) {
      timer_start(0,0);
      //int that_SECOND = SECOND;
      //int that_MINUTE = MINUTE;
      printf("timer reset\n");
      SECOND_CLICK = 0;
      WATCH_val =0;
    }

    // each second, print current time to console
    if (INTR_FLAG) {
      printf("%02d:%02d\n",MINUTE, SECOND);
      INTR_FLAG = 0;
    }
    // This keeps the Task Watchdog happy
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}


void IRAM_ATTR timer_group0_isr(void *para) { // timer for parts
  int timer_idx = (int) para;
  uint32_t intr_status = TIMERG0.int_st_timers.val;
  if((intr_status & BIT(timer_idx)) && timer_idx == TIMER_0) {
    TIMERG0.hw_timer[timer_idx].update = 1;
    TIMERG0.int_clr_timers.t0 = 1;
    TIMERG0.hw_timer[timer_idx].config.alarm_en = 1;
    SECOND++;
    if (SECOND > 59) {
      SECOND = 0;
      MINUTE++;
    }
    if (MINUTE > 59) {
      MINUTE = 0;
      HOUR++;
    }
    if (ALARM && ALARM_CNT > 0) {
      ALARM_CNT--;
    }
    INTR_FLAG = 1;
  }
}

// Timer initialization
static void tg0_timer0_init() {
  int timer_group = TIMER_GROUP_0;
  int timer_idx = TIMER_0;
  timer_config_t config;
  config.alarm_en = 1;
  config.auto_reload = 1;
  config.counter_dir = TIMER_COUNT_UP;
  config.divider = TIMER_DIVIDER;
  config.intr_type = TIMER_INTR_LEVEL;
  config.counter_en = TIMER_PAUSE;
  timer_init(timer_group, timer_idx, &config);
  timer_pause(timer_group,timer_idx);
  timer_set_counter_value(timer_group, timer_idx, 0x00000000ULL);
  timer_set_alarm_value(timer_group, timer_idx, (TIMER_INTERVAL_SEC * TIMER_SCALE));
  timer_enable_intr(timer_group, timer_idx);
  timer_isr_register(timer_group, timer_idx, timer_group0_isr, (void*) timer_idx, ESP_INTR_FLAG_IRAM, NULL);
  timer_start(timer_group, timer_idx);
}




//main functionality
static void i2c_alphanumeric(){
		int ret;
		//send command to turn on the display oscillator
    uint8_t osc = 0x21;
    uint8_t * osc_point = &osc;
		ret = i2c_master_write_slave(I2C_MASTER_NUM, osc_point);
    if (ret==ESP_OK){
      printf("\nOscillator engaged\n\n");
    }

		//send a command to turn on the display oscillator
    uint8_t disp = 0xEF;
    uint8_t * disp_point = &disp;
		ret = i2c_master_write_slave(I2C_MASTER_NUM, disp_point);
    if (ret == ESP_OK){
      printf("\nDisplay engaged\n\n");
    }

		//send a command to turn the brightness
    uint8_t bright = 0x81;
    uint8_t * bright_point = &bright;
		ret = i2c_master_write_slave(I2C_MASTER_NUM, bright_point);
    if (ret == ESP_OK){
      printf("\nBrightness engaged");
      while(1) {
    		uint16_t digit[4];
        uint16_t seconds = SECOND;
        uint16_t minutes = MINUTE;
    		digit[1] = numbertable[minutes % 10];
    		minutes /= 10;
    		digit[0] = numbertable[minutes];
        digit[3] = numbertable[seconds % 10];
        seconds /= 10;
        digit[2] = numbertable[seconds];
    	  ret = i2c_master_write_nums(I2C_MASTER_NUM, &digit);
      }
  }
}

void app_main()
{
  tg0_timer0_init();
  i2c_master_init(); //initialize the master for i2c
  xTaskCreate(timer_task, "timer_task", 2048, NULL, 5, NULL);
  i2c_alphanumeric();

}
