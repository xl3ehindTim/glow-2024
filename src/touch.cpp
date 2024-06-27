#include <Arduino.h>

#include "touch.h"


void setupTouch() {
  pinMode(TOUCHPIN, INPUT);
}

boolean isTouched() {
  return digitalRead(TOUCHPIN) == HIGH;
}