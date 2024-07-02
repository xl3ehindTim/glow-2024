// #include "Adafruit_VL53L1X.h"
#include "touch.h"
#include <VL53L1X.h>
#include <Wire.h>

// #define XSHUT_PIN_1 8
// #define XSHUT_PIN_2 9
// #define XSHUT_PIN_3 10

// int XSHUT_PINS[] = {8, 9, 10};

// #define AMOUNT_OF_SENSORS 3
// Define desired I2C addresses (ensure they don't conflict with other devices)

// int sensorAddresses[] = {0x29, 0x30, 0x31};

// Adafruit_VL53L1X sensors[AMOUNT_OF_SENSORS] = {
//     Adafruit_VL53L1X(),
//     Adafruit_VL53L1X(),
//     Adafruit_VL53L1X(),
// };

// bool lastInput[AMOUNT_OF_SENSORS]; // Array to store last touch state for each sensor

// bool lastInput = false;

// Adafruit_VL53L1X vl53 = Adafruit_VL53L1X(XSHUT_PIN, IRQ_PIN);

// The number of sensors in your system.
const uint8_t sensorCount = 2;

// The Arduino pin connected to the XSHUT pin of each sensor.
const uint8_t xshutPins[sensorCount] = { 8, 10 };
int adresses[sensorCount] = { 0x2A, 0x2B };

VL53L1X sensors[3];

void setupTouch()
{
  while (!Serial) { }
  Wire.begin();

  // Disable/reset all sensors by driving their XSHUT pins low.
  for (uint8_t i = 0; i < sensorCount; i++)
  {
    pinMode(xshutPins[i], OUTPUT);
    digitalWrite(xshutPins[i], LOW);
  }

  // Enable, initialize, and start each sensor, one by one.
  for (uint8_t i = 0; i < sensorCount; i++)
  {
    // Stop driving this sensor's XSHUT low. This should allow the carrier
    // board to pull it high. (We do NOT want to drive XSHUT high since it is
    // not level shifted.) Then wait a bit for the sensor to start up.
    pinMode(xshutPins[i], INPUT);
    delay(10);

    Serial.println("test");

    sensors[i].setTimeout(500);
    if (!sensors[i].init())
    {
      Serial.print("Failed to detect and initialize sensor ");
      Serial.println(i);
      while (1)
        ;
    }
    Serial.println("test");

    // Each sensor must have its address changed to a unique value other than
    // the default of 0x29 (except for the last one, which could be left at
    // the default). To make it simple, we'll just count up from 0x2A.
    sensors[i].setAddress(adresses[i]); //.setAddress(0x2A + i);
    // Serial.println("0x2A" + i);
    sensors[i].startContinuous(50);
  }

    while (1) 
    {
    for (uint8_t i = 0; i < sensorCount; i++)
    {
      Serial.println(sensors[i].read());
      Serial.println(sensors[i].getAddress(), HEX);
      if (sensors[i].timeoutOccurred())
      {
        Serial.print(" TIMEOUT");
      }
      Serial.print('\t');
    }
    }
    Serial.println();

  // return;
  // Wire.begin();

  // for (int i = 0; i < 1; i++)
  // {
  //   Adafruit_VL53L1X vl53 = Adafruit_VL53L1X(XSHUT_PIN, IRQ_PIN);
  //   vl53.begin(sensorAddresses[i], &Wire);
  //   vl53.setTimingBudget(50);
  //   sensors[i] = vl53;
  //   // sensors[i].begin(sensorAddresses[i], &Wire);
  //   Serial.print("Sensor ");
  //   Serial.print(i);
  //   Serial.print(" initialized. Address: 0x");
  //   Serial.println(sensorAddresses[i], HEX);
  //   Wire.endTransmission();
  // }

  // return;
  // for (int i = 0; i < AMOUNT_OF_SENSORS; i++)
  // {
  //   pinMode(XSHUT_PINS[i], OUTPUT);
  //   digitalWrite(XSHUT_PINS[i], LOW);
  // }
  // pinMode(XSHUT_PIN_1, OUTPUT);
  // pinMode(XSHUT_PIN_2, OUTPUT);
  // pinMode(XSHUT_PIN_3, OUTPUT);

  // digitalWrite(XSHUT_PIN_1, LOW);
  // digitalWrite(XSHUT_PIN_2, LOW);
  // digitalWrite(XSHUT_PIN_3, LOW);

  // Initialize sensors
  // for (int i = 0; i < AMOUNT_OF_SENSORS; i++)
  // {
  //   // digitalWrite(XSHUT_PINS[i], HIGH);
  //   // digitalWrite(XSHUT_PIN_1, HIGH);
  //   // Set temporary I2C address for current sensor
  //   Wire.beginTransmission(0x29);   // Write register address for address selection (0x52)
  //   Wire.write(0x00);               // Select register for I2C address configuration
  //   Wire.write(sensorAddresses[i]); // Write desired I2C address
  //   Serial.println("test 1");
  //   // Adafruit_VL53L1X sensor = Adafruit_VL53L1X(XSHUT_PINS[i]);
  //   // sensor.begin(sensorAddresses[i], &Wire);
  //   Serial.println("test 2");
  //   // sensors[i] = sensor;
  //   Serial.println("added sensor " + i);
  //   // Wire.endTransmission();
  // }
  // // Initialize sensor object with default address (0x29) - this will initialize with the temporarily set address
  // //   if (!sensors[i].begin(0x52, &Wire)) {
  // //     Serial.print(F("Error initializing sensor: "));
  // //     Serial.print(i);
  // //     while (1) delay(10);
  // //   } else {
  // //     Serial.print("Sensor ");
  // //     Serial.print(i);
  // //     Serial.println(" initialized");
  // //   }
  // // }

  // // Initialize lastInput array
  // for (int i = 0; i < AMOUNT_OF_SENSORS; i++)
  // {
  //   lastInput[i] = false;
  // }

  // Rest of your setup code...
}

