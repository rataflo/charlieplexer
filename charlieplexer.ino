// 8 step sequencer or rgb light
// For my son 2nd birthday <3

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN            6
#define NUMPIXELS      16

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
bool bModeMusic = true;
bool bModeLight = true;
int stepsFreq[] = {100,120,140,160,180,200,220,4699};
int stepsDur[] = {100,120,140,160,180,200,220,240};
int bSteps[] = {true, false, true, false, true, true, true, true};
byte activeStep = 0;
bool blinkActiveStep = false;
unsigned long currentMillis = 0;
unsigned long lastMillis = 0;
unsigned long lastBlink = 0;
unsigned long lastReadSW = 0;

byte iStep = 0;
bool oldState = HIGH;
int showType = 0;

void setup() {
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(A4, INPUT);
  pixels.setBrightness(50);
  pixels.begin();

  // Check mode at startup.
  if(bModeMusic){
    initMusic();
  }else{
    initLight();
  }
}

void loop() {

  unsigned long currentMillis = millis();

  // check inputs.
  int tempo = analogRead(A3);// tempo
  int duration = analogRead(A2)/2 ;// duration
  int freq = analogRead(A1) * 3;// frequency
  byte r = map(tempo, 0, 1024, 0, 255);
  byte g = map(duration, 0, 512, 0, 255);
  byte b = map(freq, 0, 3072, 0, 255);
  bool bMode = digitalRead (A4);

  // Music mode
  if(!bMode){
    if(currentMillis - lastReadSW > 100){
      lastReadSW = currentMillis;
      if(readSwitches()){
        bSteps[activeStep] = !bSteps[activeStep];
      }
    }
    if(bSteps[activeStep]){
      stepsFreq[activeStep] = freq;
      stepsDur[activeStep] = duration;
    }
  
    tempo = map(tempo, 0, 1024, 0, 400);
    
    for(int j = 0; j < 8; j++){
      byte gStep = map(stepsDur[j], 0, 512, 0, 255);
      byte bStep = map(stepsFreq[j], 0, 3072, 0, 255);
      if(!bSteps[j]){
        pixels.setPixelColor(j*2, 0, 0, 0); 
        pixels.setPixelColor((j*2)+1, 0, 0, 0);
      } else if(j == iStep){
        pixels.setPixelColor(j*2, 255, 0, 0); 
        pixels.setPixelColor((j*2)+1, 255, 0, 0);
      } else if(j == activeStep){
         if(currentMillis - lastBlink > 200){
          pixels.setPixelColor(activeStep*2, pixels.Color(blinkActiveStep ? r : 0, blinkActiveStep ? gStep : 0, blinkActiveStep ? bStep : 0)); 
          pixels.setPixelColor((activeStep*2)+1, pixels.Color(blinkActiveStep ? r : 0, blinkActiveStep ? gStep : 0, blinkActiveStep ? bStep : 0));
          blinkActiveStep = !blinkActiveStep;
          lastBlink = currentMillis;
         }
      } else {
        pixels.setPixelColor(j*2, pixels.Color(r, gStep, bStep)); 
        pixels.setPixelColor((j*2)+1, pixels.Color(r, gStep, bStep));
      }
    }
    pixels.show();  
    if(currentMillis - lastMillis > tempo){
      if(bSteps[iStep]){
        tone(11, stepsFreq[iStep], stepsDur[iStep]);
      } 

      iStep = iStep + 1 > 7 ? 0 : iStep + 1;
      lastMillis = currentMillis;
    }
  } else { //light mode.
    noTone(11);
    bool bStart = showType == 0 ? true : false;
    // Check if state changed from high to low (button press).
    if (digitalRead(12) == LOW) {
      showType = showType + 1 > 9 ? 0 : showType + 1;
      bStart = true;
    }
    if(bStart){
      startShow(showType, r, g ,b);
    }
  }
}

void initMusic(){
  for (int i=0; i<8; i++)
  {
    if(bSteps[i]){
      pixels.setPixelColor(i*2, pixels.Color(0, 255, 0)); 
      pixels.setPixelColor((i*2)+1, pixels.Color(0, 255, 0));
    }else{
      pixels.setPixelColor(i*2, pixels.Color(0, 0, 0)); 
      pixels.setPixelColor((i*2)+1, pixels.Color(0, 0, 0));
    }
  }
}

void initLight(){
}


bool readSwitches()
{
  Serial.println(digitalRead (3));
  bool bPush = false;
  // check switch 0, if pressed, get the current freq into step 0, etc. etc.
  if (digitalRead (2) == LOW)
  {
    activeStep = 0;
    bPush = true;
  }
  else if (digitalRead (3) == LOW)
  {
    activeStep = 1;
    bPush = true;
  }
  else if (digitalRead (4) == LOW)
  {
    activeStep = 2;
    bPush = true;
  }
  else if (digitalRead (5) == LOW)
  {
    activeStep = 3;
    bPush = true;
  }
  else if (digitalRead (7) == LOW)
  {
    activeStep = 4;
    bPush = true;
  }
  else if (digitalRead (8) == LOW)
  {
    activeStep = 5;
    bPush = true;
  }
  else if (digitalRead (9) == LOW)
  {
    activeStep = 6;
    bPush = true;
  }
  else if (digitalRead (10) == LOW)
  { 
    activeStep = 7;
    bPush = true;
  }
  return bPush;
}

// NEOPIXEL COLOR FUNCTIONS
void startShow(int i, byte r, byte g, byte b) {
  switch(i){
    case 0: chooseColor(r, g, b);
            break;
    case 1: colorWipe(pixels.Color(255, 0, 0), 50);  // Red
            break;
    case 2: colorWipe(pixels.Color(0, 255, 0), 50);  // Green
            break;
    case 3: colorWipe(pixels.Color(0, 0, 255), 50);  // Blue
            break;
    case 4: theaterChase(pixels.Color(127, 127, 127), 50); // White
            break;
    case 5: theaterChase(pixels.Color(127,   0,   0), 50); // Red
            break;
    case 6: theaterChase(pixels.Color(  0,   0, 127), 50); // Blue
            break;
    case 7: rainbow(20);
            break;
    case 8: rainbowCycle(20);
            break;
    case 9: theaterChaseRainbow(50);
            break;
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<pixels.numPixels(); i++) {
    pixels.setPixelColor(i, c);
    pixels.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i+j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < pixels.numPixels(); i=i+3) {
        pixels.setPixelColor(i+q, c);    //turn every third pixel on
      }
      pixels.show();

      delay(wait);

      for (int i=0; i < pixels.numPixels(); i=i+3) {
        pixels.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < pixels.numPixels(); i=i+3) {
        pixels.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      pixels.show();

      delay(wait);

      for (int i=0; i < pixels.numPixels(); i=i+3) {
        pixels.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void chooseColor(byte r, byte g, byte b){
  for(int j = 0; j < NUMPIXELS; j++){
      pixels.setPixelColor(j, pixels.Color(r, g, b)); 
    }
    pixels.show();  
}
