#include <SPI.h>
#include "RF24.h"
#include "LowPower.h"
#define interruptpin 2
RF24 radio(7, 8);

const int buttonscrollf=4;
const int buttonscrollb=3;
const int buttonsetsemi=5;
const int buttonsetfull=6;

int timer;
int dataTransmitted;
const uint64_t pipe = 0x123456789ABC;
void wakeUp() {
  timer = 0;
}

void setup() {
  Serial.begin(115200);
  pinMode(buttonscrollf, INPUT_PULLUP);
  pinMode(buttonscrollb, INPUT_PULLUP);
  pinMode(buttonsetsemi, INPUT_PULLUP);
  pinMode(buttonsetfull, INPUT_PULLUP);
  pinMode(interruptpin, INPUT_PULLUP);
  pinMode(10,OUTPUT);
  digitalWrite(10,HIGH); 
  timer = 0;
  radio.begin();
  radio.setChannel(115);
  radio.setPALevel(RF24_PA_MAX);
  radio.openWritingPipe(pipe); 
}


void loop() {
  attachInterrupt(digitalPinToInterrupt(interruptpin), wakeUp, LOW);
  while (timer < 3000) {
    if (digitalRead(buttonscrollf) == LOW) { 
      radio.powerUp();
      timer = 0;
      dataTransmitted = 101;
      radio.write( &dataTransmitted, sizeof(dataTransmitted) );
      radio.powerDown();
      delay(200);
      Serial.println(dataTransmitted);
    }

    if (digitalRead(buttonscrollb) == LOW) { 
      radio.powerUp();
      timer = 0;
      dataTransmitted = 100;
      radio.write( &dataTransmitted, sizeof(dataTransmitted) );
      radio.powerDown();
      delay(200);
      Serial.println(dataTransmitted);
    }

    if (digitalRead(buttonsetsemi) == LOW) { 
      radio.powerUp();
      timer = 0;
      dataTransmitted = 102;
      radio.write( &dataTransmitted, sizeof(dataTransmitted) );
      radio.powerDown();
      delay(400);
      Serial.println(dataTransmitted);
    }
    if (digitalRead(buttonsetfull) == LOW) {
      radio.powerUp();
      timer = 0;
      dataTransmitted = 103;
      radio.write( &dataTransmitted, sizeof(dataTransmitted) );
      radio.powerDown();
      delay(400);
      Serial.println(dataTransmitted);
    }
    delay(1);
    timer = timer + 1;
  }
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  radio.powerDown();
}

