#include "RGB_Led.h"

RGBLed::RGBLed(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin) {
  this->red_pin = red_pin;
  this->green_pin = green_pin;
  this->blue_pin = blue_pin;
}

void RGBLed::init() {
  pinMode(red_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT);
  alloff();
}

void RGBLed::alloff() {
  digitalWrite(red_pin, LOW);
  digitalWrite(green_pin, LOW);
  digitalWrite(blue_pin, LOW);
}

void RGBLed::on(uint8_t pin) {
  if (pin == red_pin) {
    digitalWrite(red_pin, HIGH);
    digitalWrite(green_pin, LOW);
    digitalWrite(blue_pin, LOW);
  }
  else if (pin == green_pin) {
    digitalWrite(red_pin, LOW);
    digitalWrite(green_pin, HIGH);
    digitalWrite(blue_pin, LOW);
  }
  else if (pin == blue_pin) {
    digitalWrite(red_pin, LOW);
    digitalWrite(green_pin, LOW);
    digitalWrite(blue_pin, HIGH);
  }
}

void RGBLed::off(uint8_t pin) {
  digitalWrite(pin, LOW);
}

void RGBLed::blink(uint8_t pin, unsigned long milliseconds) {
  digitalWrite(pin, HIGH);
  delay(milliseconds);
  digitalWrite(pin, LOW);
  delay(milliseconds);
}

void RGBLed::alternate(uint8_t pin1, uint8_t pin2, unsigned long milliseconds) {
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, LOW);
  delay(milliseconds);
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, HIGH);
  delay(milliseconds);
}

void RGBLed::cycle(uint8_t pin1, uint8_t pin2, uint8_t pin3, unsigned long milliseconds) {
  on(pin1);
  off(pin2);
  off(pin3);
  delay(milliseconds);
  off(pin1);
  on(pin2);
  off(pin3);
  delay(milliseconds);
  off(pin1);
  off(pin2);
  on(pin3);
  delay(milliseconds);
}

