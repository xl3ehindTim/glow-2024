#include <Arduino.h>
#include <DmxMaster.h>

#include "dmxLighting.h"

int channels[MAX_CHANNELS + 1] = {0};  // Define the array with an extra element to use 1-based indexing

void updateColor() {
    for (int i = 1; i <= MAX_CHANNELS; i++) {
        DmxMaster.write(i, channels[i]);
    }

    //Serial.print("updateColor: ");
    for (int i = 1; i <= MAX_CHANNELS; i++) {
        //Serial.print(channels[i]);
        if (i < MAX_CHANNELS) {
            //Serial.print(" - ");
        }
    }
    //Serial.println();
}

void byteColor(int red, int green, int blue) {
    channels[2] = red;
    channels[3] = green;
    channels[4] = blue;
    updateColor();
}

void changeColor(String color) {
    if (color.equals("red")) {
        byteColor(255, 0, 0);
    } else if (color.equals("green")) {
        byteColor(0, 255, 0);
    } else if (color.equals("blue")) {
        byteColor(0, 0, 255);
    } else if (color.equals("yellow")) {
        byteColor(255, 255, 0);
    } else if (color.equals("purple")) {
        byteColor(255, 0, 255);
    } else if (color.equals("cyan")) {
        byteColor(0, 255, 255);
    } else if (color.equals("white")) {
        byteColor(255, 255, 255);
    }
}

void changeBrightness(int brightnessLevel) {
    channels[1] = brightnessLevel;
    updateColor();
}

void setupDmx() {
    DmxMaster.usePin(DMX_PIN);
    DmxMaster.maxChannel(MAX_CHANNELS);

    changeColor("white");
}
