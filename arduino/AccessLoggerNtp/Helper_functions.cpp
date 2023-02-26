// Various helper functions (alphabetical)

#include "Helper_functions.h"


void accessDenied() {                               // Inform user that access was denied
  Serial.println(F("Access denied"));
  logfile.print(F("Access denied"));
  logfile.println();
  logfile.flush();
  MainLed.on(red);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Unknown"));
  lcd.setCursor(0, 1);
  lcd.print(F("Access denied"));
  delay(1000);
  lcd.clear();
}

void accessGranted( uint16_t setDelay) {            // Grant access to user / unlock door
  Serial.println(F("Access granted"));
  logfile.print(F("Access granted"));
  logfile.println();
  logfile.flush();
  MainLed.on(green);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Welcome"));
  lcd.setCursor(0, 1);
  lcd.print(F("Access granted"));
  digitalWrite(RELAY_PIN, LOW);                     // Unlock door!
  delay(setDelay);                                  // Hold door lock open for given seconds
  digitalWrite(RELAY_PIN, HIGH);                    // Relock door
  delay(1000);                                      // Hold green LED on for a second
  lcd.clear();
}

void alert() {                                      // Prompt user to connect USB for more information
  MainLed.on(red);
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print(F("ALERT - Please"));
  lcd.setCursor(2,1);
  lcd.print(F("connect USB"));
}

void cardError(const char *str) {                   // PICC card error
  alert();
  Serial.print(F("ERROR: "));
  Serial.println(str);
  resetFunc();
}

