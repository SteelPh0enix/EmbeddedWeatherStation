# Simple, yet overkilled, weather station

## Description

This is my first STM32 and ESP8266 project, made purely for learning purposes. I grabbed some cheap chinese electronics and merged them togeher on prototype board.

Here are some pics. First one is development board, second is displayed data.

![Prototype board](https://i.imgur.com/mzcHs7s.jpg)
![Displayed data](https://i.imgur.com/q38qZAv.jpg)

Additional header on the left of BME280 was used for external RTC module, because at the time i ordered STM32 i did not know that it has internal RTC and proper oscillator on board. Shame on me. At least i have free I2C header for possible expansion in the future.

## Used hardware

* Main microcontroller: **STM32F103C8T6** (Blue Pill)
* Connectivity microcontroller: **ESP8266** (ESP-12F)
* Environment sensor: **BME280**
* OLED Display: **SSD1306** (128x64 resolution)
* **ST-Link v2** for STM32 debugging and programming
* **FT232RL** for UART debugging and ESP programming

## Used software

* STM32:
    * **STM32CubeMX** for project and boilerplate generation
    * **Atollic TrueSTUDIO for STM32** as main IDE
    * **OpenRTOS** as *operating system* for STM32
* ESP8266:
    * **NodeMCU** for ESP firmware
    * **ESPLorer** as IDE, and code uploader

## Repository

Root directory contains Atollic TrueSTUDIO and STM32CubeMX project. NodeMCU Lua code can be found in [`NodeMCU`](./NodeMCU) directory.

## You want to make it yourself?

Okay, no problem. You can find STM32 pinout by opening the project in STM32CubeMX, and NodeMCU is connected to STM32 throught UART1. In case you are not fancy using CubeMX (for whatever masochistic reasons you have), here is full pinout of STM32:

* BME280 and SSD1306 is connected via I2C1 on PB7 (SDA) and PB8 (SCL) pins
* ADC1_IN0 (PA0) (and ADC1_IN1 (PA1) in case of two cells) will be used for battery measurement in the future. You will have to make sure that input voltage won't exceed 3.3V, or you will damage your MCU. I'l planning to use 18650 cell(s) with cheap chinese step-down converter to power this stuff up, and voltage divider to reduce input voltage for ADC.
* USART1_RX (PA10) and USART1_TX (PA9) for STM32<->ESP connectivity and optional debugging
* PC14, PC15, PD0 and PD1 as external oscillators input
* PA13 and PA14 for DSW (Debug Serial Wire)

ESP8266 pinout is basically RX and TX connected to STM32 TX and RX.

Also, make sure to create `credentails.lua` file with `SSID` and `PASSWORD` variables for your WiFi connection, and upload it to ESP. Obviously, i'm not including mine.