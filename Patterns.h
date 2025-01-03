#ifndef PATTERNS_H
#define PATTERNS_H

#include <FastLED.h>

void commandSetBrightness(String rxValue);

void setPixelColor(uint8_t colorSet[]);

void showRainbow();

void setAllBrightness();

void setAllOff();

void flashPattern();

void fadePattern();

void pathPattern();

void alternatePattern();

void snakePattern();

void descendPattern();

void twirlPattern();

void displayAll();

void setRemainingColor(String rxValue);

void commandClearColor(String rxValue);

void commandSetPixel(String rxValue);


void flashLEDForBLE();
#endif