#pragma once
#include <FastLED.h>
#include "BLEServer.h"
// How many internal neopixels do we have? some boards have more than one!
#define NUMPIXELS        1
#define STRANDLEN        7
#define LEFTSTRAND       26
#define RIGHTSTRAND      25
extern int BATTERYPIN;

//Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
//Adafruit_NeoPixel leftPixels(STRANDLEN, LEFTSTRAND, NEO_RGB + NEO_KHZ800);
//Adafruit_NeoPixel rightPixels(STRANDLEN, RIGHTSTRAND, NEO_RGB + NEO_KHZ800);

extern CRGB leftPixels[STRANDLEN];
extern CRGB rightPixels[STRANDLEN];
extern CRGB totalPixels[2*STRANDLEN];

extern BLEServer *pServer;
extern BLECharacteristic * pTxCharacteristic;
extern bool deviceConnected;
extern bool oldDeviceConnected;
extern uint8_t txValue;
extern uint8_t MAXCOMPONENTS;
extern uint8_t MAXFRAMES;
extern uint8_t FRAMETIME;
extern uint8_t frameStep;
extern uint8_t animationStep;
extern void (*processFrame)();

extern int startTime;
extern bool ledOn;
extern int color;
extern CRGB mainColor; 
extern CRGB remainingColor;