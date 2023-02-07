#include "EEPROM_functions.h"


boolean checkCards(uint8_t a[], uint8_t b[]) {    // Check id two card IDs match
  if (a[0] != 0)                                  // Make sure there is something in the array first
    cardsMatch = true;                            // Assume they match at first
  for (uint8_t i = 0; i < 4; i++) {
    if (a[i] != b[i])                             // one fails, all fail
      cardsMatch = false;
  }
  return cardsMatch;
}

boolean isStored(uint8_t id[]) {                  // Find if ID is stored in EEPROM
  uint8_t count = EEPROM.read(0);                 // Read the first Byte of EEPROM that
  for (uint8_t i = 1; i <= count; i++) {          // Loop once for each EEPROM entry
    readID(i);                                    // Read an ID from EEPROM, it is stored in storedID[4]
    if (checkCards(id, storedID)) {               // Check to see if the storedID read from EEPROM
      return true;
    }
  }
  return false;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void readID(uint8_t number) {                     // Read an ID from EEPROM
  uint8_t start = (number * 4) + 2;               // Figure out starting position
  for (uint8_t i = 0; i < 4; i++) {               // Loop 4 times to get the 4 Bytes
    storedID[i] = EEPROM.read(start + i);         // Assign values read from EEPROM to array
  }
}

void printID(uint8_t number) {
  uint8_t start = (number * 4) + 2;
  for (uint8_t i = 0; i < 4; i++) {
    storedID[i] = EEPROM.read(start + i);
    Serial.print(storedID[i] < 0x10 ? " 0" : " ");
    Serial.print(storedID[i], HEX);
  }
}

void lcdPrintID(uint8_t number) {
  uint8_t start = (number * 4) + 2;
  for (uint8_t i = 0; i < 4; i++) {
    storedID[i] = EEPROM.read(start + i);
    lcd.print(storedID[i] < 0x10 ? " 0" : " ");
    lcd.print(storedID[i], HEX);
  }
}

void writeID(uint8_t id[]) {   //  Add ID to EEPROM
  if (!isStored(id)) {     // Before we write to the EEPROM, check to see if we have seen this card before!
    uint8_t num = EEPROM.read(0);     // Get the numer of used spaces, position 0 stores the number of ID cards
    uint8_t start = (num * 4) + 6;  // Figure out where the next slot starts
    num++;                // Increment the counter by one
    EEPROM.write(0, num);     // Write the new count to the counter
    for (uint8_t i = 0; i < 4; i++) {   // Loop 4 times
      EEPROM.write(start + i, id[i]);  // Write the array values to EEPROM in the right position
    }
    successWrite();
    Serial.println(F("Succesfully added ID record to EEPROM"));
    logfile.println(F("Succesfully added ID record to EEPROM"));
  }
  else {
    failedWrite();
    Serial.println(F("Failed! There is something wrong with ID or bad EEPROM"));
  }
}

void deleteID(uint8_t id[]) {    //  Remove ID from EEPROM 
  if (!isStored(id)) {     // Before we delete from the EEPROM, check to see if we have this card!
    failedWrite();      // If not
    Serial.println(F("Failed! There is something wrong with ID or bad EEPROM"));
  }
  else {
    uint8_t num = EEPROM.read(0);   // Get the numer of used spaces, position 0 stores the number of ID cards
    uint8_t slot;       // Figure out the slot number of the card
    uint8_t start;      // = ( num * 4 ) + 6; // Figure out where the next slot starts
    uint8_t looping;    // The number of times the loop repeats
    uint8_t j;
    uint8_t count = EEPROM.read(0); // Read the first Byte of EEPROM that stores number of cards
    slot = findIDSLOT(id);   // Figure out the slot number of the card to delete
    start = (slot * 4) + 2;
    looping = ((num - slot) * 4);
    num--;      // Decrement the counter by one
    EEPROM.write(0, num);   // Write the new count to the counter
    for (j = 0; j < looping; j++) {         // Loop the card shift times
      EEPROM.write(start + j, EEPROM.read(start + 4 + j));   // Shift the array values to 4 places earlier in the EEPROM
    }
    for (uint8_t k = 0; k < 4; k++) {         // Shifting loop
      EEPROM.write(start + j + k, 0);
    }
    successDelete();
    Serial.println(F("Succesfully removed ID record from EEPROM"));
    logfile.println(F("Succesfully removed ID record from EEPROM"));
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


uint8_t findIDSLOT(uint8_t find[]) {
  uint8_t count = EEPROM.read(0);                 // Read the first Byte of EEPROM that
  for (uint8_t i = 1; i <= count; i++) {          // Loop once for each EEPROM entry
    readID(i);                                    // Read an ID from EEPROM, it is stored in storedID[4]
    if (checkCards(find, storedID)) {             // Check to see if the storedID read from EEPROM is the same as the find[] ID card passed
      return i;                                   // The slot number of the card
      // break;                                      // Stop looking we found it
    }
  }
}

void deleteSlot(uint8_t slot) {
  uint8_t num = EEPROM.read(0);   // Get the numer of used spaces, position 0 stores the number of ID cards   
  uint8_t start = (slot * 4) + 2;    // Figure out where the next slot starts
  uint8_t looping = ((num - slot) * 4);    // The number of times the loop repeats
  uint8_t j, k;
  num--;      // Decrement the counter by one
  EEPROM.write(0, num);   // Write the new count to the counter
  for (j = 0; j < looping; j++) {         // Loop the card shift times
    EEPROM.write(start + j, EEPROM.read(start + 4 + j));   // Shift the array values to 4 places earlier in the EEPROM
  }
  for (k = 0; k < 4; k++) {         // Shifting loop
    EEPROM.write(start + j + k, 0);
  }
  Serial.println(F("Succesfully removed Slot from EEPROM"));
}

void serialDeleteSlot() {
  Serial.println(F("----------------------------"));
  Serial.print(F("Cards in EEPROM:  "));
  uint8_t count = EEPROM.read(0);
  Serial.println(count);
  Serial.print(F("Slot"));
  Serial.print("\t ");
  Serial.println(F("UID"));
  for (uint8_t i = 1; i <= count; i++){
    Serial.print(i);
    Serial.print("\t");
    printID(i);
    Serial.println();
  }
  Serial.println(F("----------------------------"));
  Serial.println(F("Enter slot number and press ENTER"));
  while (Serial.available() == 0);
  uint8_t slot = Serial.parseInt(SKIP_ALL, '\n');
  Serial.print(slot);
  Serial.print("\t");
  printID(slot);
  Serial.println();
  delay(1000);
  deleteSlot(slot);
}

void lcdDeleteSlot() {
  lcd.setCursor(0,0);
  lcd.print(F("Slot         UID"));
  delay(180);
  lcd.print(F("                "));
  switch (lcdKeys.getKey()) {
    case btnUP: {
      slotStart = slotStart - 1;
      lcd.setCursor(0,1);
      lcd.print(F("    "));
      lcd.setCursor(0,1);
      lcd.print(slotStart);
      lcd.setCursor(4,1);
      lcdPrintID(slotStart);
      break;
    }
    case btnDOWN: {
      slotStart = slotStart + 1;
      lcd.setCursor(0,1);
      lcd.print(F("    "));
      lcd.setCursor(0,1);
      lcd.print(slotStart);
      lcd.setCursor(4,1);
      lcdPrintID(slotStart);
      break;
    }
    case btnSELECT: {
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
      deleteSlot(slotStart);
      break;
      }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void clearEEPROM() {
  for (uint16_t x = 0; x < EEPROM.length(); x = x + 1) {    // Loop end of EEPROM address
    if (EEPROM.read(x) == 0) {    // If EEPROM address is 0
      // Do nothing, already clear
    }
    else {
      EEPROM.write(x, 0);   // Write 0 to clear, it takes 3.3mS
    }
  }
}

