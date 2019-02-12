#  LIDAR

Rubeena Zaveri, 2018-20-11

## Summary

Our team programmed the ESP32 UART to read from the LIDAR. The LIDAR was capable of measuring the distance to an object as close as 30 centimeters and as far as 12 meters. The LIDAR takes 5V input, and communicates via Tx and Rx pins (transmit and receive). The microcontroller uses UART (number 1), to store data taken from the lidar in a 256 byte buffer. The buffer is then parsed for header bits (0x59, 0x59), and the relevant distance values are found. These values are averaged and returned as the distance value.

## Sketches and Photos

Photo of the LIDAR
![Photo of the LIDAR](https://i.imgur.com/m9wG7Ej.jpg)

Photo of Measurement Device with LIDAR attached
![Photo of Measurement Device with LIDAR attached](https://i.imgur.com/22EYpCS.jpg)


## Sources:

[ESP-IDF UART API Reference](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/uart.html)


[Data Sheet](https://cdn.sparkfun.com/assets/5/e/4/7/b/benewake-tfmini-datasheet.pdf}
)
