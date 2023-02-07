///////////////////////////////  INCLUDES  ////////////////////////////////////

// User libs
#include "Button.h"
#include "Key.h"
#include "RGB_Led.h"
#include "Helper_functions.h"
#include "EEPROM_functions.h"
#include "MFRC522_functions.h"

// Libs
// #include <EEPROM.h>
// #include <LiquidCrystal.h>
// #include <MFRC522.h>
// #include <RTClib.h>
// #include <SD.h>
// #include <SPI.h>
// #include <Wire.h>


///////////////////////////  DEFINES & CONSTANTS  /////////////////////////////

// Buttons
const uint8_t BUTTON_A_PIN = 29;           // Aux button pin
const uint8_t BUTTON_M_PIN = 25;           // Master Button pin

// Leds
const uint8_t RED_LED_PIN = 24;
const uint8_t GREEN_LED_PIN = 26;
const uint8_t BLUE_LED_PIN = 28;

const uint8_t red = RED_LED_PIN;
const uint8_t green = GREEN_LED_PIN;
const uint8_t blue = BLUE_LED_PIN;

// // MFRC522
const uint8_t RST_PIN = 48;
const uint8_t SLV_PIN = 53;

// SD card
const uint8_t CS_PIN = 10;

// Relay
const uint8_t RELAY_PIN = 27;

// RTC
const char DAYS[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday",
  "Thursday", "Friday", "Saturday"};

// Default Password
const String defaultPassword = "pass1234";

///////////////////////////  GLOBAL  VARIABLES  //////////////////////////////

boolean replaceMaster = false;   // initialize replace master card to false
boolean programMode = false;     // Initialize programming mode to false
boolean successRead = false;     // Initialize successful read from reader as false
boolean cardsMatch = false;      // Initialize cards match to false

uint8_t scannedID[4];    // Stores scanned ID read from RFID Module
uint8_t firstName[18];   // Stores first name (personal data) read from RFID Module
uint8_t lastName[18];    // Stores last name (personal data) read from RFID Module
uint8_t masterID[4];     // Stores master card's ID read from EEPROM
uint8_t storedID[4];     // Stores an ID read from EEPROM
uint8_t slotStart;       // Stores EEPROM slot start

String inputPassword;    // Stores user input password from serial console


////////////////////////////////  OBJECTS  /////////////////////////////////////

// RGB Led
RGBLed MainLed (RED_LED_PIN, GREEN_LED_PIN, BLUE_LED_PIN);

// Buttons
Button AuxButton(BUTTON_A_PIN);
Button MasterButton(BUTTON_M_PIN);

// LCD panel pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// LCD panel keys
Key lcdKeys(0);

// RFID reader
MFRC522 mfrc522(SLV_PIN, RST_PIN);

// SDcard Log file
File logfile;

// RTC
RTC_PCF8523 rtc;


///////////////////////////  FUNCTION PROTOTYPES  /////////////////////////////

// Arduino IDE takes care of that


////////////////////////********  RESTART  *********///////////////////////////

void(* resetFunc) (void) = 0;  // declare reset function at address 0


/////////////////////////********  SETUP  *********////////////////////////////

void setup() {

  // Relay init
  pinMode(RELAY_PIN, OUTPUT);   // Check how relay circuit behaves while power-cycling Arduino
  digitalWrite(RELAY_PIN, HIGH);    // Make sure door is locked

  // Led init
  MainLed.init();

  // Buttons init
  AuxButton.init();
  MasterButton.init();

  // Serial init
  Serial.begin(9600);

  // I2C init
  Wire.begin();

  // SPI init
  SPI.begin();

  // LCD init
  lcd.begin(16,2);   
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print(F("Initializing"));
  lcd.setCursor(2,1);
  lcd.print(F("RFID logger.."));

  // Connect to RTC
  if (! rtc.begin()) {
    error("Couldn't find RTC");
  }
  if (! rtc.initialized()) {
    Serial.print(F("ERROR: "));
    Serial.println("RTC is NOT running!");
    error("RTC is NOT running!");
    // Thefollowing line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2022, 1, 31, 20, 13, 0));
  }

  // MFRC522 init
  mfrc522.PCD_Init();
  // Set Antenna Gain to Max to increase reading distance
  // mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
  Serial.println(F("------------------------"));
  Serial.println(F("Access Control & Logging"));
  showReaderDetails();  // MFRC522 Card Reader details
  Serial.println(F("------------------------"));

  // SD card init
  if (! SD.begin(CS_PIN)) {
    error("SDCard failed, or not present");
  }
  // Create a new log file
  char filename[] = "LOGGER00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      // Only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE);
      break;
    }
  }
  if (! logfile) {
    error("couldnt create file");
  }
  Serial.print(F("Logging to: "));
  Serial.println(filename);
  Serial.println(F("------------------------"));

  checkForMaster(); // Check if a master card if defined
  MainLed.cycle(blue, red, green, 150);

  Serial.println(F("------------------------"));
  Serial.println(F("Everything is ready"));
  Serial.println(F("Waiting PICCs to be scanned"));
  Serial.println(F("------------------------"));

}

