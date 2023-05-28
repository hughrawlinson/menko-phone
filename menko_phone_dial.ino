#include <AcksenButton.h>
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

#define BUTTON_PIN 4
#define CLICK_TIME 500
#define BUTTON_DEBOUNCE_INTERVAL 10

int count = 0;

int sent = 1;
int pressSendTime = 0;

AcksenButton rotary = AcksenButton(BUTTON_PIN, ACKSEN_BUTTON_MODE_NORMAL, BUTTON_DEBOUNCE_INTERVAL, INPUT_PULLUP);

SoftwareSerial mySoftwareSerial(A1, A0); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

void setup() {
  Serial.begin(115200);
  mySoftwareSerial.begin(9600);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }

  myDFPlayer.volume(25);  //Set volume value. From 0 to 30
  Serial.println("Hello");
}

void loop() {
  rotary.refreshStatus();

  int newTime = millis();

  if (rotary.onPressed()) {
    count++;
    pressSendTime = newTime + CLICK_TIME;
    sent = 0;
  }

  if (!sent && newTime > pressSendTime) {
    if (count < 11) {
      int value = count;
      if (value == 10) {
        value = 0;
      }

      Serial.println(value);
      myDFPlayer.play(value + 1);

      count = 0;
      sent = 1;
    } else {
      count = 0;
      sent = 1;
    }
  }
  
  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
}

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  } 
}