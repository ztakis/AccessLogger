#include "Helper_functions.h"


void accessDenied() {
  Serial.println(F("Access denied"));
  logfile.print(F("Access denied"));
  logfile.println();
  logfile.flush();
  MainLed.on(red);    // Turn on red LED
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Unknown"));
  lcd.setCursor(0, 1);
  lcd.print(F("Access denied"));
  delay(1000);
  lcd.clear();
}

void accessGranted( uint16_t setDelay) {
  Serial.println(F("Access granted"));
  logfile.print(F("Access granted"));
  logfile.println();
  logfile.flush();
  MainLed.on(green);    // Turn on green LED
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Welcome"));
  lcd.setCursor(0, 1);
  lcd.print(F("Access granted"));
  digitalWrite(RELAY_PIN, LOW);     // Unlock door!
  delay(setDelay);                  // Hold door lock open for given seconds
  digitalWrite(RELAY_PIN, HIGH);    // Relock door
  delay(1000);                      // Hold green LED on for a second
  lcd.clear();
}

void alert() {    // Prompts user to connect USB for more information
  MainLed.on(red);
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print(F("ALERT - Please"));
  lcd.setCursor(2,1);
  lcd.print(F("connect USB"));
}

void cardError(const char *str) {   // PICC card error
  alert();
  Serial.print(F("ERROR: "));
  Serial.println(str);
  resetFunc();
}

void checkPassword() {
  do {
    Serial.println(F("------------------------"));
    Serial.println(F("Enter default password:"));
    while (Serial.available() == 0);
    inputPassword = Serial.readStringUntil('\n');
    inputPassword.trim();
    if (inputPassword != defaultPassword) {
      Serial.println("Incorrect password. Please try again.");
    }
  } while (inputPassword != defaultPassword);
  Serial.println(F("Password accepted"));
}

void error(const char *str) {   // Halt error
  alert();
  Serial.print(F("ERROR: "));
  Serial.println(str);
  while(1);
}

void failedWrite() {    // EEPROM write failed
  lcdProgramMode("!!FAILED WRITE!!");
  MainLed.alloff();
  delay(250);
  MainLed.blink(red, 250);
  MainLed.blink(red, 250);  
  MainLed.on(red);
  delay(250);
}

void lcdNormalMode() {    // LCD default message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Put your id key"));
  lcd.setCursor(0, 1);
  lcd.print(F("over the sensor"));
}

void lcdProgramMode(const char *txt) {    // LCD program mode
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(txt);
  lcd.setCursor(0,1);
  lcd.print(F("* PROGRAM MODE *"));
}

void pollButtons() {    // Poll buttons and initiate corresponding actions
  if (programMode) {    // Check if in program mode
    if (MasterButton.isPressed()) {    // Check if Master button is pressed
      MainLed.on(red);
      lcdProgramMode("*EEPROM  DELETE*");
      Serial.println(F("Master Button Pressed"));
      Serial.println(F("Master Card will be Erased! in 5 seconds"));
      Serial.println(F("EEPROM will be Erased! in 10 seconds"));
      bool buttonState = pollMasterButton(5000); // Give user enough time to cancel operation
      if (buttonState == true && MasterButton.isPressed()) {
        bool buttonState = pollMasterButton(5000);
        if (buttonState == true && MasterButton.isPressed()) {     // > 10s sec
          while (true) {
          lcdProgramMode("*ERASING EEPROM*");
          Serial.println(F("Erasing EEPROM..."));
          clearEEPROM();
          Serial.println(F("EEPROM erased successfully"));
          delay(2000);
          lcdProgramMode("*EEPROM  ERASED*");
          delay(2000);
          lcdProgramMode("*  RESTARTING  *");
          Serial.println(F("Restarting..."));
          delay(2000);
          resetFunc();
          }
        }
        else {                                              // > 5 sec & < 10 sec
          while (true) {
          lcdProgramMode("*ERASING MASTER*");
          Serial.println(F("Erasing MASTER..."));
          EEPROM.write(1, 0);                               // Reset Magic Number
          Serial.println(F("Master Card Erased from device"));
          delay(2000);
          lcdProgramMode("*MASTER  ERASED*");
          delay(2000);
          lcdProgramMode("*  RESTARTING  *");
          Serial.println(F("Restarting..."));
          delay(2000);        
          resetFunc();
          }
        }
      }
      Serial.println(F("Erase Cancelled"));
      lcdProgramMode("*   CANCELED   *");
      delay(1000);
      lcdProgramMode("PUT PICC TO SCAN");
    }
    if (AuxButton.isPressed()) {   // Check if Serial button is pressed
      MainLed.on(red);
      lcdProgramMode("* SLOT  DELETE *");
      Serial.println(F("Aux Button Pressed"));
      Serial.println(F("Keep pressing for 5 seconds"));
      bool buttonState = pollAuxButton(5000);
      if (buttonState == true && AuxButton.isPressed()) {
        Serial.println(F("Use LCD up/down buttons to select UID"));
        Serial.println(F("Use LCD select button to delete UID"));        
        lcd.setCursor(0,1);
        lcd.print(F("                "));
        while (true) {
          lcdDeleteSlot();
        }
      }
      Serial.println(F("Slot Delete Cancelled"));
      Serial.println();
      lcdProgramMode("*   CANCELED   *");
      delay(1000);
      lcdProgramMode("PUT PICC TO SCAN");
    }    
  }
  else {
    if (MasterButton.isPressed()) {    // Check if Master button is pressed
      alert();
      Serial.println(F("Master Button Pressed"));
      Serial.println(F("Master Card will be Erased! in 5 seconds"));
      Serial.println(F("EEPROM will be Erased! in 10 seconds"));
      bool buttonState = pollMasterButton(5000);    // Give user enough time to cancel
      if (buttonState == true && MasterButton.isPressed()) {
        bool buttonState = pollMasterButton(5000);
        if (buttonState == true && MasterButton.isPressed()) {     // > 10s sec
          while (true) {
            checkPassword();
            Serial.println(F("Erasing EEPROM in 5 sec..."));            
            delay(5000);
            Serial.println(F("Erasing EEPROM..."));
            clearEEPROM();
            delay(2000);
            Serial.println(F("EEPROM erased successfully"));
            Serial.println(F("Restarting..."));
            delay(2000);
            resetFunc();
          }
        }
        else {                                            // > 5 sec & < 10 sec
          while (true) {
            checkPassword();
            Serial.println(F("Erasing MASTER in 5 sec..."));            
            delay(5000);
            Serial.println(F("Erasing MASTER..."));          
            EEPROM.write(1, 0);                           // Reset Magic Number
            delay(2000);
            Serial.println(F("Master Card erased successfully"));
            Serial.println(F("Restarting..."));
            delay(2000);        
            resetFunc();
          }
        }
      }
      Serial.println(F("Erase Cancelled"));
      Serial.println();
    }
    if (AuxButton.isPressed()) {    // Check if Aux button is pressed 
      alert();
      Serial.println(F("Aux button Pressed"));
      Serial.println(F("Program mode dialog in 5 seconds"));
      Serial.println(F("Slot delete dialog in 10 seconds"));
      bool buttonState = pollAuxButton(5000);
      if (buttonState == true && AuxButton.isPressed()) {
        bool buttonState = pollAuxButton(5000);
        if (buttonState == true && AuxButton.isPressed()) {        // Pressed > 10s sec
          checkPassword();
          while (true) {
            serialDeleteSlot();
          }
        }
        else {                                          // Pressed > 5 sec & < 10 sec
          checkPassword();
          while (true) {
            programMode = true;
            lcdProgramMode("*   ENTERING   *");
            delay(1500);
            lcdProgramMode("PUT PICC TO SCAN");
            Serial.println(F("-----------------------------"));
            Serial.println(F("Scan a PICC to ADD or REMOVE to EEPROM"));
            return;
          }
        }
      }
      Serial.println(F("Slot Delete Cancelled"));
      Serial.println();
    }
  }
}

