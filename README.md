# NFC/RFID access control and logging
My final project for CS50x

![screenshot](media/screenshot.png)

## Youtube video

[![Watch the video](media/thumb.jpg)](https://youtu.be/j5Nwe5qK4YY)

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
- Good WiFi reception is needed, otherwise use the ntp vesion where WiFi is started on demand only for rtc to ntp sync

2. The web part (vscode + flask + sqlite3 on a Raspberry 4)
- I tried to make a simple web page much like CS50's birthdays
- Arduino's web client "prints" data to the server using GET
- The web page can be set to auto reload for real time monitoring
- There is no pagination :( (I promise I will do it after CS50w)

## Usage

#### Basic standalone usage

1. Swipe a card over the reader. The device reads the card's id and searches the EEPROM for a match.
If there is a possitive match, the RGB LED turns green, a record is created in the web server's database
and (if implemented) a relay can unlock a door, barricade or similar blocking mechanism. If there is not
a match, the RGB LED turns red, the relay remains off and a record in red shows in the web page.

2. To add or remove a card's id in EEPROM, we need to have the "Master" card which is created during the initial setup.
After we swipe the master card over the reader, the device enters "Program mode". Then we swipe the card it's status
want to change. If the card's id is found in the EEPROM, then is deleted, otherwise added. When we finish adding and removing
cards, we swipe the master card again to return to normal mode.
If a card is lost(anavailable) and we want to remove it from EEPROM, we enter program mode using the master card and then
press the aux button for 5 sec. Using the lcd and up/down buttons, we select the id we want to be removed and press the select
button. The id selected is deleted after 3 sec. After we finish, we press the reset button to restart the device.
To use this method, we need to know the card's id in advance.

#### Initial setup

The device cannot be used until we complete the next step.
For the initial setup, we need to connect the device's USB port to a computer and either use Arduino's IDE serial console
or putty or minicom etc. After we select the correct com port, if it's the first use of the device, we are asked for the 
default password. The password is set in the firmware and we need to re-flash the device if we want to change it.
After giving the correct password, we are asked to set up a master card. We swipe a card over the reader and that is registered
as a master card from now on. Now we can use this mASTER card to enter program mode and add cards as described above.
The device is now ready for use.

#### Admin menu









