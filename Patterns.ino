void commandSetBrightness(String rxValue) {
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
  displayAll();
}

void alternatePattern() {
  CRGB first;
  CRGB second;
  CRGB color;
  if(frameStep % 2 == 0) {
    first = mainColor;
    second = remainingColor;
  }
  else {
    first = remainingColor;
    second = mainColor;
  }
  for(uint8_t i = 0; i < STRANDLEN; i++) {
    if(i % 2 == 0) color = first;
    else color = second;
    leftPixels[i] = color;
    rightPixels[i] = color;
  }
  displayAll();
}

void pathPattern() {
  // Add a bright pixel that moves
  leftPixels[frameStep % STRANDLEN] = CHSV((255.0*frameStep/MAXFRAMES) * 2, 255, 255);
  rightPixels[frameStep % STRANDLEN] = CHSV((255.0*(frameStep+STRANDLEN)/MAXFRAMES) * 2, 255, 255);
  // Blur the entire strip
  blur1d(leftPixels,STRANDLEN, 172);
  //fadeToBlackBy(leftPixels, STRANDLEN, 16);
  blur1d(rightPixels, STRANDLEN, 172);
  //fadeToBlackBy(rightPixels, STRANDLEN, 16);
  FastLED.show();
}

void showRainbow() {
  uint8_t startHue = (255.0 * ((float)frameStep/(float)MAXFRAMES));
  fill_rainbow(leftPixels, STRANDLEN, startHue, 255/STRANDLEN);
  fill_rainbow(rightPixels, STRANDLEN, startHue, 255/STRANDLEN);
  displayAll();
}

void setAllBrightness() {
  FastLED.setBrightness(26);
}

void setAllOff() {
  FastLED.setBrightness(0);
}

void flashPattern() {
  uint8_t scale = FastLED.getBrightness();
  if(scale != 26) {
    setAllBrightness();
  }
  else {
    setAllOff();
  }
  displayAll();
}

void snakePattern() {
  for(uint8_t i = 0; i < STRANDLEN; i++) {
    leftPixels[i] = remainingColor;
    rightPixels[i] = remainingColor;
  }
  if(frameStep < STRANDLEN) {
    leftPixels[frameStep] = mainColor;
  }
  else {
    rightPixels[frameStep % STRANDLEN] = mainColor;
  }
  displayAll();
}

void fadePattern() {
  uint8_t scale = FastLED.getBrightness();
  if(scale == 0) {
    nscale8(leftPixels, STRANDLEN, 26);
    nscale8(rightPixels, STRANDLEN, 26);
  }
  else {
    fadeToBlackBy(leftPixels, STRANDLEN, 26);
    fadeToBlackBy(rightPixels, STRANDLEN, 26);
  }
  displayAll();
}

void displayAll() {
  FastLED.show();
}

void setRemainingColor(String rxValue) {
  String value = rxValue.c_str();
  value.toLowerCase();
  if(value == "ared") {
    ///remainingColor = [0xFF, 0x00, 0x00];
  }
}

void commandClearColor(String rxValue) {
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

  fill_solid(leftPixels, STRANDLEN, outputColor);
  fill_solid(rightPixels, STRANDLEN, outputColor);
  setAllBrightness();
  displayAll();
  processFrame = nullptr;
  
  // Done
  replyOK();
}

void commandSetPixel(String rxValue) {
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
