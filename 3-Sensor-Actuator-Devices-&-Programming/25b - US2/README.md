#  Ultrasonic sensor 2

Author: Rubeena Zaveri, 2018-09-26

## Summary

Our team programmed the ESP32 rx/tx channels to read from the ultrasonic sensor. Ultrasonic sensor 2 uses two pins, a trigger and an echo, in order to gather data. A pulse is sent via the trigger and the time between triggering and receiving is measured. This time correlates to a distance and can be converted to this value using the equation: Test distance = (high level time × velocity of sound (340M/S) / 2.


## Sketches and Photos

Diagram of US
![Diagram of US](https://i.imgur.com/pFVx18I.jpg)

US wiring
![US wiring](https://i.imgur.com/c7XCe0o.jpg)

[Video of Ultrasonic sensor being presented](https://drive.google.com/open?id=1jw5bJunGceZ2aDUcaCEUR_Onyp0W8k9h)

## Sources:

[US Data Sheet](https://cdn.sparkfun.com/assets/b/3/0/b/a/DGCH-RED_datasheet.pdf)
