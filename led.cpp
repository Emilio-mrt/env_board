#include "led.h"

CRGB leds[NUM_LEDS];

void initLED() {
    FastLED.addLeds<LED_TYPE, DATA_PIN>(leds, NUM_LEDS);
    FastLED.clear();
    setLEDColor(0, 0, 255);
}
void setLEDColor(uint8_t r,uint8_t g,uint8_t b) {
    clearLED();
    leds[0] = CRGB(r,g,b);
    FastLED.show();
}

void clearLED() {
    FastLED.clear();
    FastLED.show();
}
