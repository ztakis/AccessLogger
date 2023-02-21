/*

INFO TBD

*/

////////////////////////////////////////////////////////////////////////////////

#include "Helper_functions.h"

////////////////////////////////////////////////////////////////////////////////

// Buttons
const uint8_t BUTTON_A_PIN = 29;            // Aux button pin
// const uint8_t BUTTON_M_PIN = 25;         // Master Button pin (Depreciated))

// Leds
const uint8_t RED_LED_PIN = 24;
const uint8_t GREEN_LED_PIN = 26;
const uint8_t BLUE_LED_PIN = 28;

const uint8_t red = RED_LED_PIN;
const uint8_t green = GREEN_LED_PIN;
const uint8_t blue = BLUE_LED_PIN;

// MFRC522
const uint8_t RST_PIN = 48;
const uint8_t SLV_PIN = 53;

// SD card
const uint8_t CS_PIN = 10;

// Relay
const uint8_t RELAY_PIN = 27;

// RTC
const char DAYS[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday",
  "Thursday", "Friday", "Saturday"};

// WiFi
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

// NTP
const uint16_t localPort = 2390;            // Local port to listen for UDP packets
const uint8_t NTP_PACKET_SIZE = 48;         // NTP timestamp is in the first 48 bytes of the message
const uint16_t UDP_TIMEOUT = 2000;          // Timeout in miliseconds to wait for an UDP packet to arrive
uint8_t packetBuffer[NTP_PACKET_SIZE];      // Buffer to hold incoming and outgoing packets

char timeServer[] = "ntp.nist.gov";         // NTP server
const int GMT = 2;                          // Change this to adapt it to your time zone

// Default Password
const String defaultPassword = "pass1234";


////////////////////////////////////////////////////////////////////////////////

// RGB Led
RGBLed MainLed (RED_LED_PIN, GREEN_LED_PIN, BLUE_LED_PIN);

// Buttons
Button AuxButton(BUTTON_A_PIN);
// Button MasterButton(BUTTON_M_PIN);    // Master Button (Depreciated))

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

// WiFi UDP
WiFiEspUDP Udp;   // A UDP instance to let us send and receive packets over UDP                  
// WiFiUDP Udp;   // When using the WiFiEspAT.h lib


////////////////////////////////////////////////////////////////////////////////

boolean replaceMaster = false;    // initialize replace master card to false
boolean programMode = false;      // Initialize programming mode to false
boolean successRead = false;      // Initialize successful read from reader as false
boolean cardsMatch = false;       // Initialize cards match to false

uint8_t scannedID[4];     // Stores scanned ID read from reader
uint8_t firstName[18];    // Stores first name (personal data) read from reader
uint8_t lastName[18];     // Stores last name (personal data) read from reader
uint8_t masterID[4];      // Stores master card's ID read from EEPROM
uint8_t storedID[4];      // Stores an ID read from EEPROM
uint8_t slotStart = 0;    // Stores EEPROM slot start // Stores EEPROM card's start address

String inputPassword;     // Stores user input password from serial console


////////////////////////////  FUNCTION  PROTOTYPES  ////////////////////////////

// Arduino IDE takes care of that


///////////////////////////////////   SETUP   //////////////////////////////////

void setup() {

  // Relay init
  pinMode(RELAY_PIN, OUTPUT);       // Check how relay circuit behaves while power-cycling Arduino
  digitalWrite(RELAY_PIN, HIGH);    // Make sure door is locked. If needed, set relay pin LOW instead

  // Led init
  MainLed.init();
  MainLed.on(red);

  // Buttons init
  AuxButton.init();
  // MasterButton.init();           // Master Button (Depreciated))

  // Serial init
  Serial.begin(9600);               // Debug serial
  Serial3.begin(115200);            // WiFi serial

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
    // The following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // MFRC522 init
  mfrc522.PCD_Init();
  // Set Antenna Gain to Max to increase reading distance
  // mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
  Serial.println(F("------------------------------"));
  Serial.println(F("Access Control & Logging"));
  showReaderDetails();  // MFRC522 Card Reader details
  Serial.println(F("------------------------------"));

  // SD card init
  if (! SD.begin(CS_PIN)) {
    error("SDCard failed, or not present");
  }
  // Create a new log file
  char filename[] = "LOG_#_00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {    // Only open a new file if it doesn't exist ////////////////////////////////////
      logfile = SD.open(filename, FILE_WRITE);
      break;
    }
  }
  if (! logfile) {
    error("couldnt create file");
  }
  Serial.print(F("Logging to: "));
  Serial.println(filename);

  checkForMaster(); // Check if a master card if defined
  MainLed.cycle(blue, red, green, 150);

  Serial.println(F("------------------------------"));
  Serial.println(F("Everything is ready"));
  Serial.println(F("Waiting PICCs to be scanned"));
  Serial.println(F("------------------------------"));

}

