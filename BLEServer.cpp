#include "BLEServer.h"
#include <Arduino.h>

MyCallbacks::MyCallbacks() {

}

void MyCallbacks::onWrite(BLECharacteristic *pCharacteristic) {
  std::string rxValue = std::string((pCharacteristic->getValue()).c_str());

  if (rxValue.length() > 0) {
    Serial.println("*********");
    Serial.print("Received Value: ");
    for (int i = 0; i < rxValue.length(); i++)
      Serial.print(rxValue[i], HEX);

    Serial.println("\nLength of ");
    Serial.print(rxValue.length());
    Serial.println("*********");
  }
  
  switch (rxValue[0]){
      case 'V':
        returnVersion();
        break;
      case 'P':
        commandSetPixel(rxValue);
        break;
      case 'C':
        commandClearColor(rxValue);
        break;
      case 'X':
        processFrame = nullptr;
        setAllBrightness();
        break;
      case 'R':
        processFrame = showRainbow;
        break;
      case 'F':
        processFrame = flashPattern;
        break;
      default:
        replyOK();
    }
    };

MyServerCallbacks::MyServerCallbacks() {

}

void MyServerCallbacks::onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

void MyServerCallbacks::onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    };

void replyOK() {
  pTxCharacteristic->setValue("OK");
  pTxCharacteristic->notify();
}

void returnVersion() {
  pTxCharacteristic->setValue("Neopixel v2.0");
  pTxCharacteristic->notify();
}