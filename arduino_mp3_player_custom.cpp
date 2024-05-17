/*
=================================================================== Arduino Serial MP3 Player Module Control with Serial Monitor

------------------------------------------------------------------- Reference
Reference : http://www.arducoding.com/2018/02/access-modules-mp3-player-yx5300-with.html
-------------------------------------------------------------------

------------------------------------------------------------------- Installation
Installation:

YX5300 MP3 Player Module      Arduino Uno
        RX <---------------------> 7
        TX <---------------------> 6
       VCC <---------------------> 5V
       GND <---------------------> GND
-------------------------------------------------------------------

------------------------------------------------------------------- About the commands
Asynchronous serial port control play mode:

|---------------------------------------------------------------------------------------------|
|                 Command bytes: $S VER Len CMD Feedback data $O                              |
|---------------------------------------------------------------------------------------------|
| Mark     | Byte | Byte description                                                          |
|---------------------------------------------------------------------------------------------|
| $S       | 0x7E | Every command should start with $(0x7E)                                   |
| VER      | 0xFF | Version information                                                       |
| Len      | 0xxx | The number of bytes of the command without starting byte and ending byte  |
| CMD      | 0xxx | Such as PLAY and PAUSE and so on                                          |
| Feedback | 0xxx | 0x00 = not feedback, 0x01 = feedback                                      |
| data     |      | The length of the data is not limit and usually it has two bytes          |
| $O       | 0xEF | Ending byte of the command                                                |
|---------------------------------------------------------------------------------------------|

For more information see the YX5300 Serial MP3 Player Module document, link to download it is available in the video description of this project.
-------------------------------------------------------------------
*/

#include <SoftwareSerial.h>

// RX YX5300 MP3 Player Module <-----> TX(7) SoftwareSerial Arduino Uno.
// TX YX5300 MP3 Player Module <-----> RX(6) SoftwareSerial Arduino Uno.
SoftwareSerial mp3(6,7); //-> SoftwareSerial mp3 (RX_Pin, TX_Pin);

// ------------------------------------------------------- The byte commands are based on the table contained in the MP3 Player Module Document.
// Link to download the MP3 Player Module Document is available in the video description.

#define CMD_NEXT_SONG         0X01    //-> Play next song.
#define CMD_PREV_SONG         0X02    //-> Play previous song.
#define CMD_PLAY_W_INDEX      0X03    //-> Play with index.
// Play with index : 7E FF 06 03 00 00 01 EF -> Play the first song.
// Play with index : 7E FF 06 03 00 00 02 EF -> Play the second song.

#define CMD_VOLUME_UP         0X04    //-> Volume increased one.
#define CMD_VOLUME_DOWN       0X05    //-> Volume decrease one.
#define CMD_SET_VOLUME        0X06    //-> Set volume.
// Set volume : 7E FF 06 06 00 00 1E EF -> Set the volume to 30 (0x1E is 30) (Set volume is 06).

#define CMD_SNG_CYCL_PLAY     0X08    //-> Single Cycle Play.
// Single Cycle Play : 7E FF 06 08 00 00 01 EF -> Single cycle play the first song.

#define CMD_SEL_DEV           0X09    //-> Select device.
#define DEV_TF                0X02    //-> 02 for TF card.
// Select device : 7E FF 06 09 00 00 02 EF -> Select storage device to TF card.

#define CMD_SLEEP_MODE        0X0A    //-> Chip enters sleep mode
#define CMD_WAKE_UP           0X0B    //-> Chip wakes up
#define CMD_RESET             0X0C    //-> Chip reset
#define CMD_PLAY              0X0D    //-> Resume playback
#define CMD_PAUSE             0X0E    //-> Playback is paused
#define CMD_PLAY_FOLDER_FILE  0X0F    //-> Play with folder and file name.
// Play with folder and file name : 7E FF 06 0F 00 01 01 EF -> Play the song with the directory: /01/001xxx.mp3
// Play with folder and file name : 7E FF 06 0F 00 01 02 EF -> Play the song with the directory: /01/002xxx.mp3

#define CMD_STOP_PLAY         0X16    //-> Stop playing continuously. 
#define CMD_FOLDER_CYCLE      0X17    //-> Cycle play with folder name.
// Cycle play with folder name : 7E FF 06 17 00 01 02 EF -> 01 folder cycle play

#define CMD_SHUFFLE_PLAY      0x18    //-> Shuffle play.
#define CMD_SET_SNGL_CYCL     0X19    //-> Set single cycle play.
// Set single cycle play : 7E FF 06 19 00 00 00 EF -> Start up single cycle play.
// Set single cycle play : 7E FF 06 19 00 00 01 EF -> Close single cycle play.

#define CMD_SET_DAC           0X1A    //-> Set DAC.
#define DAC_ON                0X00    //-> DAC On
#define DAC_OFF               0X01    //-> DAC Off
// Set DAC : 7E FF 06 1A 00 00 00 EF -> Start up DAC output.
// Set DAC : 7E FF 06 1A 00 00 01 EF -> DAC no output.

