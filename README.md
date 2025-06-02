# thermo

This program is meant to run on a Raspberry Pi Zero with multiple DS2484 one-wire bus masters hooked up via I2C.
This is achieved with a GPIO QWIIC hat with a dtoverlay that configures GPIOs pins as 14 I2C busses.
[Adafruit DS2484 I2C to 1-Wire Bus Adapter Breakouts](https://www.adafruit.com/product/5976) are hooked up to 12 out of 14 I2C busses.
