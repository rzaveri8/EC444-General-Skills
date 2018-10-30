#  ALPHANUMERIC DISPLAY

Author: Rubeena Zaveri, 2018-09-26

## Summary

The Alphanumeric display communicates with ESP32 via I2C. I2C is a way to communicate between a microcontroller ("master") and connected device(s) ("slave").It uses 2 pins from the microcontroller even if other devices are connected to it. Rather than the data being bluntly transmitted, it is a concatenation of multiple elements. The data starts with a start byte, includes an address that is assigned to the slave that is a recipient of the data, of course includes the data, and then asks for an acknowledgment from the slave. This simplifies things by reducing the number of pins the ESP32 uses while avoiding the problem of data getting lost or transmitted to the wrong devices.Using this connection we can easily feed the alphanumeric board with the correct information even if we were to connect other devices.

## Method:

      1. Configure Master and Slave components -- uses a struct called i2c_config_t with attributes such as mode, sda_io_num, and scl_io_num
      2. Install drivers -- done within the configuring step
      3. Write data from master to slave
          a. Initialize a command link -- this creates the space for the message that includes the data and address and such
          b. Load everything into the command link -- put the data, address, and bytes into the command link
      4. In the task function:
          a. Load the data (in this case, the numbers for the display)
          b. Invoke the function to write data to the slave
          c. Invoke the i2c_slave_read_buffer function to fully transmit the data


## Sketches and Photos

![Photo of the alphanumeric display](https://i.imgur.com/1lvDIoC.jpg)

## Sources:
[ESP32 example project]{https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/i2c.html}
}
