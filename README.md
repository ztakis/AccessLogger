# NFC/RFID access control and logging
My final project for CS50x

[![Watch the video](https://i9.ytimg.com/vi_webp/by-YmVJW6ls/mq2.webp?sqp=CLiC1Z8G-oaymwEmCMACELQB8quKqQMa8AEB-AH-CYAC0AWKAgwIABABGD4gZShbMA8=&rs=AOn4CLC7iRjJqomXbnA3zXJ-Fc8w7q1nrg)](https://youtu.be/by-YmVJW6ls) | [![Watch the video](https://i9.ytimg.com/vi_webp/nelEj20rjT4/mq3.webp?sqp=CJCH1Z8G-oaymwEmCMACELQB8quKqQMa8AEB-AH-CYAC0AWKAgwIABABGE8gZShjMA8=&rs=AOn4CLDt7CYyAsUxJf053sL9CiGFFmJYTA)](https://youtu.be/nelEj20rjT4)

[![Watch the video](https://i9.ytimg.com/vi_webp/aY_QvsBsoKk/mq3.webp?sqp=CLyJ1Z8G-oaymwEmCMACELQB8quKqQMa8AEB-AH-CYAC0AWKAgwIABABGBMgVSh_MA8=&rs=AOn4CLBhz_-eMkHvKqV4PrpWS_SdvyDj5A)](https://youtu.be/aY_QvsBsoKk)


<!-- ![screenshot.png](screenshot.png) -->

## Description

Arduino based NFC/RFID access control and logging solution
that includes a Flask web page for real time monitoring

## Table of Contents

- [Hardware](#hardware)
- [Softdware](#software)
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

## Software

The sofware consists of two parts:
1. The core part (Arduino's v2.0.x IDE) 
- I tried to split the project in several files so it could better organized and more readable
- I used a lot of functions and made some classes
- The device can work standalone but for the more advanced stuff, needs usb serial connection
- Good WiFi reception is needed, otherwise use the ntp vesion where WiFi is started on demand

2. The web part (vscode + flask + sqlite3 on a Raspberry 4)
- I tried to make a simple web page much like CS50's birthdays
- Arduino's web client "prints" data to the server using GET
- The web page can be set to auto reload for real time monitoring

## Usage

To use the application, follow these steps:

1. Open the application in your web browser.
2. Create a new note by clicking the "New Note" button.
3. Edit an existing note by clicking on its title.
4. Delete a note by clicking on the trash can icon next to its title.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
