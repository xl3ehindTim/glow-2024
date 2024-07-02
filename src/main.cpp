#include <Arduino.h>
#include <DmxMaster.h>
#include <SoftwareSerial.h>
#include <Adafruit_VL53L1X.h>

#include "touch.h"
#include "dmxLighting.h"
#include "mp3Player.h"

bool isLifeless = true;
int currentBrightness = 0;

unsigned long lastAlive = 0;
bool isFadingIn = false;

const unsigned long lifelessDelay = 5000;

int calculateVolumeStep(int brightness, int steps, int beginSteps) {
    int volumeSteps = brightness / (steps - beginSteps);
    int volumeStep = currentBrightness / volumeSteps;
    return volumeStep;
}

void fadingIn(int maxBrightness, bool useMusic) {
    if (currentBrightness < maxBrightness) {
        currentBrightness++;
        changeBrightness(currentBrightness);

        if (useMusic) {
            //Serial.println(calculateVolumeStep(MAX_BRIGHTNESS, 30, 5));
            volumeMP3(calculateVolumeStep(MAX_BRIGHTNESS, 30, 5));

            // volumeMP3(newVolume);
        }
    } else {
        isFadingIn = false; // Stop fading in when max brightness is reached
    }
}

void fadingOut(int lowestBrightness, bool useMusic) {
    if (currentBrightness > lowestBrightness) {
        currentBrightness--;
        changeBrightness(currentBrightness);

        if (useMusic) {
            //Serial.println("music");
            //Serial.println(calculateVolumeStep(MAX_BRIGHTNESS, 30, 5));
            volumeMP3(calculateVolumeStep(MAX_BRIGHTNESS, 30, 5));
            // volumeMP3(newVolume);
        }
    }
}

void lifeless() {
    changeColor("white");

    if (currentBrightness == 0) {
        isFadingIn = true;
    } else if (currentBrightness >= 50) {
        isFadingIn = false;
    }

    if (isFadingIn) {
        fadingIn(50, false);
    } else {
        fadingOut(0, false);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting up...");

    setupMP3(9600);
    setupDmx();
    setupTouch();
    lastAlive = millis();  // Update the last touch time
    currentVolume = 1;
    volumeMP3(calculateVolumeStep(MAX_BRIGHTNESS, 30, 8));
}

void loop() {
    if (isLifeless) {
        lifeless();
        delay(40);
    }

    if (isTouched()) {
        if (isLifeless) {
            currentBrightness = 0; // Reset brightness to zero when touched
            changeBrightness(currentBrightness);
            isLifeless = false;
        }
        changeColor("purple");
        fadingIn(MAX_BRIGHTNESS, true);
        // myFunction();
        // volumeMP3(10);
        lastAlive = millis();  // Update lastAlive when touched
        delay(20);
    }  
    
    if (currentBrightness != 0 && !isTouched() && !isLifeless) {
        fadingOut(0, true);
        // myFunction();
        // volumeMP3(0);
        lastAlive = millis();  // Update the last touch time
        delay(60);
    } 
    
    if ((millis() - lastAlive) > lifelessDelay && !isTouched() && !isLifeless) {
        isLifeless = true;
        isFadingIn = true;
    } else if ((millis() - lastAlive) > lifelessDelay && isTouched()) {
        //Serial.println("lifeless");
    }
}
