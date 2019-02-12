#  DC Motor

Rubeena Zaveri, 2018-20-11

## Summary

Used PMW to driver the DC motor using the H bridge. The motor runs on 5v and is controled by an H-Bridge Motor Driver (L293D) which interfaces the ESP32 to the motors.


## Sketches and Photos
[Video of car driving](https://drive.google.com/open?id=14FLaKTIXJl3Ki7IpLC_MDVtWWblU_bO)

![Image of motors](https://i.imgur.com/Qi2KRpH.jpg)

## Sources:
[L298 Driver Example](https://github.com/espressif/esp-idf/tree/11b444b8f493165eb4d93f44111669ee46be0327/examples/peripherals/mcpwm/mcpwm_brushed_dc_control)

[About H-Bridges](http://www.modularcircuits.com/blog/articles/h-bridge-secrets/h-bridges-the-basics/)

[ESP Motor control with PWM](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/mcpwm.html)
