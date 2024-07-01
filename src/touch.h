#include <Arduino.h>

#ifndef TOUCH_H
#define TOUCH_H


#define IRQ_PIN 2
#define XSHUT_PIN 3

void setupTouch();

boolean isTouched();

#endif
