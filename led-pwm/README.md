#  Simple PWM control test

There is a variety of ways to light up a LED, but with ESP32 there is a possibility to control it's intensity with the PWM method. You can do it as a pure software solution with simple digital gpios but the result is limited. The most effective way is to use the hardware solution since ESP32 has one driver embedded just for it. In this simple code, i practice some simple ways to that and learn about esp32 programming with the espressif framework.

## How to test projects

I usually use the espressif addon  for Visual Studio Code. All you have to do is build the c files in the main folder. Chose one file, then do [build], [flash] and [monitor]. Alternatively you can do in the espressif cmd (PORT could be COM6, for example):

> idf.py -p build flash monitor PORT