///////////////////////*******  MAIN LOOP  ********/////////////////////////////

void loop () {

  do {
    successRead = getCard();  // Upon successful read from reader we set successRead to 1 otherwise 0
    if (programMode) {    // Check if in program mode
      MainLed.cycle(red, green, blue, 250);
    }
    else {
      lcdNormalMode();
      MainLed.on(blue);
      digitalWrite(RELAY_PIN, HIGH);    // Make sure door is locked
    }
    pollButtons();    // Check if a button is pressed
  }
  while (!successRead);  // This will run at least once, then it will loop until we get a successful read

  printDateTime();
  printCardUID();
  printUser();
  
  pollProgramMode();
  if (programMode) {
    lcdProgramMode("PUT PICC TO SCAN");
  }

}

////************************************************************************////

void checkForMaster() {     // Check if master card defined, if not let user choose a master card
  if (EEPROM.read(1) != 143) {                    // EEPROM address 1 should hold magical number which is '143'
    alert();
    Serial.println(F("No Master Card Defined"));
    checkPassword();
    Serial.println(F("Scan A PICC to Define as Master Card"));
    do {
      successRead = getCard();                    // sets successRead to 1 when we get read from reader otherwise 0
    }
    while (!successRead);                         // Program will not go further while you not get a successful read
    MainLed.off(red);
    MainLed.blink(blue, 250);
    for (uint8_t i = 0; i < 4; i++) {             // Loop 4 times
      EEPROM.write(2 + i, scannedID[i]);          // Write scanned PICC's UID to EEPROM, start from address 3
    }
    EEPROM.write(1, 143);                         // Write to EEPROM we defined Master Card.
    Serial.println(F("Master Card Defined"));
    MainLed.on(green);
  }
  Serial.println(F("Master Card's UID"));
  for (uint8_t i = 0; i < 4; i++) {               // Read Master Card's UID from EEPROM
    masterID[i] = EEPROM.read(2 + i);             // Write it to masterID
    Serial.print(masterID[i], HEX);
  }
  Serial.println("");
}

void pollProgramMode() {    // Check if in program mode
  if (programMode) {
    // if ( isMaster(scannedID) ) { // When in program mode, if scanned card is master card then exit program mode
    if (checkCards(scannedID, masterID)) {
      Serial.println(F("Master Card Scanned"));
      Serial.println(F("Exiting Program Mode"));
      logfile.println(F("Exiting Program Mode"));
      Serial.println(F("-----------------------------"));
      programMode = false;
      lcdProgramMode("*   EXITING    *");
      delay(1500);
      return;
    }
    else {
      if ( isStored(scannedID) ) { // If scanned card is known delete it
        Serial.println(F("I know this PICC, removing..."));
        deleteID(scannedID);
        Serial.println("-----------------------------");
        Serial.println(F("Scan a PICC to ADD or REMOVE to EEPROM"));
      }
      else {                    // If scanned card is not known add it
        Serial.println(F("I do not know this PICC, adding..."));
        writeID(scannedID);
        Serial.println(F("-----------------------------"));
        Serial.println(F("Scan a PICC to ADD or REMOVE to EEPROM"));
      }
    }
  }
  else {
    // if (isMaster(scannedID)) {    // When not in program mode, if scanned card is master card then enter program mode
    if (checkCards(scannedID, masterID)) { 
      programMode = true;
      lcdProgramMode("*   ENTERING   *");
      delay(1500);
      logfile.print(F("Program Mode"));
      logfile.println();
      Serial.println(F("Hello Master - Entered Program Mode"));
      uint8_t count = EEPROM.read(0);   // Read the first Byte of EEPROM that
      Serial.print(F("I have "));     // stores the number of ID's in EEPROM
      Serial.print(count);
      Serial.print(F(" record(s) on EEPROM"));
      Serial.println("");
      Serial.println(F("Scan a PICC to ADD or REMOVE to EEPROM"));
      Serial.println(F("Scan Master Card again to Exit Program Mode"));
      Serial.println(F("-----------------------------"));
    }
    else {
      if (isStored(scannedID)) { // If not, see if the card is in the EEPROM
        accessGranted(300);         // Open the door lock for 300 ms
      }
      else {      // If not, show that the ID was not valid
        accessDenied();
      }
    }
  }
}

////************************************************************************////


