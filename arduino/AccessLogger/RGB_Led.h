// Define RGB_Led class

#ifndef RGB_LED_H
#define RGB_LED_H

#include <Arduino.h>

class RGBLed {

  private:
    uint8_t red_pin;
    uint8_t green_pin;
    uint8_t blue_pin;

  public:
    RGBLed(uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin);
    void init();
    void alloff();
    void on(uint8_t pin);
    void off(uint8_t pin);
    void blink(uint8_t pin, unsigned long milliseconds);
    void alternate(uint8_t pin1, uint8_t pin2, unsigned long milliseconds);
    void cycle(uint8_t pin1, uint8_t pin2, uint8_t pin3, unsigned long milliseconds);

};

#endif

