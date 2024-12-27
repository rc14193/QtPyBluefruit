#include "Globals.h"

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;
uint8_t MAXCOMPONENTS = 4;
uint8_t frameStep = 0;
void (*processFrame)();

CRGB leftPixels[STRANDLEN];
CRGB rightPixels[STRANDLEN];

int startTime = millis();
bool ledOn = false;
int color = 0x000000;
