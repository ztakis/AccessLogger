// EEPROM related functions

#include "EEPROM_functions.h"


boolean checkCards(uint8_t a[], uint8_t b[]) {              // Check id two card IDs match
  if (a[0] != 0)                                            // Make sure there is something in the array first
    cardsMatch = true;                                      // Assume they match at first
  for (uint8_t i = 0; i < 4; i++) {
    if (a[i] != b[i])                                       // If one fails, all fail
      cardsMatch = false;
  }
  return cardsMatch;
}

boolean isStored(uint8_t id[]) {                            // Find if an ID is stored in EEPROM
  uint8_t counter = EEPROM.read(0);                         // We store the total count of cards in EEPROM's position 0
  for (uint8_t i = 1; i <= counter; i++) {                  // Loop until counter matches the number of ID cards stored
    readID(i);                                              // For counter value, get storedID using readID function
    if (checkCards(id, storedID)) {                         // Compare id to storedID
      return true;                                          // Return true or false
    }
  }
  return false;
}

uint8_t findSlot(uint8_t id[]) {                            // Find the slot number of a card stored in EEPROM
  uint8_t counter = EEPROM.read(0);                         // We store the total count of cards in EEPROM's position 0
  for (uint8_t i = 1; i <= counter; i++) {                  // Loop until counter matches the number of ID cards stored
    readID(i);                                              // For counter value, get storedID using readID function
    if (checkCards(id, storedID)) {                         // Compare id to storedID
      return i;                                             // Return the slot number of the card
    }
  }
}

void readID(uint8_t number) {                               // Read an ID from EEPROM
  uint8_t start = 1 + (number * 4) + 1;                     // Figure out starting position
  for (uint8_t i = 0; i < 4; i++) {                         // Loop 4 times to get the 4 bytes
    storedID[i] = EEPROM.read(start + i);                   // Assign values read from EEPROM to array
  }
}

void printID(uint8_t number) {                              // Print an ID from EEPROM to serial
  uint8_t start = 1 + (number * 4) + 1;
  for (uint8_t i = 0; i < 4; i++) {
    storedID[i] = EEPROM.read(start + i);
    Serial.print(storedID[i] < 0x10 ? " 0" : " ");          // Prints leading zero for HEX less than 0x10
    Serial.print(storedID[i], HEX);
  }
}

