/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
   Has a characteristic of: 6E400002-B5A3-F393-E0A9-E50E24DCCA9E - used for receiving data with "WRITE" 
   Has a characteristic of: 6E400003-B5A3-F393-E0A9-E50E24DCCA9E - used to send data with  "NOTIFY"

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   In this example rxValue is the data received (only accessible inside that function).
   And txValue is the data to be sent, in this example just a byte incremented every second. 
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Adafruit_NeoPixel.h>

// How many internal neopixels do we have? some boards have more than one!
#define NUMPIXELS        1
#define STRANDLEN        7
#define LEFTSTRAND       26
#define RIGHTSTRAND      15

Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel leftPixels(STRANDLEN, LEFTSTRAND, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rightPixels(STRANDLEN, RIGHTSTRAND, NEO_GRB + NEO_KHZ800);

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;
uint8_t MAXCOMPONENTS = 4;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

void returnVersion() {
  pTxCharacteristic->setValue("Neopixel v2.0");
  pTxCharacteristic->notify();
}

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if(rxValue[0] == 'V') {
        returnVersion();
      }
      else if (rxValue[0] == 'P'){   // Set Pixel
          commandSetPixel(rxValue);
      }
      else if (rxValue[0] == 'C'){   // Clear with color
          commandClearColor(rxValue);
      }
      else if (rxValue[0] == 'B'){   // Set brightness
          commandSetBrightness(rxValue);
      }
      else {
        replyOK();
      }

      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++)
          Serial.print(rxValue[i], HEX);

        Serial.println("Length of ");
        Serial.print(rxValue.length());
        Serial.println("*********");
      }
    }
};

int startTime = millis();
bool ledOn = false;
int color = 0x000000;
uint8_t mainColor[3] = {0, 0, 0}; 
uint8_t remainingColor[3] = {0, 0, 0};

void setup() {
  Serial.begin(115200);

#if defined(NEOPIXEL_POWER)
  // If this board has a power control pin, we must set it to output and high
  // in order to enable the NeoPixels. We put this in an #if defined so it can
  // be reused for other boards without compilation errors
  pinMode(NEOPIXEL_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_POWER, HIGH);
#endif

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  leftPixels.begin();
  rightPixels.begin();
  pixels.setBrightness(100); // not so bright
  leftPixels.setBrightness(100);
  rightPixels.setBrightness(100);

  // Create the BLE Device
  BLEDevice::init("MYDEVICE");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
										CHARACTERISTIC_UUID_TX,
										BLECharacteristic::PROPERTY_NOTIFY
									);
                      
  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
											 CHARACTERISTIC_UUID_RX,
											BLECharacteristic::PROPERTY_WRITE
										);

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void replyOK() {
  pTxCharacteristic->setValue("OK");
  pTxCharacteristic->notify();
}

void flashLEDForBLE() {
    if(!ledOn) {
      color = 0x0000FF;
    }
    else {
      color = 0x000000;
    }
    ledOn = !ledOn;
    startTime = millis();
    leftPixels.fill(color);
    rightPixels.fill(color);
    pixels.fill(color);
    leftPixels.show();
    rightPixels.show();
    pixels.show();
}

void loop() {

    if (deviceConnected) {
        //pTxCharacteristic->setValue(&txValue, 1);
        //pTxCharacteristic->notify();
        txValue++;
		delay(10); // bluetooth stack will go into congestion, if too many packets are sent
	}
  else if (!deviceConnected && (millis()-startTime) > 500){
    flashLEDForBLE();
  }

    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
		// do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}

void commandSetPixel(std::string rxValue) {
  Serial.println(F("Command: SetPixel"));

  // Read color
  uint8_t colorSet[MAXCOMPONENTS+1];
  for (int j = 1; j <= MAXCOMPONENTS;) {
    colorSet[j-1] = rxValue[j];
    Serial.println("Setting colorClear ");
    Serial.print(colorSet[j-1]);
    Serial.print(" to the following:");
    Serial.println(rxValue[j], HEX);
    j++;
  }
  if(colorSet[0] < STRANDLEN) {
    leftPixels.setPixelColor(colorSet[0], colorSet[2], colorSet[3], colorSet[1]);
  } 
  else if(colorSet[0] >= STRANDLEN && colorSet[0] < (2*STRANDLEN)) {
    rightPixels.setPixelColor(colorSet[0]-STRANDLEN, colorSet[2], colorSet[3], colorSet[1]);
  }
  leftPixels.show();
  rightPixels.show();
  pixels.show();
}

void commandSetBrightness(std::string rxValue) {
  pixels.setBrightness((uint8_t)rxValue[1]); // not so bright
  leftPixels.setBrightness((uint8_t)rxValue[1]);
  rightPixels.setBrightness((uint8_t)rxValue[1]);
  leftPixels.show();
  rightPixels.show();
  pixels.show();

  replyOK();
}

void setRemainingColor(std::string rxValue) {
  String value = rxValue.c_str();
  value.toLowerCase();
  if(value == "ared") {
    ///remainingColor = [0xFF, 0x00, 0x00];
  }
}

void commandClearColor(std::string rxValue) {
  Serial.println(F("Command: ClearColor"));

  // Read color
  uint8_t colorClear[MAXCOMPONENTS];
  for (int j = 1; j < MAXCOMPONENTS;) {
    colorClear[j-1] = rxValue[j];
    Serial.println("Setting colorClear ");
    Serial.print(colorClear[j-1]);
    Serial.print(" to the following:");
    Serial.println(rxValue[j], HEX);
    j++;
  }

  uint32_t outputColor = ((uint32_t)colorClear[0] << 16) | ((uint32_t)colorClear[1] << 8) | ((uint32_t)colorClear[2]);
  Serial.println("Setting pixels now with output color");
  Serial.print(outputColor, HEX);

  leftPixels.fill(outputColor);
  rightPixels.fill(outputColor);
  pixels.fill(outputColor);
  leftPixels.show();
  rightPixels.show();
  pixels.show();
  
  // Done
  replyOK();
}