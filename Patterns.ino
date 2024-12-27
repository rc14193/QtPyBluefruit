void commandSetBrightness(std::string rxValue) {
  setAllBrightness();
  displayAll();
  replyOK();
}

void setPixelColor(uint8_t colorSet[]) {
  if(colorSet[0] < STRANDLEN) {
    leftPixels[colorSet[0]] = CRGB(colorSet[2], colorSet[3], colorSet[1]);
  } 
  else if(colorSet[0] >= STRANDLEN && colorSet[0] < (2*STRANDLEN)) {
    rightPixels[colorSet[0]] = CRGB(colorSet[2], colorSet[3], colorSet[1]);
  }
}

void showRainbow() {

}

void setAllBrightness() {
  FastLED.setBrightness(26);
}

void setAllOff() {
  FastLED.setBrightness(0);
}

void flashPattern() {
}

void displayAll() {
  FastLED.show();
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

  uint32_t outputColor = ((uint32_t)colorClear[1] << 16) | ((uint32_t)colorClear[0] << 8) | ((uint32_t)colorClear[2]);
  Serial.println("Setting pixels now with output color");
  Serial.print(outputColor, HEX);

  fill_solid(leftPixels, STRANDLEN, outputColor);
  fill_solid(rightPixels, STRANDLEN, outputColor);
  displayAll();
  processFrame = nullptr;
  
  // Done
  replyOK();
}

void commandSetPixel(std::string rxValue) {
  Serial.println(F("Command: SetPixel"));

  // Read color
  uint8_t colorSet[MAXCOMPONENTS+1];
  for (int j = 1; j <= MAXCOMPONENTS;) {
    colorSet[j-1] = rxValue[j];
    Serial.println("Setting colorPixel ");
    Serial.print(colorSet[j-1]);
    Serial.print(" to the following:");
    Serial.println(rxValue[j], HEX);
    j++;
  }

  setAllBrightness();
  setPixelColor(colorSet);
  displayAll();
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
    fill_solid(leftPixels, STRANDLEN, color);
    fill_solid(rightPixels, STRANDLEN, color);
    setAllBrightness();
    displayAll();
}
