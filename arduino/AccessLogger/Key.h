// Define Key class

#ifndef KEY_H
#define KEY_H

#include <Arduino.h>

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

class Key {

  private:
    uint8_t pin;
  public:
    Key(uint8_t pin);
    int getKey();

};

#endif

