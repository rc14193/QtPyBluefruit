#include "BLEServer.h"
#include <Arduino.h>
#include <map>

std::map<String, void(*)()> patternMap;

String getBatteryVoltage() {
  uint raw = analogRead(BATTERYPIN);
  float voltage = float(raw) * (3.7 / 2047); // 3.7 nominal voltage, datasheet says 12 bit but 11 bit is giving me the correct value as physical measurement
  float percentage = (voltage / 4.2) * 100;
  return String(raw) + " " + String(voltage) + "V " + String(percentage) + "%";
}

void replyMessage(String msg) {
  pTxCharacteristic->setValue(msg);
  pTxCharacteristic->notify(); 
}

void clearAnimation() {
  processFrame = nullptr;
  setAllBrightness();
  return;
}

MyCallbacks::MyCallbacks() {
  patternMap["twirl"] = twirlPattern;
  patternMap["rainbow"] = showRainbow;
  patternMap["flash"] = flashPattern;
  patternMap["alternate"] = alternatePattern;
  patternMap["snake"] = snakePattern;
  patternMap["drops"] = descendPattern;
  patternMap["reset"] = clearAnimation;
}

void MyCallbacks::onWrite(BLECharacteristic *pCharacteristic) {
  String rxValue = pCharacteristic->getValue();

  if (rxValue.length() > 0) {
    Serial.println("*********");
    Serial.print("Received Value: ");
    for (int i = 0; i < rxValue.length(); i++){
      Serial.print(rxValue[i], HEX);
      Serial.print(" ");
    }

    Serial.println("\nLength of ");
    Serial.print(rxValue.length());
    Serial.println("*********");
  }

  if (rxValue.indexOf("battery") != -1){
    replyMessage(String(getBatteryVoltage()));
    return;
  }

  // Check if any pattern is in the string
  for (auto const &pair : patternMap) {
    if (rxValue.indexOf(pair.first) != -1) {
        animationStep = 0;
        mainColor = leftPixels[0];
        remainingColor = leftPixels[1];
        processFrame = pair.second;
        replyOK();
        return;
    }
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