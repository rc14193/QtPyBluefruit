#include "Globals.h"

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;
uint8_t MAXCOMPONENTS = 5;
uint8_t FRAMETIME = 1000/24; // 24 fps
uint8_t MAXFRAMES = 24; // 24 fps
uint8_t frameStep = 0;
uint8_t animationStep = 0;
int BATTERYPIN = A2;
void (*processFrame)();

CRGB leftPixels[STRANDLEN];
CRGB rightPixels[STRANDLEN];
CRGB totalPixels[2*STRANDLEN];

CRGB mainColor;
CRGB remainingColor;

int startTime = millis();
bool ledOn = false;
int color = 0x000000;
