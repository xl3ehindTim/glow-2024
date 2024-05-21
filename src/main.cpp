#include <Arduino.h>
#include <DmxMaster.h>
#include <SoftwareSerial.h>

SoftwareSerial mp3(6, 7);

#define CMD_VOLUME_UP 0X04   //-> Volume increased one.
#define CMD_VOLUME_DOWN 0X05 //-> Volume decrease one.
#define CMD_SET_VOLUME 0X06  //-> Set volume.
#define CMD_PLAY 0X0D        //-> Resume playback

#define CMD_SEL_DEV 0X09 //-> Select device.
#define DEV_TF 0X02      //-> 02 for TF card.

static int8_t Send_buf[8] = {0}; //-> Buffer to send commands.

/*
DMX uses multiple channels for rgb and brightness
*/
const int maxChannels = 4;

// Initialize rgb variables
int redValue = 0;

const int buttonPin = 11;

boolean isTouched()
{
  return digitalRead(buttonPin) == HIGH;
}

void sendCommand(byte command, byte dat1, byte dat2)
{
  delay(20);
  Send_buf[0] = 0x7E;    //-> Every command should start with $(0x7E)
  Send_buf[1] = 0xFF;    //-> Version information
  Send_buf[2] = 0x06;    //-> The number of bytes of the command without starting byte and ending byte
  Send_buf[3] = command; //-> Such as PLAY and PAUSE and so on
  Send_buf[4] = 0x01;    //-> 0x00 = not feedback, 0x01 = feedback
  Send_buf[5] = dat1;    //-> data1
  Send_buf[6] = dat2;    //-> data2
  Send_buf[7] = 0xEF;    //-> Ending byte of the command

  for (uint8_t i = 0; i < 8; i++)
  {
    mp3.write(Send_buf[i]);
  }
}

void setupDmx()
{
  /*
  Setup DMX, uses multiple channels for brightness, red, green, blue and any other usage
  depending on the device.
  */
  DmxMaster.usePin(3);
  DmxMaster.maxChannel(4);

  // Set brightness to full
  DmxMaster.write(1, 255);
}

void setupMp3(int port = 9600)
{
  mp3.begin(port);

  sendCommand(CMD_SEL_DEV, 0, DEV_TF);
  delay(500);
  
  sendCommand(CMD_PLAY, 0, 0);
}

void setup()
{
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);

  setupMp3();
  setupDmx();
}

void loop()
{
  if (isTouched())
  {
    // Ensures value doesn't exceed 255
    redValue = min(redValue + 1, 255); 
    Serial.println(redValue);
    DmxMaster.write(2, redValue);
    sendCommand(CMD_VOLUME_UP, 0, 0);
  }

  // Decrease red value gradually
  if (redValue != 0 && !isTouched())
  {
    redValue = redValue - 1;
    DmxMaster.write(2, redValue);

    sendCommand(CMD_VOLUME_DOWN, 0, 0);
  }

  delay(25);
}
