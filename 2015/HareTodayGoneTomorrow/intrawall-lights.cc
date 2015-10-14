#include <algorithm>

#include "Adafruit_TLC5947.h"

// How many boards do you have chained?
#define NUM_TLC5974 1

const int maxPWM = (1 << 12) - 1;

const int highDuty = maxPWM >> 4;
const int lowDuty = 0;

const int dataPin = 12;
const int clockPin = 13;
const int latchPin = 10;

int mapping[] = { 0, 1, 2,  8, 9, 10, 11,  12, 13, 14, 15,  20, 21, 22, 23 };

const unsigned leds = sizeof(mapping) / sizeof(int);

elapsedMillis globalTimer;

Adafruit_TLC5947 tlc = Adafruit_TLC5947(NUM_TLC5974, clockPin, dataPin, latchPin);

void lightOff(int id) {
  tlc.setPWM(mapping[id], lowDuty);
}

void lightOn(int id) {
  tlc.setPWM(mapping[id], highDuty);
}

void allLightsOff() {
  for (unsigned i = 0; i < leds; lightOff(i++)) { }
}

// ----- THE MAN -----

int theManPattern[] = { 0, // head
2, 14, // arms
7, 9 // legs
};

void runManPattern() {
  allLightsOff();
  for (unsigned i = 0; i < sizeof(theManPattern) / sizeof(int); ++i) {
    lightOn((theManPattern[i] + (globalTimer / 500)) % leds);
  }
  tlc.write();
}

// ----- THE WINDMILL -----

int theWindmillPattern[] = { 0, 1, 2,  5, 6, 7,  10, 11, 12 };

void runWindmillPattern() {
  allLightsOff();
  for (unsigned i = 0; i < sizeof(theWindmillPattern) / sizeof(int); ++i) {
    lightOn((theWindmillPattern[i] + (globalTimer / 500)) % leds);
  }
  tlc.write();
}

// ----- THE CHASE -----

int firstTailLength = 4, secondTailLength = 2;

void runChasingTailPattern() {
  allLightsOff();

  int firstOffset = globalTimer / 200;
  for (int i = 0; i < firstTailLength; ++i) {
    lightOn((firstOffset + i) % leds);
  }

  int secondOffset = globalTimer / 150;
  for (int i = 0; i < secondTailLength; ++i) {
    lightOn((secondOffset + i) % leds);
  }

  tlc.write();
}

// ----- RANDOM LIGHTS -----

unsigned randomBlinkRangeMillis = 3 * 1000; // 3 seconds

bool randomBlinksToggle[leds];

unsigned randomBlinksTTL[leds];

int generateRandomBlinksTTL() {
  return globalTimer + random() % randomBlinkRangeMillis;
}

void initRandomBlinks() {
  unsigned i;
  for (i = 0; i < leds; randomBlinksToggle[i++] = random() % 2) { }
  for (i = 0; i < leds; randomBlinksTTL[i++] = generateRandomBlinksTTL()) { }
}

void runRandomBlinks() {
  // dump the state
  unsigned i;
  bool anythingChanged = false;
  for (i = 0; i < leds; ++i) {
    if (globalTimer >= randomBlinksTTL[i]) {
      randomBlinksTTL[i] = generateRandomBlinksTTL();
      randomBlinksToggle[i] ^= true;
      anythingChanged = true;
    }
  }
  if (anythingChanged) {
    allLightsOff();
    for (i = 0; i < leds; ++i) {
      if (randomBlinksToggle[i]) {
        lightOn(i);
      }
    }
    tlc.write();
  }
}

// ----- MAIN PROGRAM -----

// ----- PATTERN SELECT -----

// -1 - Hold Pattern
// 0 - Random Blink
// 1 - The Windmill
// 2 - The Chase

const int patternsInUse = 3;

const int minPatternTimeMillis = 3 * 60 * 1000; // 3 minutes

const int patternTriggerRandomRangeMillis = 10 * 60 * 1000; // 10 minutes

int currentPattern = 0;

unsigned nextPatternTrigger = 0;

void chooseNextPattern() {
  int nextPattern;
  do {
    nextPattern = random() % patternsInUse;
  } while (currentPattern == nextPattern);
  currentPattern = nextPattern;
  switch (currentPattern) {
    case 0:
      initRandomBlinks();
      break;
    case 1:
      // no init for The Windmill
      break;
    case 2:
      // no init for The Chase
      break;
  }
  nextPatternTrigger = globalTimer + minPatternTimeMillis + random() % patternTriggerRandomRangeMillis;
}

// ----- BUTTON MANAGEMENT -----

const int holdPin = 22;
const int resetPin = 23;

const int holdLED = 18;
const int resetLED = 17;

bool resetButtonWasPressed = false;
bool holdButtonWasPressed = false;

bool holdButtonIsPressed() {
  return digitalRead(holdPin) == LOW;
}

bool resetButtonIsPressed() {
  return digitalRead(resetPin) == LOW;
}

void initButtons() {
  pinMode(holdPin, INPUT_PULLUP);
  pinMode(resetPin, INPUT_PULLUP);
  resetButtonWasPressed = holdButtonIsPressed();
  holdButtonWasPressed = holdButtonIsPressed();
  tlc.setPWM(holdLED, lowDuty);
  tlc.setPWM(resetLED, highDuty);
  tlc.write();
}

void runButtonsAndMaybeHold() {
  // hold button
  if (holdButtonWasPressed != holdButtonIsPressed()) {
    holdButtonWasPressed = holdButtonIsPressed();
    if (holdButtonWasPressed) {
      for (unsigned int i = 0; i < leds; lightOn(i++)) { }
      currentPattern = -1;
      tlc.setPWM(holdLED, highDuty);
    } else {
      chooseNextPattern();
      tlc.setPWM(holdLED, lowDuty);
    }
    tlc.write();
  }
  if (currentPattern < 0) {
    return;
  }
  // reset button
  if (resetButtonWasPressed != resetButtonIsPressed()) {
    resetButtonWasPressed = resetButtonIsPressed();
    if (resetButtonWasPressed) {
      chooseNextPattern();
      tlc.setPWM(resetLED, lowDuty);
    } else {
      tlc.setPWM(resetLED, highDuty);
    }
    tlc.write();
  }
}

// ----- MAIN BODY -----

void setup() {
  initButtons();
  tlc.begin();
  chooseNextPattern();
}

void loop() {
  runButtonsAndMaybeHold();
  // if it's not on hold and it's time
  if (currentPattern < 0) {
    return;
  }
  if (globalTimer >= nextPatternTrigger) {
    chooseNextPattern();
  }
  switch (currentPattern) {
    case 0:
      runRandomBlinks();
      break;
    case 1:
      runWindmillPattern();
      break;
    case 2:
      runChasingTailPattern();
      break;
  }
}
