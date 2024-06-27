#ifndef MP3PLAYER_H
#define MP3PLAYER_H

#include <Arduino.h>

extern int currentVolume;

void sendCommand(byte command, byte dat1, byte dat2);
void setupMP3(int port);
void volumeMP3(int newVolume);

#define CMD_SEL_DEV 0x09
#define DEV_TF 0x02
#define CMD_PLAY 0x0D
#define CMD_VOLUME_UP 0x04
#define CMD_VOLUME_DOWN 0x05
#define MAX_VOLUME 30

#endif
