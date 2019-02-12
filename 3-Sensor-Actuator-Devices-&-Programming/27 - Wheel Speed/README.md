#  Wheel Speed

Rubeena Zaveri, 2018-20-11

## Summary

Optical detector uses pwm modulation to power IR LED, and uses adc #2 to read raw adc reading of phototransistor with multisampling. Raw data below 1500 are counted as seeing white color, and reading above 1500 means that sensor is seeing black color object or is not seeing anything. Time taken from one rising edge to another is measured to get the current speed. Distance is circumference (radius depends on how far away from wheel center the optical detector is placed) and hence we get speed if we divide distance by time.

## Sketches and Photos
![wheel sensor mounted on car](https://i.imgur.com/0opQmsO.jpg)

## Sources:
[Pulse Counter](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/pcnt.html)
[Timer](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/timer.html)
