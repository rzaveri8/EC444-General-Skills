// Rubeena Zaveri Skill 17 2018-09-26

#include <stdio.h>
#include "driver/i2c.h"

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

		uint16_t digit[4];
		digit[0] = 0x00FF; //H 0b0000000011110110
		digit[1] = 0x0006; //E 0b0000000011111001
		digit[2] = 0x008F; //Y 0b0000000011101110
		digit[3] = 0x00E6; //Y 0b0000000011101110

		ret = i2c_master_write_nums(I2C_MASTER_NUM, &digit); //write data to display
}

void app_main()
{
    i2c_master_init();
		test();
}
