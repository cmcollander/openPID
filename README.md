# openPID
An open-source PID motor controller and interface for education

![](https://i.imgur.com/ktodUVm.jpeg)

This is a continuation of a previous project. The RP2040 was a good choice as the PIO can be used to keep track of the encoder state. The modularity of the Qwiic I2C components allows easy modification and expansion of the system. After the base system is built, I plan on creating a Matlab wrapper that will allow logging, control, and system identification.

I'm currently not happy with the cable management of the motor controller, but I was trying to avoid any custom PCBs, which the previous version had. Maybe a PCB will be required to connect between the motor, motor controller, external DC source, and the control/voltage pins of the carrier board...

## Components
* [TB6612FNG Motor Controller](https://www.sparkfun.com/products/14451)
* [RP2040 MicroMod](https://www.sparkfun.com/products/17720)
* [MicroMod Qwiic Dual Carrier Board](https://www.sparkfun.com/products/17724)
* [Qwiic Joystick](https://www.sparkfun.com/products/15168)
* [Qwiic 16x2 SerLCD](https://www.sparkfun.com/products/16396)
* [Quadrature Encoder](https://www.amazon.com/Taiss-Incremental-Encoder-Voltage-Warranty%EF%BC%89600P/dp/B07MX1SYXB/)
* [Toy DC Motor](https://www.amazon.com/Topoox-15000-16500RPM-Electric-Science-Experiments/dp/B073Q2Y3RC/)

## TODO
* Write motor control class
* Debug non-working position
* Enable PID Behavior
* Find good PID values for first-use
* Enable Matlab control through USB-C
* Enable velocity control (currently only position)
