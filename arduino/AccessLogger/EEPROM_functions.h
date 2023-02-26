// EEPROM related functions

#ifndef EEPROM_FUNCTIONS_H
#define EEPROM_FUNCTIONS_H

#include <Arduino.h>
#include <EEPROM.h>
#include "MFRC522_functions.h"

extern boolean successRead;
extern boolean cardsMatch;
extern boolean programMode;
extern uint8_t storedID[];
extern uint8_t masterID[];
extern uint8_t slotStart;
extern const String defaultPassword;
extern String inputPassword;

extern void(* resetFunc) (void);
extern void checkPassword();

boolean checkCards (uint8_t a[], uint8_t b[]);
boolean isStored(uint8_t find[]);

uint8_t findSlot(uint8_t id[]);

void readID(uint8_t number);

void clearEEPROM();
void deleteID(uint8_t a[]);

void deleteSlot(uint8_t slot);
void serialDeleteSlot();

void lcdDeleteSlot();
void lcdPrintID(uint8_t number);
void printID(uint8_t number);
void writeID(uint8_t a[]);


#endif

