#include <Arduino.h>
#include <DmxMaster.h>

void setup() {

  DmxMaster.usePin(3);

  DmxMaster.maxChannel(4);
}

void loop() {
  DmxMaster.write(1, 255);  //brightness
  DmxMaster.write(2, 128);  //red
  DmxMaster.write(3, 0);  //green
  DmxMaster.write(4, 0);  //blue

  delay(1000);

  DmxMaster.write(1, 128);  //brightness
  DmxMaster.write(2, 255);  //red
  DmxMaster.write(3, 0);  //green
  DmxMaster.write(4, 0);  //blue

  delay(1000);
}
