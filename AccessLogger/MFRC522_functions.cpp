#include "MFRC522_functions.h"

/////////////////////////  Show Reader Details  ////////////////////////////////
void showReaderDetails() {
  // Get the MFRC522 software version
  uint8_t v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("MFRC522 Software Version: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (unknown),probably a chinese clone?"));
  Serial.println("");
  // When 0x00 or 0xFF is returned, communication probably failed
  if ((v == 0x00) || (v == 0xFF)) {
    Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
    Serial.println(F("SYSTEM HALTED: Check connections."));
    alert();
    while (true); // do not go further
  }
}

/////////////////////  Get PICC's UID and Personal Data  ///////////////////////
uint8_t getCard() {
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (uint8_t i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  // Some variables we need
  MFRC522::StatusCode status;
  uint8_t block;
  uint8_t len = 18;
  if ( ! mfrc522.PICC_IsNewCardPresent()) {      // Look for new cards
    return 0;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {    // Select one of the cards
    return 0;
  }
  // Get PICC's UID
  for (uint8_t i = 0; i < 4; i++) {
    scannedID[i] = mfrc522.uid.uidByte[i];
  }
  // Get first name
  block = 4;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    cardError("Authentication failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return 0;
  }
  status = mfrc522.MIFARE_Read(block, firstName, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    cardError("Reading failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return 0;
  }
  // Get last name
  block = 1;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    cardError("Authentication failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return 0;
  }
  status = mfrc522.MIFARE_Read(block, lastName, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    cardError("Reading failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return 0;
  }
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  return 1;
}