bool pollAuxButton(uint32_t interval) {    // Poll Aux Button
  uint32_t now = (uint32_t)millis();
  while ((uint32_t)millis() - now < interval)  {
    // check on every half a second
    if (((uint32_t)millis() % 500) == 0) {
      if (! AuxButton.isPressed())
        return false;
    }
  }
  return true;
}

bool pollMasterButton(uint32_t interval) {    // Poll Master Button
  uint32_t now = (uint32_t)millis();
  while ((uint32_t)millis() - now < interval)  {
    // check on every half a second
    if (((uint32_t)millis() % 500) == 0) {
      if (! MasterButton.isPressed())
        return false;
    }
  }
  return true;
}

void printCardUID() {
  Serial.println();
  Serial.print(F("Scanned PICC's UID: "));
  for (uint8_t i = 0; i < 4; i++) {
    Serial.print(scannedID[i] < 0x10 ? "0" : "");
    Serial.print(scannedID[i], HEX);
    logfile.print(scannedID[i] < 0x10 ? "0" : "");
    logfile.print(scannedID[i], HEX);
  }
  logfile.print(",");
  Serial.println();
}

void printDateTime() {
  DateTime now = rtc.now();
  logfile.print(now.unixtime()); // seconds since 1/1/1970
  logfile.print(",");
  logfile.print('"');
  logfile.print(now.year(), DEC);
  logfile.print("/");
  logfile.print(now.month(), DEC);
  logfile.print("/");
  logfile.print(now.day(), DEC);
  logfile.print(" ");
  logfile.print(now.hour(), DEC);
  logfile.print(":");
  logfile.print(now.minute(), DEC);
  logfile.print(":");
  logfile.print(now.second(), DEC);
  logfile.print('"');
  logfile.print(",");
  Serial.println();
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(DAYS[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
}

void printUser() {
  lcd.clear();
  lcd.setCursor(0, 0);
  Serial.print(F("Name: "));
  for (uint8_t i = 0; i < 16; i++) {
    if (firstName[i] != 32) {
      lcd.write(firstName[i]);
      Serial.write(firstName[i]);
      logfile.write(firstName[i]);
    }
  }
  lcd.setCursor(0, 1);
  Serial.print(" ");
  logfile.print(",");
  for (uint8_t i = 0; i < 16; i++) {
    lcd.write(lastName[i]);
    Serial.write(lastName[i]);
    logfile.write(lastName[i]);
  }
  Serial.println();
  logfile.print(",");
  delay(750);
  // Serial.println();
}

void successDelete() {    // UID uccessfully removed from EEPROM
  lcdProgramMode("* PICC REMOVED *");
  MainLed.alloff();
  delay(250);
  MainLed.blink(red, 250);
  MainLed.blink(red, 250);
  MainLed.on(red);  
  delay(250);
}

void successWrite() {     // UID successfull added to EEPROM
  lcdProgramMode("*  PICC ADDED  *");
  MainLed.alloff();
  delay(250);
  MainLed.blink(green, 250);
  MainLed.blink(green, 250); 
  MainLed.on(green);
  delay(250);
}

