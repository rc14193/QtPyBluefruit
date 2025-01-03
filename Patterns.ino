void commandSetBrightness(String rxValue) {
  setAllBrightness();
  displayAll();
  replyOK();
}

void setPixelColor(uint8_t* colorSet) {
  Serial.println("Setting Pixel number: ");
  Serial.print(colorSet[0]);
  if(colorSet[0] < STRANDLEN) {
    leftPixels[colorSet[0]] = CRGB(colorSet[2], colorSet[3], colorSet[4]);
  } 
  else if(colorSet[0] >= STRANDLEN && (uint8_t)colorSet[1] < (2*STRANDLEN)) {
    rightPixels[colorSet[0]] = CRGB(colorSet[2], colorSet[3], colorSet[4]);
  }
  displayAll();
}

void alternatePattern() {
  if(frameStep % 12 != 0) return; // animate every quarter second since 24 fps
  CRGB first;
  CRGB second;
  CRGB color;
  if(animationStep % 2 == 0) {
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
  animationStep = (animationStep+1) % MAXFRAMES;
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
  if(frameStep % 6 != 0) return; // animate every quarter second since 24 fps
  uint8_t scale = FastLED.getBrightness();
  if(scale != 26) {
    setAllBrightness();
  }
  else {
    setAllOff();
  }
  displayAll();
}

uint8_t descendFrameOne[] = {3};
uint8_t descendFrameTwo[] = {2, 4};
uint8_t descendFrameThree[] = {1, 5, 6}; 
uint8_t descendFrameFour[] = {0};

uint8_t* descendAnimation[] = {descendFrameOne, descendFrameTwo, descendFrameThree, descendFrameFour};
uint8_t descendRowSizes[] = {1, 2, 3, 1};
uint8_t DESCENDFRAMES = 4;

void descendPattern() {
  if(frameStep % 6 != 0) return; // animate every quarter second since 24 fps
  for(uint8_t i = 0; i < STRANDLEN; i++) {
    leftPixels[i] = CRGB::Black;
    rightPixels[i] = CRGB::Black;  
  }

  uint8_t* frame = descendAnimation[animationStep];
  for(uint8_t i = 0; i <  descendRowSizes[animationStep]; i++) {
    leftPixels[frame[i]] = mainColor;
    rightPixels[frame[i]] = mainColor; 
  }

  animationStep = (animationStep+1) % DESCENDFRAMES;
  displayAll();
}

void snakePattern() {
  if(frameStep % 3 != 0) return;
  for(uint8_t i = 0; i < STRANDLEN; i++) {
    leftPixels[i] = remainingColor;
    rightPixels[i] = remainingColor;
  }
  if(animationStep < STRANDLEN) {
    leftPixels[animationStep] = mainColor;
  }
  else {
    rightPixels[animationStep % STRANDLEN] = mainColor;
  }
  animationStep = (animationStep+1) % (STRANDLEN*2);
  displayAll();
}

void twirlPattern() {
  if(frameStep % 3 != 0) return;

  for(uint8_t i = 0; i < STRANDLEN; i++) {
    leftPixels[i] = remainingColor;
    rightPixels[i] = remainingColor;
  }
  if(animationStep == 5) animationStep += 1;

  leftPixels[animationStep] = mainColor;
  rightPixels[animationStep] = mainColor;

  leftPixels[5] = mainColor;
  rightPixels[5] = mainColor;
  animationStep = (animationStep+1) % STRANDLEN;
  displayAll();
}

void fadePattern() {
  return; // disabled for now
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

  uint8_t pixelComponents[MAXCOMPONENTS] = {0,0,0,0,0}; // app sends as row, col, r, g, b

  for(uint8_t i = 1; i < rxValue.length(); i++) {
    pixelComponents[i-1] = rxValue[i]; // strip off P that inidcates command
  }

  setAllBrightness();
  setPixelColor(pixelComponents);
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
