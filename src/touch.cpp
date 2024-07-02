#include "Adafruit_VL53L1X.h"

#include "touch.h"

bool lastInput = false;

Adafruit_VL53L1X vl53 = Adafruit_VL53L1X(XSHUT_PIN, IRQ_PIN);

void setupTouch()
{
  Serial.println("Setup touch");
  Wire.begin();
  Serial.println("begin wire");

  if (!vl53.begin(0x29, &Wire))
  {
    Serial.print(F("Error on init of VL sensor: "));
    while (1) delay(10);
  } else {
    Serial.println("vl153");
  }

  if (!vl53.startRanging())
  {
    Serial.print(F("Couldn't start ranging: "));
    Serial.println(vl53.vl_status);
    while (1) delay(10);
  } else {
    Serial.println("Time of Flight on");
  }

  vl53.setTimingBudget(50);

  /*
  vl.VL53L1X_SetDistanceThreshold(100, 300, 3, 1);
  vl.VL53L1X_SetInterruptPolarity(0);
  */
}

boolean isTouched()
{
  int16_t distance;

  if (!vl53.dataReady()) 
  {
    return lastInput;
  }

  // new measurement for the taking!
  if (vl53.dataReady())
  {
    distance = vl53.distance();
    if (distance == -1)
    {
      lastInput = false;
      return false;
    }

    // data is read out, time for another reading!
    vl53.clearInterrupt();

    Serial.println(distance);

    if (distance < 1200)
    {
      lastInput = true;
      return true;
    }

    lastInput = false;
    return false;
  }

  return false;
}