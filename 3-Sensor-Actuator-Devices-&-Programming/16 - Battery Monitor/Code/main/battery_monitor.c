/*
    General Idea: Alphanumeric display communicates to ESP32 via I2C.

    More Detail: I2C is a way to communicate between a microcontroller ("master") and connected device(s) ("slave").
    It is useful because it only uses 2 pins from the microcontroller even if multiple devices are connected. Of course
    this poses a problem: how can the master transmit data to a specific device if there are multiple devices connected?
    The answer lies in the way the data is transmitted. Rather than the data being bluntly transmitted, it is a concatenation
    of multiple elements. The data starts with a start byte, includes an address that is assigned to the slave that is a
    recipient of the data, of course includes the data, and then asks for an acknowledgment from the slave. While this
    may seem like it's unnecessarily lengthy, it simplifies things by reducing the number of pins the ESP32 uses while
    avoiding the problem of data getting lost or transmitted to the wrong devices. Using this connection we can easily feed
    the alphanumeric board with the correct information even if we were to connect other devices :)

    Method:
      1. Configure Master and Slave components -- uses a struct called i2c_config_t with attributes such as mode, sda_io_num, and scl_io_num
      2. Install drivers -- done within the configuring step
      3. Write data from master to slave
          a. Initialize a command link -- this creates the space for the message that includes the data and address and such
          b. Load everything into the command link -- put the data, address, and bytes into the command link
      4. In the task function:
          a. Load the data (in this case, the numbers for the display)
          b. Invoke the function to write data to the slave
          c. Invoke the i2c_slave_read_buffer function to fully transmit the data

    Sources: i2c_example_main.c, Adafruit_LEDBackpack.h, https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/i2c.html#
    }
*/

#include "driver/i2c.h"
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include <math.h>
#define DATA_LENGTH                        512              /*!<Data buffer length for test buffer*/

#define I2C_MASTER_SCL_IO          22               /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO          23               /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM             I2C_NUM_0        /*!< I2C port number for master dev */
#define I2C_MASTER_TX_BUF_DISABLE  0                /*!< I2C master do not need buffer */
#define I2C_MASTER_RX_BUF_DISABLE  0                /*!< I2C master do not need buffer */
#define I2C_MASTER_FREQ_HZ         100000           /*!< I2C master clock frequency */

#define ESP_SLAVE_ADDR                     0x70             /*!< ESP32 slave address, you can set any 7bit value */
#define WRITE_BIT                          I2C_MASTER_WRITE /*!< I2C master write */
#define ACK_CHECK_EN                       0x1              /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS                      0x0              /*!< I2C master will not check ack from slave */
#define ACK_VAL                            0x0              /*!< I2C ack value */
#define NACK_VAL                           0x1              /*!< I2C nack value */


//ADC Variables

#define SAMPLE_NUM    64
#define DEFAULT_VREF  1100

static esp_adc_cal_characteristics_t  *adc_chars;
static const adc1_channel_t channel = ADC1_CHANNEL_3; //GPIO 39
static const adc_atten_t  atten = ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_1;


///////////////////////////////////////////////////////////////
//																													 //
//											ADC FUNCTIONS												 //
//																													 //
///////////////////////////////////////////////////////////////

static void check_efuse()
{
    //Check TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("eFuse Two Point: NOT supported\n");
    }

    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        printf("eFuse Vref: Supported\n");
    } else {
        printf("eFuse Vref: NOT supported\n");
    }
}

static void print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Characterized using Two Point Value\n");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("Characterized using eFuse Vref\n");
    } else {
        printf("Characterized using Default Vref\n");
    }
}

void init_reader() {
  check_efuse();
  //configure ADC
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(channel, atten);
  adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
  print_char_val_type(val_type);
}

uint32_t read_voltage()
{
  uint32_t volts;
  //Initialize reading variable
  uint32_t val = 0;

  for (int i = 0; i < SAMPLE_NUM; i++){
    val += adc1_get_raw(channel);
  }
  val /= SAMPLE_NUM;

  volts = esp_adc_cal_raw_to_voltage(val, adc_chars);

  return volts;
}

///////////////////////////////////////////////////////////////
//																													 //
//											I2C FUNCTIONS												 //
//																													 //
///////////////////////////////////////////////////////////////
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

void i2c_master_init()
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

//main functionality
static void test(){
		int ret;

		//send command to turn on the oscillator
    uint8_t osc = 0x21;
    uint8_t * osc_point = &osc;
		ret = i2c_master_write_slave(I2C_MASTER_NUM, osc_point);
    if (ret==ESP_OK){
      printf("\nOscillator engaged\n\n");
    }

		//send a command to turn on the display
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
    }

		uint32_t data = read_voltage();
		printf("\n%d\n", data);
		uint16_t digit[4];
		if (data >= 1000) {
			digit[3] = numbertable[data % 10];
			data /= 10;
			digit[2] = numbertable[data%10];
			data /= 10;
			digit[1] = numbertable[data % 10];
			data /= 10;
			digit[0] = numbertable[data];
		}
		else {
			digit[3] = numbertable[data % 10];
			data /= 10;
			digit[2] = numbertable[data%10];
			data /= 10;
			digit[1] = numbertable[data % 10];
			digit[0] = 0x0000;
		}

		ret = i2c_master_write_nums(I2C_MASTER_NUM, &digit);
}

void app_main()
{
	while(1){
		init_reader();
    i2c_master_init();
		test();
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}
