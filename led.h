#ifndef LED_H
#define LED_H

#include <FastLED.h>
#define NUM_LEDS 1
#define DATA_PIN D8
#define LED_TYPE NEOPIXEL

void initLED();
void setLEDColor(uint8_t r,uint8_t g,uint8_t b);
void clearLED();

#endif