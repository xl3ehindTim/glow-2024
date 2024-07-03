#include "touch.h"
#include <VL53L1X.h>
#include <Wire.h>

const uint8_t sensorCount = 3;

// The Arduino pin connected to the XSHUT pin of each sensor. The array consist out of an array where the sensor pin (value 0) is connected to a light pin (value 1)
const uint8_t xshutPins[sensorCount][2] = {{8, 1}, {9, 2}, {10, 3}};

int lastInput[sensorCount];

VL53L1X sensors[sensorCount];

void setupTouch()
{
  Wire.begin();

  // Disable/reset all sensors/lights by driving their XSHUT pins low.
  for (uint8_t i = 0; i < sensorCount; i++)
  {
    pinMode(xshutPins[i][0], OUTPUT);
    pinMode(xshutPins[i][1], OUTPUT);
    digitalWrite(xshutPins[i][0], LOW);
    digitalWrite(xshutPins[i][1], HIGH);
  }

  // Enable, initialize, and start each sensor, one by one.
  for (uint8_t i = 0; i < sensorCount; i++)
  {
    // Stop driving this sensor's XSHUT low. This should allow the carrier
    // board to pull it high. (We do NOT want to drive XSHUT high since it is
    // not level shifted.) Then wait a bit for the sensor to start up.
    pinMode(xshutPins[i][0], INPUT);
    delay(10);

    sensors[i].setTimeout(500);
    if (!sensors[i].init())
    {
      Serial.print("Failed to detect and initialize sensor ");
      Serial.print(i);
      while (1)
        ;
    } else {
      digitalWrite(xshutPins[i][1], LOW);
    }

    // Each sensor must have its address changed to a unique value other than
    // the default of 0x29 (except for the last one, which could be left at
    // the default). To make it simple, we'll just count up from 0x2A.
    sensors[i].setAddress(0x2A + i);
    sensors[i].startContinuous(50);
  }
}

int getSensorTriggerValue() {
  int triggeredSensors = 0;

  for (int i = 0; i < sensorCount; i++) {
    if (sensors[i].dataReady()) {
      int16_t distance = sensors[i].read();
      
      // Check if the distance is within the trigger threshold (adjust threshold as needed)
      if (distance < 1200) {
        triggeredSensors++;
        lastInput[i] = true; 
        digitalWrite(xshutPins[i][1], HIGH);
      } else {
        lastInput[i] = false;
        digitalWrite(xshutPins[i][1], LOW);
      }
    } else {
      // Sensor not dataReady, use lastInput state
      triggeredSensors += lastInput[i];
    }
  }

  // Calculate percentage of triggered sensors (0 - 100) based on amount of sensors
  int triggerPercentage = (triggeredSensors * 100) / sensorCount;
  return triggerPercentage;
}