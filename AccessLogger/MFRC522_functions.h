#ifndef MFRC522_FUNCTIONS_H
#define MFRC522_FUNCTIONS_H

#include <Arduino.h>
#include <MFRC522.h>
#include "Helper_functions.h"

extern MFRC522 mfrc522;

extern uint8_t firstName[];
extern uint8_t lastName[];

void showReaderDetails();
uint8_t getCard();

#endif

