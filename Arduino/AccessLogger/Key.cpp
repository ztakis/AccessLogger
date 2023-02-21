// Define Key class functions

#include "Key.h"

Key::Key(uint8_t pin) {
  this->pin = pin;
}

int Key::getKey() {
  // read the value from the sensor
  if (analogRead(pin) > 1000) return btnNONE;
  if (analogRead(pin) < 50)   return btnRIGHT;
  if (analogRead(pin) < 195)  return btnUP;
  if (analogRead(pin) < 380)  return btnDOWN;
  if (analogRead(pin) < 555)  return btnLEFT;
  if (analogRead(pin) < 790)  return btnSELECT;
  return btnNONE;  // when all others fail, return this...
}