boolean isTouched()
{
  bool anyTouched = false;
  return anyTouched;

  // for (int i = 0; i < AMOUNT_OF_SENSORS; i++)
  // {
  //   if (sensors[i].dataReady())
  //   {
  //     int16_t distance = sensors[i].distance();
  //     // Handle distance data and update anyTouched flag
  //     if (distance < 1200)
  //     {
  //       anyTouched = true;
  //       lastInput[i] = true; // Update last touched state for this sensor
  //       break;               // Exit loop if any sensor detects touch
  //     }
  //     else
  //     {
  //       lastInput[i] = false; // Update last touched state for this sensor
  //     }
  //     // Clear interrupt (if applicable) and handle other processing for sensor i
  //   }
  // }

  // // Update overall touched state based on anyTouched flag
  // return anyTouched;
}

// void setupTouch()
// {
//   Serial.println("Setup touch");
//   Wire.begin();
//   Serial.println("begin wire");

//   if (!vl53.begin(0x29, &Wire))
//   {
//     Serial.print(F("Error on init of VL sensor: "));
//     while (1) delay(10);
//   } else {
//     Serial.println("vl153");
//   }

//   if (!vl53.startRanging())
//   {
//     Serial.print(F("Couldn't start ranging: "));
//     Serial.println(vl53.vl_status);
//     while (1) delay(10);
//   } else {
//     Serial.println("Time of Flight on");
//   }

//   vl53.setTimingBudget(50);
// }

// boolean isTouched()
// {
//   int16_t distance;

//   if (!vl53.dataReady())
//   {
//     return lastInput;
//   }

//   // new measurement for the taking!
//   if (vl53.dataReady())
//   {
//     distance = vl53.distance();
//     if (distance == -1)
//     {
//       lastInput = false;
//       return false;
//     }

//     // data is read out, time for another reading!
//     vl53.clearInterrupt();

//     Serial.println(distance);

//     if (distance < 1200)
//     {
//       lastInput = true;
//       return true;
//     }

//     lastInput = false;
//     return false;
//   }
// }
