
const int sensor = 8;
const int led = 9;

void setup() {
  pinMode(sensor, INPUT);
  pinMode(led, OUTPUT);
}

void loop() {
  if(digitalRead(sensor)==HIGH) {
    digitalWrite(led, HIGH);
  }
  else digitalWrite(led, LOW);
  delay(10);
}

