#include <Key.h>
#include <Keypad.h>

const int first=5;
const int buzzer=3;

void setup() {
pinMode(first,INPUT_PULLUP);
pinMode(buzzer,OUTPUT);
}

void loop() {
if(digitalRead(first)==LOW) {
  digitalWrite(buzzer,HIGH);
  delay(500);
  digitalWrite(buzzer,LOW);
  
}

}
