#include <TaskScheduler.h>

const int RED_TIME = 3000;
const int YELLOW_TIME = 3000;
const int GREEN_TIME = 3000;
const int GREEN_BLINK = 5000;
const int GREEN_BLINK_INTERVAL = 500;

bool redOE();
void redOD();

bool yellowOE();
void yellowOD();

bool greenOE();
void greenOD();

bool greenBlinkOE();
void greenBlinkCB();
void greenBlinkOD();

Scheduler ts;

Task red(RED_TIME, TASK_ONCE, NULL, &ts, false, &redOE, &redOD);
Task yellow(YELLOW_TIME, TASK_ONCE, NULL, &ts, false, &yellowOE, &yellowOD);
Task green(GREEN_TIME, TASK_ONCE, NULL, &ts, false, &greenOE, &greenOD);
Task greenBlink(GREEN_BLINK_INTERVAL, GREEN_BLINK / GREEN_BLINK_INTERVAL, &greenBlinkCB, &ts, false, &greenBlinkOE, &greenBlinkOD);

bool redToGreen = true;

void setup() {
  pinMode(8, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(9, OUTPUT);

  redToGreen = true;
  red.restartDelayed();
}

void loop() {
  ts.execute();
}

bool redOE() {
  digitalWrite(8, HIGH);
  redToGreen = true;
  return true;
}

void redOD() {
  digitalWrite(8, LOW);
  yellow.restartDelayed();
}

bool yellowOE() {
  digitalWrite(11, HIGH);
  return true;
}

void yellowOD() {
  digitalWrite(11, LOW);
  if (redToGreen) {
    green.restartDelayed();
  }
  else {
    red.restartDelayed();
  }
}


bool greenOE() {
  digitalWrite(9, HIGH);
  return true;
}

void greenOD() {
  digitalWrite(9, LOW);
  greenBlink.restartDelayed();
}

bool greenState;
bool greenBlinkOE() {
  digitalWrite(9, HIGH);
  greenState = true;
  return true;
}

void greenBlinkCB()  {
  greenState = !greenState;
  digitalWrite(9, greenState ? HIGH : LOW);
}

void greenBlinkOD() {
  digitalWrite(9, LOW);
  yellow.restartDelayed();
  redToGreen = false;
}
