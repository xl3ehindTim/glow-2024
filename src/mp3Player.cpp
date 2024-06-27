#include <Arduino.h>
#include <SoftwareSerial.h>

#include "mp3Player.h"

SoftwareSerial mp3(6, 7);  // RX, TX

int currentVolume = 1;  // Definieer de globale variabele currentVolume

// Function to send command to MP3 module
void sendCommand(byte command, byte dat1, byte dat2) {
    byte Send_buf[8];  // Array to store the command bytes
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

// Function to set up MP3 player
void setupMP3(int port) {
    mp3.begin(port);

    sendCommand(CMD_SEL_DEV, 0, DEV_TF);
    delay(500);

    sendCommand(CMD_PLAY, 0, 0);
}

// Function to adjust MP3 volume
void volumeMP3(int newVolume) {
    if (currentVolume == newVolume) {
        //Serial.println("Volume is the same");
        return;
    } else if (newVolume == 0) {
        newVolume = 5;
    }
    while (currentVolume < newVolume) {
        sendCommand(CMD_VOLUME_UP, 0, 0);
        currentVolume++;
    }
    while (currentVolume > newVolume) {
        if (currentVolume == 5) {
            break;
        }
        sendCommand(CMD_VOLUME_DOWN, 0, 0);
        currentVolume--;
    }

    
    //Serial.print("Volume is now: ");
    //Serial.println(currentVolume);
}
