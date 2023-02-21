# NFC/RFID access control and logging
My final project for CS50x

[![Watch the video](screenshot.png)](https://youtu.be/T-D1KVIuvjA)

<!-- ![screenshot.png](screenshot.png) -->

## Description

Arduino based NFC/RFID access control and logging solution
that includes a Flask web page for real time monitoring

## Table of Contents

- [Hardware](#hardware)
- [Usage](#usage)
- [License](#license)

## Hardware
Since this is for CS50, I won't go into much detail about the hardware.
I used the following:

- Arduino Mega 2560 with built in ESP8266 WiFi (RobotDyn)
- MFRC-522 13.56MHz nfc/rfid reader
- Data Logger Shield with SD card and real time clock (Adafruit)
- LCD Keypad Shield
- Simple custom board with RGB led and button(s)

Basic connections:
- Ardunino and ESP8266 WiFi, communicate using hardware serial
- MFRC-522 uses SPI
- SD card uses SPI and RTC uses I2C
- LCD Keypad uses parallel and analog

## Usage

To use the application, follow these steps:

1. Open the application in your web browser.
2. Create a new note by clicking the "New Note" button.
3. Edit an existing note by clicking on its title.
4. Delete a note by clicking on the trash can icon next to its title.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
