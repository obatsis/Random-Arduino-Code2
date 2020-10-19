int ledPin=10;
int potPin=0;
int potValue;
void setup() {
  pinMode(ledPin,OUTPUT);
  pinMode(potValue,INPUT);
  pinMode(LED_BUILTIN,OUTPUT);
}

void loop() {

 potValue=analogRead(potPin);
 potValue=map(potValue,313,1023,0,255);
 analogWrite(ledPin,potValue);

}
