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

int currentVolume = 0;

// DMX uses multiple channels for RGB and brightness
const int maxChannels = 4;

// Initialize RGB variables
int brightness = 0;

// Pin connected to touch sensor
const int buttonPin = 11;

// Function to check if the button is touched
boolean isTouched() {
  return digitalRead(buttonPin) == HIGH;
}

// Function to send command to MP3 module
void sendCommand(byte command, byte dat1, byte dat2) {
  delay(20);
  Send_buf[0] = 0x7E;    //-> Every command should start with $(0x7E)
  Send_buf[1] = 0xFF;    //-> Version information
  Send_buf[2] = 0x06;    //-> The number of bytes of the command without starting byte and ending byte
  Send_buf[3] = command; //-> Such as PLAY and PAUSE and so on
  Send_buf[4] = 0x01;    //-> 0x00 = not feedback, 0x01 = feedback
  Send_buf[5] = dat1;    //-> data1
  Send_buf[6] = dat2;    //-> data2
  Send_buf[7] = 0xEF;    //-> Ending byte of the command

  for (uint8_t i = 0; i < 8; i++) {
    mp3.write(Send_buf[i]);
  }
}

// Function to set up DMX
void setupDmx() {
  // Setup DMX, uses multiple channels for brightness, red, green, blue and any other usage
  // depending on the device.
  DmxMaster.usePin(3);
  DmxMaster.maxChannel(maxChannels);

  // Set brightness to full
  DmxMaster.write(1, 255);
  DmxMaster.write(2, 255);
  DmxMaster.write(3, 0);
  DmxMaster.write(4, 0);
}

// Function to set up MP3 player
void setupMp3(int port = 9600) {
  mp3.begin(port);

  sendCommand(CMD_SEL_DEV, 0, DEV_TF);
  delay(500);
  
  sendCommand(CMD_PLAY, 0, 0);
}

// Function to adjust MP3 volume
void volumeMP3(int newVolume) {
  if (currentVolume == newVolume) {
    Serial.println("Volume is the same");
    return;
  } else if (newVolume == 0) {
    newVolume = 1;
  }
  while (currentVolume < newVolume)
  {
    sendCommand(CMD_VOLUME_UP, 0, 0);
    currentVolume++;
  }
  while (currentVolume > newVolume)
  {
    if (currentVolume == 1) {
      break;
    }
    sendCommand(CMD_VOLUME_DOWN, 0, 0);
    currentVolume--;
  }
  Serial.print("Volume is now: ");
  Serial.println(currentVolume);
}

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);

  setupMp3();
  setupDmx();
  volumeMP3(1);

}

void loop() {
  if (isTouched()) {
    // Ensure value doesn't exceed 255
    brightness = min(brightness + 1, 255);
    DmxMaster.write(1, brightness);
    //int callculateVolume = brightness / 255;
    int newVolume = brightness / 8.5;
    Serial.print(brightness);
    Serial.print(" - ");
    Serial.println(newVolume);
    volumeMP3(newVolume);
  }

  // Decrease red value gradually
  if (brightness != 0 && !isTouched()) {
    brightness = brightness - 1;
    DmxMaster.write(1, brightness);
    //.int callculateVolume = brightness / 255;
    int newVolume = brightness / 8.5;
    Serial.print(brightness);
    Serial.print(" - ");
    Serial.println(newVolume);
    volumeMP3(newVolume);
  }

  delay(25);
}