#define CMD_PLAY_W_VOL        0X22    //-> Play with volume.
// Play with volume : 7E FF 06 22 00 1E 01 EF -> Set the volume to 30 (0x1E is 30) and play the first song
// Play with volume : 7E FF 06 22 00 0F 02 EF -> Set the volume to 15(0x0f is 15) and play the second song

// Additional byte commands from several references. But I tried it on my module and it doesn't work. Maybe the module I'm using doesn't support these commands. 
// Because the module that I use uses a chip AA19HFA440-94 (Although in the shop where this module was purchased writing YX5300 chip). 
// Maybe these commands can work in modules with the YX5300 chip.
// Please comment in the comments column of this project video if these commands work or not in your module.
#define CMD_PLAYING_N         0x4C
#define CMD_QUERY_STATUS      0x42
#define CMD_QUERY_VOLUME      0x43
#define CMD_QUERY_FLDR_TRACKS 0x4e
#define CMD_QUERY_TOT_TRACKS  0x48
#define CMD_QUERY_FLDR_COUNT  0x4f
// -------------------------------------------------------

static int8_t Send_buf[8] = {0}; //-> Buffer to send commands. 
static uint8_t ansbuf[10] = {0}; //-> Buffer to receive response.  

// ******************************************************************************************************* setup()
void setup(){
  Serial.begin(9600);
  mp3.begin(9600);
  delay(500);
  
  Serial.println("Hardware Serial dan Software Serial begin.");
  delay(500);

  Serial.println();

  Serial.println("Select storage device to TF card.");
  sendCommand(CMD_SEL_DEV, 0, DEV_TF);
  delay(500); 
  
  printHelp();

  Serial.println();
}
// *******************************************************************************************************

// ******************************************************************************************************* loop()
void loop(){
  char c = ' ';

  // Incoming characters from the serial (Serial Monitor) are accommodated for processing and then sent to the mp3 player module.
  if ( Serial.available() ){
    c = Serial.read();
    sendMP3Command(c);
  }

  // Check response.
  if (mp3.available()){
    Serial.println(decodeMP3Answer());
  }
  
  delay(100);
}
// *******************************************************************************************************

// ******************************************************************************************************* printHelp()
// List of commands entered from the serial monitor.
void printHelp(){
    Serial.println("HELP  ");
    Serial.println(" h = Print again this Massage");
    Serial.println(" p = Play");
    Serial.println(" P = Pause");
    Serial.println(" > = Next");
    Serial.println(" < = Previous");
    Serial.println(" s = Stop Play"); 
    Serial.println(" + = Fade Volume UP");
    Serial.println(" - = Fade Volume DOWN");
    Serial.println(" c = Query current file");
    Serial.println(" q = Query status");
    Serial.println(" v = Query volume");
    Serial.println(" x = Query folder count");
    Serial.println(" t = Query total file count");
    Serial.println(" f = Play folder 1.");
    Serial.println(" S = Sleep");
    Serial.println(" W = Wake up");
    Serial.println(" r = Reset");
  }
// *******************************************************************************************************

// ******************************************************************************************************* sendMP3Command
// Process and send commands entered from the serial monitor.
void sendMP3Command(char c) {
  switch (c) {
    case '?':
    case 'h':
      printHelp();
      break;

    case 'p':
      Serial.println("Play ");
      sendCommand(CMD_PLAY);
      break;

    case 'P':
      Serial.println("Pause");
      sendCommand(CMD_PAUSE);
      break;

    case '>':
      Serial.println("Next");
      sendCommand(CMD_NEXT_SONG);
      sendCommand(CMD_PLAYING_N); //-> Chek the file number that is playing
      break;

    case '<':
      Serial.println("Previous");
      sendCommand(CMD_PREV_SONG);
      sendCommand(CMD_PLAYING_N); //-> Chek the file number that is playing
      break;

    case 's':
      Serial.println("Stop Play");
      sendCommand(CMD_STOP_PLAY);
      break;


    case '+':
      Serial.println("Fade Volume Up");
      fadevolumeup();
      break;

    case '-':
      Serial.println("Fade Volume Down");
      fadevolumedown();
      break;

    case 'c':
      Serial.println("Query current file");
      sendCommand(CMD_PLAYING_N);
      break;

    case 'q':
      Serial.println("Query status");
      sendCommand(CMD_QUERY_STATUS);
      break;

    case 'v':
      Serial.println("Query volume");
      sendCommand(CMD_QUERY_VOLUME);
      break;

    case 'x':
      Serial.println("Query folder count");
      sendCommand(CMD_QUERY_FLDR_COUNT);
      break;

    case 't':
      Serial.println("Query total file count");
      sendCommand(CMD_QUERY_TOT_TRACKS);
      break;

    case 'f':
      Serial.println("Playing folder 1");
      sendCommand(CMD_FOLDER_CYCLE, 1, 0);
      break;

    case 'S':
      Serial.println("Sleep");
      sendCommand(CMD_SLEEP_MODE);
      break;

    case 'W':
      Serial.println("Wake up");
      sendCommand(CMD_WAKE_UP);
      break;

    case 'r':
      Serial.println("Reset");
      sendCommand(CMD_RESET);
      break;
  }
}
// ******************************************************************************************************* 

