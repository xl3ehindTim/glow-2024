#include "mpr121.h"
#include <Wire.h>

#define TOU_THRESH 60  // Adjusted for acrylic sensitivity
#define REL_THRESH 30  // Adjusted for acrylic sensitivity

int irqpin = 2;  // Digital 2
boolean touchStates[12]; //to keep track of the previous touch states

void setup(){
  pinMode(irqpin, INPUT);
  digitalWrite(irqpin, HIGH); //enable pullup resistor
  
  Serial.begin(9600);
  Wire.begin();

  mpr121_setup();
}

void loop(){
  readTouchInputs();
}

void readTouchInputs(){
  if(!checkInterrupt()){
    //read the touch state from the MPR121
    Wire.requestFrom(0x5A, 2); 
    
    byte LSB = Wire.read();
    byte MSB = Wire.read();
    
    uint16_t touched = ((MSB << 8) | LSB); //16bits that make up the touch states

    for (int i = 0; i < 12; i++){  // Check what electrodes were pressed
      if(touched & (1 << i)){
        if(touchStates[i] == 0){
          //pin i was just touched
          Serial.print("pin ");
          Serial.print(i);
          Serial.println(" was just touched");
        }
        touchStates[i] = 1;      
      } else {
        if(touchStates[i] == 1){
          Serial.print("pin ");
          Serial.print(i);
          Serial.println(" is no longer being touched");
        }
        touchStates[i] = 0;
      }
    }
  }
}

void mpr121_setup(void){
  set_register(0x5A, ELE_CFG, 0x00); 
  
  // Section A - Controls filtering when data is > baseline.
  set_register(0x5A, MHD_R, 0x01);
  set_register(0x5A, NHD_R, 0x01);
  set_register(0x5A, NCL_R, 0x00);
  set_register(0x5A, FDL_R, 0x00);

  // Section B - Controls filtering when data is < baseline.
  set_register(0x5A, MHD_F, 0x01);
  set_register(0x5A, NHD_F, 0x01);
  set_register(0x5A, NCL_F, 0xFF);
  set_register(0x5A, FDL_F, 0x02);
  
  // Section C - Sets touch and release thresholds for each electrode
  for (int i = 0; i < 12; i++) {
    set_register(0x5A, ELE0_T + i * 2, TOU_THRESH);
    set_register(0x5A, ELE0_R + i * 2, REL_THRESH);
  }
  
  // Section D
  // Set the Filter Configuration
  // Set ESI2
  set_register(0x5A, FIL_CFG, 0x04);
  
  // Section E
  // Electrode Configuration
  // Set ELE_CFG to 0x00 to return to standby mode
  set_register(0x5A, ELE_CFG, 0x0C);  // Enables all 12 Electrodes
  
  // Section F
  // Enable Auto Config and auto Reconfig
  set_register(0x5A, ATO_CFG0, 0x0B); // Enable auto-configuration
  set_register(0x5A, ATO_CFGU, 0xC9); // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V
  set_register(0x5A, ATO_CFGL, 0x82); // LSL = 0.65*USL = 0x82 @3.3V
  set_register(0x5A, ATO_CFGT, 0xB5); // Target = 0.9*USL = 0xB5 @3.3V
  
  set_register(0x5A, ELE_CFG, 0x0C);
}

boolean checkInterrupt(void){
  return digitalRead(irqpin);
}

void set_register(int address, unsigned char r, unsigned char v){
    Wire.beginTransmission(address);
    Wire.write(r);
    Wire.write(v);
    Wire.endTransmission();
}