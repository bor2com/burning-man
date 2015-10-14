#include <SPI.h>

#include <Adafruit_DotStar.h>

// ----- Microcontroller -----

const int pixelCount = 59;

const int dataPin = 22;
const int clockPin = 21;

Adafruit_DotStar strip = Adafruit_DotStar(pixelCount, dataPin, clockPin, DOTSTAR_BRG);

// ----- Pixels -----

const int maxBrigthness = 0x7f;
const int maxDurationMillis = 10 * 1000; // 10 seconds

int brightness[pixelCount];
uint32_t duration[pixelCount];
elapsedMillis currentTime[pixelCount];

void setup() {
  strip.begin();
  // Randomly set pixels
  for (int i = 0; i < pixelCount; ++i) {
    brightness[i] = random() % maxBrigthness;
    duration[i] = random() % maxDurationMillis;
  }
}

void loop() {
  int median;
  double ratio = 0;
  for (int i = 0; i < pixelCount; ++i) {
    if (currentTime[i] > duration[i]) {
      brightness[i] = random() % maxBrigthness;
      duration[i] = random() % maxDurationMillis;
      currentTime[i] = 0;
    }
    median = duration[i] / 2;
    ratio = sin(abs(int(currentTime[i]) - median) * M_PI / median);
    strip.setPixelColor(i, ratio * brightness[i]);
  }
  strip.show();
  /*
  for (int i = 0; i < pixelCount; ++i) {
      strip.setPixelColor(i, 100);
      strip.show();
      delay(100);
  }
  for (int i = 0; i < pixelCount; ++i) {
      strip.setPixelColor(i, 0);
      strip.show();
      delay(100);
  }
  /*
  strip.setPixelColor(0, 0x0f0000);
  strip.setPixelColor(1, 0x000f00);
  strip.setPixelColor(2, 0x00000f);
 //  strip.setPixelColor(tail, 0);     // 'Off' pixel at tail
  strip.show();                     // Refresh strip
  /*
  delay(20);                        // Pause 20 milliseconds (~50 FPS)

  if(++head >= NUMPIXELS) {         // Increment head index.  Off end of strip?
    head = 0;                       //  Yes, reset head index to start
    if((color >>= 8) == 0)          //  Next color (R->G->B) ... past blue now?
      color = 0xFF0000;             //   Yes, reset to red
  }
  if(++tail >= NUMPIXELS) tail = 0; // Increment, reset tail index
  */
}
