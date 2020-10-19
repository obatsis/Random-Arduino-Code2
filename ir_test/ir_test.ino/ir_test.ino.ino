#include <IRremote.h>

IRrecv irrecv(0);
decode_results results;


void setup() {
 Serial.begin(9600);
}

void loop() {
 if(irrecv.decode(&results)) {
  Serial.println(results.value, HEX);
 }
 delay(100);
irrecv.resume();
}
