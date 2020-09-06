#include "FastLED.h"
#include "faces.h"

// Matrix size
#define NUM_ROWS 28
#define NUM_COLS 26

// Define pins
#define DATA_PIN 3
#define BUTTON_0 9
#define BUTTON_1 10
#define BUTTON_2 11

// LED brightness
#define BRIGHTNESS 255

// Define the array of leds
#define NUM_LEDS NUM_ROWS * NUM_COLS
CRGB leds[NUM_LEDS];

// Animation controls
byte pressed = 1; // Tracks which face is currently being displayed
bool button0Pressed = false;
bool button1Pressed = false;
bool button2Pressed = false;


void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  mid(); // Start the display with the forward-facing face
}

void loop() {
  // Read the button inputs
  button0Pressed = digitalRead(BUTTON_0) == LOW;
  button1Pressed = digitalRead(BUTTON_1) == LOW;
  button2Pressed = digitalRead(BUTTON_2) == LOW;

  // If an input occured AND it's different than the current face, display that face
  if (button0Pressed && !(pressed == 0)) {
    pressed = 0;
    left();
  }
  if (button1Pressed && !(pressed == 1)) {
    pressed = 1;
    mid();
  }
  if (button2Pressed && !(pressed == 2)) {
    pressed = 2;
    right();
  }

  // Blink the eyes ocassionally
  EVERY_N_MILLISECONDS_I(blinktime, 1000) {
    blinking();
    // After blinking, return to the previous face
    switch (pressed) {
      case 0: left(); break;
      case 1: mid(); break;
      case 2: right(); break;
    }
    // Set the next blink delay
    blinktime.setPeriod( random16(1000, 3000) );
  }
}

// Face-related functions:
void mid() {
  // Read the forward face from PROGMEM, then display it.
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = pgm_read_dword_near(IdleFace + i);
  }
  FastLED.show();
}

void left() {
  // Read the left face from PROGMEM, then display it.
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = pgm_read_dword_near(LeftFace + i);
  }
  FastLED.show();
}

void right() {
  // Read the right face from PROGMEM, then display it.
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = pgm_read_dword_near(RightFace + i);
  }
  FastLED.show();
}

void blinking() {
  // Pick the appropriate blinking face based on the current facing
  switch (pressed) {
    case 0: leftB();   break;
    case 1: midB();    break;
    case 2: rightB();  break;
  }
}

void midB() {
  // Read the right face from PROGMEM, then display it.
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = pgm_read_dword_near(IdleBlink + i);
  }
  FastLED.show();
  // Hold the blink for 50 milliseconds so the blink is visible
  delay(50);
}

void leftB() {
  // Read the right face from PROGMEM, then display it.
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = pgm_read_dword_near(LeftBlink + i);
  }
  FastLED.show();
  // Hold the blink for 50 milliseconds so the blink is visible
  delay(50);
}

void rightB() {
  // Read the right face from PROGMEM, then display it.
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = pgm_read_dword_near(RightBlink + i);
  }
  FastLED.show();
  // Hold the blink for 50 milliseconds so the blink is visible
  delay(50);
}
