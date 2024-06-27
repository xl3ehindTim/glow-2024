#include <Arduino.h>

#ifndef DMXLIGHTING_H
#define DMXLIGHTING_H

#define DMX_PIN 3

#define MAX_BRIGHTNESS 100
#define MIN_BRIGHTNESS 0
#define BRIGHTNESS_STEPS 1
#define MAX_CHANNELS 4

void setupDmx();
void changeBrightness(int brightnessLevel);
void changeColor(String color);


#endif