void checkPassword() {                              // Prompts user for password untill correct
  do {
    Serial.println(F("------------------------------"));
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

void error(const char *str) {                       // General error
  alert();
  Serial.print(F("ERROR: "));
  Serial.println(str);
  while(1);
}

void failedWrite() {                                // EEPROM write failed error
  lcdProgramMode("!!FAILED WRITE!!");
  MainLed.alloff();
  delay(250);
  MainLed.blink(red, 250);
  MainLed.blink(red, 250);  
  MainLed.on(red);
  delay(250);
}

void lcdNormalMode() {                              // LCD default message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Put your id key"));
  lcd.setCursor(0, 1);
  lcd.print(F("over the sensor"));
}

void lcdProgramMode(const char *txt) {              // LCD program mode
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(txt);
  lcd.setCursor(0,1);
  lcd.print(F("* PROGRAM MODE *"));
}

void pollButtons() {                                // Poll buttons and initiate corresponding actions
  if (programMode) {    // Check if in program mode
    if (AuxButton.isPressed()) {   // Check if Aux button is pressed
      MainLed.on(red);
      lcdProgramMode("* CARD  DELETE *");
      Serial.println(F("Aux Button Pressed"));
      Serial.println(F("Keep pressing for 5 seconds"));
      bool buttonState = pollAuxButton(5000);
      if (buttonState == true && AuxButton.isPressed()) {
        Serial.println(F("Use LCD up/down buttons to select UID"));
        Serial.println(F("Use LCD select button to delete UID"));        
        lcd.setCursor(0,1);
        lcd.print(F("                "));
        lcdDeleteSlot();
      }
      Serial.println(F("Canceled"));
      Serial.println();
      lcdProgramMode("*   CANCELED   *");
      delay(1000);
      lcdProgramMode("PUT PICC TO SCAN");
    }    
  }
  else {    // If not in program mode
    if (AuxButton.isPressed()) {    // Check if Aux button is pressed 
      alert();
      Serial.println("");
      Serial.println(F("------------------------------"));
      Serial.println(F("Aux button Pressed"));
      Serial.println(F("Admin Menu in 5 seconds"));
      bool buttonState = pollAuxButton(5000);
      if (buttonState == true && AuxButton.isPressed()) {   // If pressed > 5 sec
          checkPassword();
          serialMenu();
          Serial.println(F("------------------------------"));
          Serial.println(F("Exiting Admin Menu..."));
          Serial.println(F("Waiting PICCs to be scanned"));
          Serial.println(F("------------------------------"));
          Serial.println("");
          return;
      }
      Serial.println(F("Canceled"));
      Serial.println();
    }
  }
}

bool pollAuxButton(uint32_t interval) {             // Poll Aux Button
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

bool pollMasterButton(uint32_t interval) {          // Poll Master Button
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

void printCardUID() {                               // Print card's UID to serial & log file
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

void printDateTime() {                              // Print date & time to serial & log file & web
  DateTime now = rtc.now();

  logfile.print(now.unixtime());
  logfile.print(",");
  logfile.print(now.year(), DEC);
  logfile.print("/");
  logfile.print(now.month(), DEC);
  logfile.print("/");
  logfile.print(now.day(), DEC);
  logfile.print(" ");
  logfile.print(now.hour() < 10 ? "0" : "");
  logfile.print(now.hour(), DEC);
  logfile.print(":");
  logfile.print(now.minute() < 10 ? "0" : "");
  logfile.print(now.minute(), DEC);
  logfile.print(":");
  logfile.print(now.second() < 10 ? "0" : "");
  logfile.print(now.second(), DEC);
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
  Serial.print(now.hour() < 10 ? "0" : "");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute() < 10 ? "0" : "");
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second() < 10 ? "0" : "");
  Serial.print(now.second(), DEC);
}

void printUser() {                                  // Print card's personal data to lcd & serial & log file
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
    if ((lastName[i] >= 'a' && lastName[i] <= 'z') || (lastName[i] >= 'A' && lastName[i] <= 'Z') || lastName[i] == '*') {
      logfile.write(lastName[i]);
    }
    else {;
      logfile.write("");
    }
  }
  Serial.println();
  logfile.print(",");
  delay(750);
}

void serialMenuBanner() {                           // Admin menu banner
  Serial.println("");
  Serial.println("==============================");
  Serial.println("          Admin Menu          ");
  Serial.println("==============================");
  Serial.println("Please select an option:");
  Serial.println("");  
  Serial.println("1 - Erase cards from EEPROM");
  Serial.println("2 - Erase master card");
  Serial.println("3 - Erase EEPROM completely");
  Serial.println("4 - Personalize cards");
  Serial.println("5 - Sync RTC to NTP");
  Serial.println("6 - Exit menu");
  Serial.println("");
}

void serialMenu() {                                 // This is the admin menu shown in serial console 
  serialMenuBanner();
  while (true) {
    switch (Serial.read()) {
      case '1':   // Erase cards from EEPROM
        serialDeleteSlot();
        break;
      case '2':   // Erase master card
        Serial.println(F("Erasing MASTER in 5 sec..."));            
        delay(5000);
        Serial.println(F("Erasing MASTER..."));          
        EEPROM.write(1, 0);   // Reset Magic Number
        delay(2000);
        Serial.println(F("Master Card erased successfully"));
        Serial.println(F("Restarting..."));
        Serial.println("");
        delay(2000);        
        resetFunc();
        break;
      case '3':   // Erase all EEPROM
        Serial.println(F("Erasing EEPROM in 5 sec..."));            
        delay(5000);
        Serial.println(F("Erasing EEPROM..."));
        clearEEPROM();
        delay(1000);
        Serial.println(F("EEPROM erased successfully"));
        Serial.println(F("Restarting..."));
        Serial.println("");
        delay(1000);
        resetFunc();
        break;
      case '4':   // Write personal data to card
        showReaderDetails();
        Serial.println("");
        Serial.println(F("Write personal data on a MIFARE PICC"));        
        Serial.println("Press reset when finished");
        while (true) {
          writePersonalData();
        }
        break;
      case '5':   // Sync RTC to NTP
        Serial.println("");
        Serial.println(F("Sync RTC to NTP"));
        wifiInit();
        printWifiStatus();
        Udp.begin(localPort);      
        while (true) {
          printWifiStatus();
          rtcAdjustNTP();
          delay(10000);
        }
        break;
      case '6': return;   // Exit
      default: continue;  // Includes the case 'no input'
    }
    serialMenuBanner();
  }
}

void successDelete() {                              // Inform user that card's UID successfully removed from EEPROM
  lcdProgramMode("* PICC REMOVED *");
  MainLed.alloff();
  delay(250);
  MainLed.blink(red, 250);
  MainLed.blink(red, 250);
  MainLed.on(red);  
  delay(250);
}

void successWrite() {                               // Inform user that card's UID was added to EEPROM successfully
  lcdProgramMode("*  PICC ADDED  *");
  MainLed.alloff();
  delay(250);
  MainLed.blink(green, 250);
  MainLed.blink(green, 250); 
  MainLed.on(green);
  delay(250);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

