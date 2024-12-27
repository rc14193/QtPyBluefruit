#ifndef BLESERVER_H
#define BLESERVER_H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "Patterns.h"
#include "Globals.h"

class MyCallbacks: public BLECharacteristicCallbacks {
  public:
    MyCallbacks();
    void onWrite(BLECharacteristic *pCharacteristic);
};

class MyServerCallbacks: public BLEServerCallbacks {
  public:
    MyServerCallbacks();
    void onConnect(BLEServer* pServer);
    void onDisconnect(BLEServer* pServer);
};

void replyOK();

void returnVersion();

#endif