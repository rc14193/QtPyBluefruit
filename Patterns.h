#ifndef PATTERNS_H
#define PATTERNS_H

#include <FastLED.h>

void commandSetBrightness(std::string rxValue);

void setPixelColor(uint8_t colorSet[]);

void showRainbow();

void setAllBrightness();

void setAllOff();

void flashPattern();

void displayAll();

void setRemainingColor(std::string rxValue);

void commandClearColor(std::string rxValue);

void commandSetPixel(std::string rxValue);


void flashLEDForBLE();
#endif