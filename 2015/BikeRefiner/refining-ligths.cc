#include <Adafruit_NeoPixel.h>

// ----- Microcontroller Spec -----

const int theBrightness = 255 / 4; // 1/8th of max

const int pixelsCount = 26;

const int controlPin = 10;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(pixelsCount, controlPin, NEO_GRB + NEO_KHZ800);

// ----- Color Management -----

const int maxSteps = 10; // 10 steps to next color

const int shiftDelayMillis = 20; // 0.2 seconds between steps

int currentStep = 0, currentPixel = 0;

int currentColorRed = 0xff;
int currentColorGreen = 0xff;
int currentColorBlue = 0xff;

int nextColorRed = 0xff;
int nextColorGreen = 0xff;
int nextColorBlue = 0xff;

void resetColors() {
  currentStep = 0;
  currentPixel = 0;
  currentColorRed = nextColorRed;
  currentColorGreen = nextColorGreen;
  currentColorBlue = nextColorBlue;
  nextColorRed = random() & 0xff;
  nextColorGreen = random() & 0xff;
  nextColorBlue = random() & 0xff;
  for (int i = 0; i < pixelsCount; ++i) {
    pixels.setPixelColor(i, currentColorRed, currentColorGreen, currentColorBlue);
    pixels.show();
  }
}

// ----- Main Body -----

void setup() {
  // Init Microcontroller
  pixels.begin();
  pixels.setBrightness(theBrightness);
  for (int i = 0; i < pixelsCount; ++i) {
    pixels.setPixelColor(i, currentColorRed, currentColorGreen, currentColorBlue);
  }
  pixels.show();
  // Init Colors
  resetColors();
}

int adjustDelta(int delta, int curStep) {
  return delta - delta / (curStep + 2);
}

void loop() {
  pixels.setPixelColor(currentPixel,
    currentColorRed + adjustDelta(nextColorRed - currentColorRed, currentStep),
    currentColorGreen + adjustDelta(nextColorGreen - currentColorGreen, currentStep),
    currentColorBlue + adjustDelta(nextColorBlue - currentColorBlue, currentStep));
  pixels.show();
  delay(shiftDelayMillis);

  if (++currentPixel >= pixelsCount) {
    currentPixel = 0;
    if (++currentStep >= maxSteps) {
      resetColors();
    }
  }
}