/////////////////////////////////   MAIN LOOP  /////////////////////////////////

void loop () {

  do {
    successRead = getCard();             // Upon successful read from reader we set successRead to 1, otherwise 0
    if (programMode) {                   // Check if in program mode
      MainLed.cycle(red, green, blue, 250);
    }
    else {
      lcdNormalMode();
      MainLed.on(blue);
      digitalWrite(RELAY_PIN, HIGH);    // Make sure door is locked
    }
    pollButtons();                      // Check if a button is pressed
  }
  while (! successRead);                // This will run at least once, then it will loop until we get a successful read

  printDateTime();
  printCardUID();
  printUser();

  pollProgramMode();
  if (programMode) {
    lcdProgramMode("PUT PICC TO SCAN");
  }

}

/////////////////////////////////    RESTART   /////////////////////////////////

void(* resetFunc) (void) = 0;           // declare reset function at address 0


////////////////////////////////////////////////////////////////////////////////

void checkForMaster() {                 // Check if master card defined, if not let user choose a master card
  if (EEPROM.read(1) != 143) {          // EEPROM address 1 should hold magical number which is '143'
    alert();
    Serial.println(F("No master card Defined"));
    checkPassword();
    Serial.println(F("Scan A PICC to Define as master card"));
    do {
      successRead = getCard();          // Sets successRead to 1 when we get a successful read from reader, otherwise 0
    }
    while (!successRead);               // Program will not go further while you not get a successful read
    MainLed.off(red);
    MainLed.blink(blue, 250);
    for (uint8_t i = 0; i < 4; i++) {
      EEPROM.write(2 + i, scannedID[i]);    // Write scanned PICC's UID to EEPROM, start from address 3
    }
    EEPROM.write(1, 143);                   // By writing 143 to EEPROM address 1, we define a master card.
    Serial.println(F("Master card Defined"));
    MainLed.on(green);
  }
  Serial.println(F("Master card's UID"));   // Print master card's UID from EEPROM
  for (uint8_t i = 0; i < 4; i++) {
    masterID[i] = EEPROM.read(2 + i);
    Serial.print(masterID[i], HEX);
  }
  Serial.println("");
}

void pollProgramMode() {                // Check if in program mode
  if (programMode) {
    if (checkCards(scannedID, masterID)) {    // In program mode, if scanned card is master card then exit program mode
      Serial.println(F("Master card Scanned"));
      Serial.println(F("Exiting Program Mode"));
      logfile.println(F("Exiting Program Mode"));
      Serial.println(F("------------------------------"));
      programMode = false;
      lcdProgramMode("*   EXITING    *");
      delay(1500);
      return;
    }
    else {
      if ( isStored(scannedID) ) {            // If scanned card is known delete it
        Serial.println(F("I know this PICC, removing..."));
        deleteID(scannedID);
        Serial.println("------------------------------");
        Serial.println(F("Scan a PICC to ADD or REMOVE to EEPROM"));
      }
      else {                                  // If scanned card is not known add it
        Serial.println(F("I do not know this PICC, adding..."));
        writeID(scannedID);
        Serial.println(F("------------------------------"));
        Serial.println(F("Scan a PICC to ADD or REMOVE to EEPROM"));
      }
    }
  }
  else {                                    // When not in program mode, if scanned card is master card then enter program mode
    if (checkCards(scannedID, masterID)) {
      lcdProgramMode("*   ENTERING   *");
      delay(1500);
      logfile.print(F("Program Mode"));
      logfile.println();
      Serial.println(F("Hello Master - Entered Program Mode"));
      uint8_t count = EEPROM.read(0);   // Read the first Byte of EEPROM that
      Serial.print(F("I have "));       // stores the number of ID's in EEPROM
      Serial.print(count);
      Serial.print(F(" record(s) on EEPROM"));
      Serial.println("");
      Serial.println(F("Scan a PICC to ADD or REMOVE to EEPROM"));
      Serial.println(F("Scan master card again to Exit Program Mode"));
      Serial.println(F("------------------------------"));
      programMode = true;
    }
    else {                           // If not master, see if the card is in the EEPROM
      if (isStored(scannedID)) {     // If it is
        accessGranted(300);          // Open the door lock for 300 ms
      }
      else {                         // If not, show that the ID was not valid
        accessDenied();
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////


