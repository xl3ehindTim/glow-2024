#include <Arduino.h>
#include <DmxMaster.h>

/*
DMX uses multiple channels for rgb and brightness
*/
const int maxChannels = 4;

// Initialize rgb variables
int redValue = 0;

const int buttonPin = 11;

void setup()
{
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);

  // Select pin
  DmxMaster.usePin(3);

  // Set max channel (brightness, red, green, blue)
  DmxMaster.maxChannel(4);

  DmxMaster.write(1, 255);      
}

boolean isTouched()
{
  return digitalRead(buttonPin) == HIGH;
}

void loop()
{
  if (isTouched())
  {
    // Increase red value gradually (e.g., by 10)
    redValue = min(redValue + 1, 255); // Ensures value doesn't exceed 255
    Serial.println(redValue);
    DmxMaster.write(2, redValue); 
  }
  else
  {
    while (redValue != 0 && !isTouched())
    {
      // Decrease red value gradually
      redValue = redValue - 1;      
      DmxMaster.write(2, redValue);
      delay(25);
    }
  }
  delay(25);
}