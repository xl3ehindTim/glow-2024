#include <Arduino.h>
#include <DmxMaster.h>
#include <SoftwareSerial.h>

#include "touch.h"
#include "dmxlighting.h"
#include "mp3Player.h"

bool isLifeless = true;
int currentBrightness = 0;

unsigned long lastAlive = 0;
bool isFadingIn = false;

const unsigned long lifelessDelay = 5000;

int calculateVolumeStep(int brightness, int steps, int beginSteps)
{
    int volumeSteps = brightness / (steps - beginSteps);
    int volumeStep = currentBrightness / volumeSteps;
    return volumeStep;
}

void fadingIn(int maxBrightness, bool useMusic)
{
    if (currentBrightness < maxBrightness)
    {
        currentBrightness++;
        changeBrightness(currentBrightness);

        if (useMusic)
        {
            volumeMP3(calculateVolumeStep(MAX_BRIGHTNESS, 30, 5));
        }
    }
    else
    {
        isFadingIn = false; // Stop fading in when max brightness is reached
    }
}

void fadingOut(int lowestBrightness, bool useMusic)
{
    if (currentBrightness > lowestBrightness)
    {
        currentBrightness--;
        changeBrightness(currentBrightness);

        if (useMusic)
        {
            // Serial.println("music");
            // Serial.println(calculateVolumeStep(MAX_BRIGHTNESS, 30, 5));
            volumeMP3(calculateVolumeStep(MAX_BRIGHTNESS, 30, 5));
            // volumeMP3(newVolume);
        }
    }
}

void lifeless()
{
    changeColor("white");

    if (currentBrightness == 0)
    {
        isFadingIn = true;
    }
    else if (currentBrightness >= 50)
    {
        isFadingIn = false;
    }

    if (isFadingIn)
    {
        fadingIn(50, false);
    }
    else
    {
        fadingOut(0, false);
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting up...");

    setupMP3(9600);
    setupDmx();
    setupTouch();
    lastAlive = millis(); // Update the last touch time
    currentVolume = 1;
    volumeMP3(calculateVolumeStep(MAX_BRIGHTNESS, 30, 8));
}

void loop()
{
    if (isLifeless)
    {
        lifeless();
        delay(40);
    }

    int sensorTriggerValue = getSensorTriggerValue();

    if (sensorTriggerValue > 0)
    {
        if (isLifeless)
        {
            currentBrightness = 0;
            changeBrightness(currentBrightness);
            isLifeless = false;
        }
        changeColor("purple");

        // TODO: Change to sensorTriggerValue
        fadingIn(MAX_BRIGHTNESS, true);
        lastAlive = millis();
        delay(20);
    }

    if (currentBrightness != 0 && sensorTriggerValue == 0 && !isLifeless)
    {
        fadingOut(0, true);
        lastAlive = millis();
        delay(60);
    }

    if ((millis() - lastAlive) > lifelessDelay && sensorTriggerValue == 0 && !isLifeless)
    {
        isLifeless = true;
        isFadingIn = true;
    }
}
