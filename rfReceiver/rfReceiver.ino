#include <SPI.h>
#include "RF24.h"
#define out 9
#define steps  5
int bright;
int before;
RF24 radio(7, 8);
int dataReceived;
const uint64_t pipe = 0x123456789ABC;

void setup() {
  Serial.begin(115200);
  before = 0;
  bright = 255;
  pinMode(out, OUTPUT);
  TCCR1B = (TCCR1B & 0b11111000) | 0x02;
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setChannel(110);
  radio.openReadingPipe(1, pipe);
  radio.startListening();
}

void loop() {
  if ( radio.available())
  {
    while (radio.available())
    {
      radio.read( &dataReceived, sizeof(dataReceived) );
    }
    radio.stopListening();
    Serial.print("Data received = ");
    Serial.println(dataReceived);
    if (dataReceived == 100) {
      if (before == 0) {
        digitalWrite(out, HIGH);
        before = 1;
      }
      else {
        digitalWrite(out, LOW);
        before = 0;
        bright = 255;
      }
    }
    if (dataReceived == 102 && before == 1) {
      if (bright - 255 / steps < 0) {
        analogWrite(out, bright);
      }
      else {
        bright = bright - 255 / steps;
        analogWrite(out, bright);
      }
    }
    if (dataReceived == 101 && before == 1) {
      if (bright + 255 / steps > 255) {
        analogWrite(out, bright);
      }
      else {
        bright = bright + 255 / steps;
        analogWrite(out, bright);
      }
    }
    radio.startListening();
  }
}


