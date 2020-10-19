const int sw=5;
const int main=10;
int mainState=0;
int swState=0;
int led2=2;
int led3=3;
int siren=12;

void setup() {
 pinMode(led2,OUTPUT);
 pinMode(led3,OUTPUT);
 pinMode(sw,INPUT);
 pinMode(main,INPUT);
 pinMode(siren,OUTPUT);

}

void loop() {
mainState= digitalRead(main);
 swState =digitalRead(sw);
 
 if (mainState==0) {
   digitalWrite(LED_BUILTIN,LOW);
 
  // swState=digitalRead(sw);
    if(swState==1)  {
    digitalWrite(led2,HIGH);
    digitalWrite(led3,LOW);
    digitalWrite(siren,HIGH);
    delay(20);

    digitalWrite(led2,LOW);
    digitalWrite(led3,HIGH);
    digitalWrite(siren,LOW);
    delay(20);
    }
    swState=digitalRead(sw);
  if(swState==0) {
    digitalWrite(led2,HIGH);
    digitalWrite(led3,LOW);
    delay(1000);
   
    digitalWrite(led2,LOW);
    digitalWrite(led3,HIGH);
    delay(700);
    swState=digitalRead(sw);
   }
 }
 else {
digitalWrite(LED_BUILTIN,HIGH);
  digitalWrite(led2,LOW);
  digitalWrite(led3,LOW);
 
} 


}