void lcdPrintID(uint8_t number) {                           // Print an ID from EEPROM to LCD
  uint8_t start = 1 + (number * 4) + 1;
  for (uint8_t i = 0; i < 4; i++) {
    storedID[i] = EEPROM.read(start + i);
    lcd.print(storedID[i] < 0x10 ? " 0" : " ");
    lcd.print(storedID[i], HEX);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void writeID(uint8_t id[]) {                                // Add card to EEPROM
  if (!isStored(id)) {                                      // Before we add it, check to see if is allready in there
    uint8_t counter = EEPROM.read(0);                       // We store the total count of cards in EEPROM's position 0
    uint8_t start = (counter * 4) + 6;                      // Figure out where the next available slot starts
    for (uint8_t i = 0; i < 4; i++) {                       // Loop 4 times (card's id is 4 bytes)
      EEPROM.write(start + i, id[i]);                       // Write the array values to EEPROM in the right position
    }
    successWrite();
    EEPROM.write(0, counter + 1);                           // Update the total count
    Serial.println(F("Succesfully added ID record to EEPROM"));
    logfile.println(F("Succesfully added ID record to EEPROM"));
  }
  else {
    failedWrite();
    Serial.println(F("Failed! There is something wrong with ID or bad EEPROM"));
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void deleteID(uint8_t id[]) {                               // Delete card from EEPROM 
  if (!isStored(id)) {                                      // Before we delete it, check if is actually in there
    failedWrite();                                          // If not, warn user
    Serial.println(F("Failed! ID not found in EEPROM"));
  }
  else {
    uint8_t counter = EEPROM.read(0);                       // We store the total count of cards in EEPROM's position 0
    uint8_t slot = findSlot(id);                            // Find the slot number of the card to delete
    uint8_t start = 1 + (slot * 4) + 1;                     // Figure out where the next slot starts
    uint8_t addrCount = (counter - slot) * 4;               // The number of addresses to be shifted
    uint8_t j, k;

    for (j = 0; j < addrCount; j++) {
      EEPROM.write(start + j, EEPROM.read(start + 4 + j));  // Shift the array values 4 places earlier in the EEPROM
    }
    for (k = 0; k < 4; k++) {
      EEPROM.write(start + j + k, 0);                       // Zero last slot
    }

    successDelete();
    EEPROM.write(0, counter - 1);                           // Update the total count
    Serial.println(F("Succesfully removed ID record from EEPROM"));
    logfile.println(F("Succesfully removed ID record from EEPROM"));
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void deleteSlot(uint8_t slot) {                             // Delete card from EEPROM using slot position number
  uint8_t counter = EEPROM.read(0);                         // We store the total count of cards in EEPROM's position 0
  uint8_t start = 1 + (slot * 4) + 1;                       // Figure out where the next slot starts
  uint8_t addrCount = (counter - slot) * 4;                 // The number of addresses to be shifted
  uint8_t j, k;

  for (j = 0; j < addrCount; j++) {
    EEPROM.write(start + j, EEPROM.read(start + 4 + j));    // Shift the array values to 4 places earlier in the EEPROM
  }
  for (k = 0; k < 4; k++) {
    EEPROM.write(start + j + k, 0);                         // Zero last slot
  }

  EEPROM.write(0, counter - 1);                             // Update the total count
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void serialDeleteSlot() {                                   // Delete slot using serial console
  while (true) {
    Serial.println(F("------------------------------"));
    Serial.print(F("Cards in EEPROM:  "));
    uint8_t counter = EEPROM.read(0);
    Serial.println(counter);
    Serial.print(F("Slot"));
    Serial.print("\t ");
    Serial.println(F("UID"));
    for (uint8_t i = 1; i <= counter; i++){                 // Print a list of all cards in EEPROM
      Serial.print(i);
      Serial.print("\t");
      printID(i);
      Serial.println("");
    }
    Serial.println(F("------------------------------"));
    Serial.println(F("Enter slot number to delete"));       // Promt user
    Serial.println(F("Or enter zero (0) to exit"));
    Serial.println(F("------------------------------"));
    while (Serial.available() > 0) {                        // Clear serial buffer
      Serial.read();
    }
    while (Serial.available() == 0);
    uint8_t slot = Serial.parseInt(SKIP_ALL, '\r');         // Parse serial input for slot number
    if (slot == 0 || slot > 255) {                          // If zero, break loop to exit
      break;
    }
    Serial.print(F("Selected: "));    
    Serial.print(slot);
    Serial.print("     ");
    printID(slot);                                          // Print slot
    Serial.println("");
    delay(1000);
    deleteSlot(slot);                                       // Delete slot
    Serial.println(F("Succesfully removed from EEPROM"));
    delay(1000);
  }
  Serial.println(F("Exiting..."));                          // Exit after break when slot is zero
  return;
}

void lcdDeleteSlot() {                                      // Delete slot using LCD buttons
  while(slotStart > 0 || slotStart <= 255) {                // Accept slot# 1-255
    lcd.setCursor(0,0);
    lcd.print(F("Slot         UID"));
    delay(180);
    lcd.print(F("                "));
    switch (lcdKeys.getKey()) {
      case btnUP: {                                         // If button up
        slotStart = slotStart - 1;                          // decrement (-1) slot
        lcd.setCursor(0,1);
        lcd.print(F("    "));
        lcd.setCursor(0,1);
        lcd.print(slotStart);
        lcd.setCursor(4,1);
        lcdPrintID(slotStart);
        break;
      }
      case btnDOWN: {                                       // If button down
        slotStart = slotStart + 1;                          // increment (+1) slot
        lcd.setCursor(0,1);
        lcd.print(F("    "));
        lcd.setCursor(0,1);
        lcd.print(slotStart);
        lcd.setCursor(4,1);
        lcdPrintID(slotStart);
        break;
      }
      case btnSELECT: {                                     // If select button is pressed
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Slot: ");
        lcd.print(slotStart);
        lcd.setCursor(7,0);
        lcd.print(" wil be");
        lcd.setCursor(0,1);
        lcd.print("deleted in 3 sec");
        delay (3000);
        lcd.clear();
        deleteSlot(slotStart);                              // delete selected slot
        break;
        }
    }
  }
}

void clearEEPROM() {                                        // Erase EEPROM completely
  for (uint16_t i = 0; i < EEPROM.length(); i++) {          // Loop till the end of EEPROM
    EEPROM.write(i, 0);                                     // Write all zeros to clear
  }
}

