#include <Arduino.h>

const int touchPin = 11;

void setup() {
  Serial.begin(9600);
  pinMode(touchPin, INPUT);  // Set touchPin as input
}
void loop() {
  while (digitalRead(touchPin) == LOW);
  {
    Serial.println("Touch detected!");
  }
}