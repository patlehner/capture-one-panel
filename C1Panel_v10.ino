// Capture One Panel - translates rotary encoder input into keyboard shortcuts
// Patrick Lehner, post@patricklehner.ch, 2018


// ---------------------------------------------------------------------

#include "RotaryEncoder.h"
#include "Keyboard.h"

const int encoderCount = 15;
const int switchCount = 2;
const int firstPin = 22;
const int undoRedo = 1;  // encoder number for undo and redo
const int keyPressDelay = 10;
const int switchPressDelay = 100;
boolean switchReleased[switchCount];
char encoderShortcuts[ ] = "kybmwshrdufvecl";
char switchShortcuts[ ] = "lr";
RotaryEncoder *encoders[encoderCount];


// ---------------------------------------------------------------------

void setup() {
  for (int n = 0; n < encoderCount; n++) {
    encoders[n] = new RotaryEncoder(firstPin + 2 * n, firstPin + 2 * n + 1);
  }
  for (int p = firstPin; p < firstPin + 2 * encoderCount; p++) {
    attachInterrupt(digitalPinToInterrupt(p), readEncoders, CHANGE);
  }
  for (int n = 0; n < switchCount; n++) {
    switchReleased[n] = true;
    pinMode(firstPin + 2 * encoderCount + n, INPUT);
  }
  Keyboard.begin();
}


// ---------------------------------------------------------------------

void readEncoders() {
  for (int n = 0; n < encoderCount; n++) {
    encoders[n]->tick();
  }
}


// ---------------------------------------------------------------------

void loop() {
  int newPosition;
  for (int n = 0; n < encoderCount; n++) {
    newPosition = encoders[n]->getPosition();
    if (newPosition != 0) {
      switch (n) {
        case undoRedo:
          Keyboard.press(KEY_LEFT_GUI);
          if (newPosition < 0) Keyboard.press(KEY_LEFT_SHIFT);
          Keyboard.press(encoderShortcuts[n]);
          break;
        default:
          Keyboard.press(KEY_RIGHT_CTRL);
          if (newPosition < 0) Keyboard.press(KEY_RIGHT_SHIFT);
          Keyboard.press(encoderShortcuts[n]);
          break;
      }
      delay(keyPressDelay);
      Keyboard.releaseAll();
      encoders[n]->setPosition(0);
    }
  }
  for (int n = 0; n < switchCount; n++) {
    if (digitalRead(firstPin + 2 * encoderCount + n) == HIGH) {
      switchReleased[n] = true;
    } else if (switchReleased[n]) {
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press(switchShortcuts[n]);
      delay(keyPressDelay);
      Keyboard.releaseAll();
      delay(switchPressDelay);
      switchReleased[n] = false;
    }
  }
}
