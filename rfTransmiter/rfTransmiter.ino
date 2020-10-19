#include <SPI.h>
#include "RF24.h"
#include "LowPower.h"
#define button1 4 //Power
#define button2 5 //Brightness+
#define button3 6 //Brightness-
#define interruptpin 2
RF24 radio(7, 8);
int timer;
int dataTransmitted;
const uint64_t pipe = 0x123456789ABC;

void wakeUp() {
  timer = 0;
}

void setup() {
  Serial.begin(115200);
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(interruptpin, INPUT_PULLUP);
  timer = 0;
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setChannel(110);
  radio.openWritingPipe(pipe);
}


void loop() {
  attachInterrupt(digitalPinToInterrupt(interruptpin), wakeUp, LOW);
  while (timer < 3000) {
    if (digitalRead(button1) == LOW) { //POWER
      radio.powerUp();
      timer = 0;
      dataTransmitted = 100;
      radio.write( &dataTransmitted, sizeof(dataTransmitted) );
      radio.powerDown();
      delay(400);
      Serial.println(dataTransmitted);
    }

    if (digitalRead(button2) == LOW) { //BRIGHTNESS+
      radio.powerUp();
      timer = 0;
      dataTransmitted = 101;
      radio.write( &dataTransmitted, sizeof(dataTransmitted) );
      radio.powerDown();
      delay(400);
      Serial.println(dataTransmitted);
    }

    if (digitalRead(button3) == LOW) { //BRIGHTNESS-
      radio.powerUp();
      timer = 0;
      dataTransmitted = 102;
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

