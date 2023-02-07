#include "Button.h"

Button::Button(uint8_t pin) {
  this->pin = pin;
}
void Button::init() {
  pinMode(pin, INPUT_PULLUP);   // Enable pin's pull up resistor
}

bool Button::isPressed() {
  if (digitalRead(pin) == LOW) {
    return true;
  }
  else if (digitalRead(pin) == HIGH) {
    return false;
  }
}