// ******************************************************************************************************* decodeMP3Answer()
// The meaning of the mp3 player module response
String decodeMP3Answer() {
  String decodedMP3Answer = "";

  decodedMP3Answer += sanswer();

  switch (ansbuf[3]) {
    case 0x3A:
      decodedMP3Answer += " -> Memory card inserted.";
      break;

    case 0x3D:
      decodedMP3Answer += " -> Completed play num " + String(ansbuf[6], DEC);
      break;

    case 0x40:
      decodedMP3Answer += " -> Error";
      break;

    case 0x41:
      decodedMP3Answer += " -> Data recived correctly. ";
      break;

    case 0x42:
      decodedMP3Answer += " -> Status playing: " + String(ansbuf[6], DEC);
      break;

    case 0x48:
      decodedMP3Answer += " -> File count: " + String(ansbuf[6], DEC);
      break;

    case 0x4C:
      decodedMP3Answer += " -> Playing: " + String(ansbuf[6], DEC);
      break;

    case 0x4E:
      decodedMP3Answer += " -> Folder file count: " + String(ansbuf[6], DEC);
      break;

    case 0x4F:
      decodedMP3Answer += " -> Folder count: " + String(ansbuf[6], DEC);
      break;
  }

  return decodedMP3Answer;
}

// ******************************************************************************************************* sendCommand
// Function for sending commands.
void sendCommand(byte command){
  sendCommand(command, 0, 0);
}

void sendCommand(byte command, byte dat1, byte dat2){
  delay(20);
  Send_buf[0] = 0x7E;    //-> Every command should start with $(0x7E)
  Send_buf[1] = 0xFF;    //-> Version information
  Send_buf[2] = 0x06;    //-> The number of bytes of the command without starting byte and ending byte
  Send_buf[3] = command; //-> Such as PLAY and PAUSE and so on
  Send_buf[4] = 0x01;    //-> 0x00 = not feedback, 0x01 = feedback
  Send_buf[5] = dat1;    //-> data1
  Send_buf[6] = dat2;    //-> data2
  Send_buf[7] = 0xEF;    //-> Ending byte of the command
  
  Serial.print("Sending: ");
  
  for (uint8_t i = 0; i < 8; i++){
    mp3.write(Send_buf[i]) ;
    Serial.print(sbyte2hex(Send_buf[i]));
  }
  
  Serial.println();
}
// *******************************************************************************************************

// ******************************************************************************************************* Functions for conversion
// For byte to hex conversion.
String sbyte2hex(uint8_t b){
  String shex;

  shex = "0X";

  if (b < 16) shex += "0";
  shex += String(b, HEX);
  shex += " ";
  return shex;
}

// For hex to integer conversion.
int shex2int(char *s, int n){
  int r = 0;
  for (int i=0; i<n; i++){
     if(s[i]>='0' && s[i]<='9'){
      r *= 16; 
      r +=s[i]-'0';
     }else if(s[i]>='A' && s[i]<='F'){
      r *= 16;
      r += (s[i] - 'A') + 10;
     }
  }
  return r;
}

// ******************************************************************************************************* 

// ******************************************************************************************************* sanswer(void)
// Function: sanswer. Returns a String answer from mp3 UART module.          
// Parameter:- uint8_t b. void.                                                  
// Return: String. If the answer is well formated answer.                        

String sanswer(void){
  uint8_t i = 0;
  String mp3answer = ""; 

  // Get only 10 Bytes
  while (mp3.available() && (i < 10)){
    uint8_t b = mp3.read();
    ansbuf[i] = b;
    i++;

    mp3answer += sbyte2hex(b);
  }

  // if the answer format is correct.
  if ((ansbuf[0] == 0x7E) && (ansbuf[9] == 0xEF)){
    return mp3answer;
  }

  return "???: " + mp3answer;
}
// *******************************************************************************************************

// Volume up takes 3150 milliseconds
void fadevolumeup(){
      Serial.println("Fade Volume Down");
      
      // Volume has 30 steps. Since the volume can't be measured, the volume up command does the max amount of volume up commands.
      for(int i=0; i<15; i++){
        delay(10);
        sendCommand(CMD_VOLUME_UP);
      }

      for(int i=0; i<15; i++){
        delay(200);
        sendCommand(CMD_VOLUME_UP);
      }
}

// Volume fades down in 
void fadevolumedown(){
      Serial.println("Fade Volume Down");

      int STEPS = 30;
      int FADE_TIME = 4500/STEPS; // In milliseconds

      // Volume has 30 steps. Since the volume can't be measured, the volume down command does the max amount of volume down commands.
      for(int i=0; i<STEPS; i++){
        delay(FADE_TIME);
        sendCommand(CMD_VOLUME_DOWN);
      }
}
