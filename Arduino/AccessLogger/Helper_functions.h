// Various helper functions

#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <SD.h>

#include "Button.h"
#include "EEPROM_functions.h"
#include "Key.h"
#include "RGB_Led.h"
#include "WiFi_functions.h"

extern Button AuxButton;
extern Button MasterButton;
extern File logfile;
extern Key lcdKeys;
extern LiquidCrystal lcd;
extern RGBLed MainLed;
extern RTC_PCF8523 rtc;
extern WiFiEspClient client;

extern const uint8_t red;
extern const uint8_t green;
extern const uint8_t blue;
extern const uint8_t RELAY_PIN;

extern uint8_t firstName[];
extern uint8_t lastName[];
extern uint8_t scannedID[];
extern const char DAYS[7][12];

void accessDenied();
void accessGranted (uint16_t setDelay);
void alert();
void cardError(const char *str);
void checkPassword();
void error(const char *str);
void failedWrite();
void lcdNormalMode();
void lcdProgramMode(const char *txt);
void pollButtons();
bool pollAuxButton(uint32_t interval);
bool pollMasterButton(uint32_t interval);
void printCardUID();
void printDateTime();
void printUser();
void serialMenuBanner();
void serialMenu();
void successDelete();
void successWrite();


#endif

