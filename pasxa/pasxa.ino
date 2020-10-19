const int led1=2;
const int led2=5;
const int led3=4;
const int button=7;

void setup() {
  pinMode(button,INPUT);
  pinMode(led1,OUTPUT);
  pinMode(led2,OUTPUT);
  pinMode(led3,OUTPUT);
}

void loop() {
  if(digitalRead(button)==HIGH) {
    
    
    digitalWrite(led1,HIGH);
    delay(2000);
    digitalWrite(led1,LOW);
    delay(100);
    digitalWrite(led2,HIGH);
    delay(2000);
    digitalWrite(led2,LOW);
    delay(100);
    digitalWrite(led3,HIGH);
    delay(2000);
    digitalWrite(led3,LOW);   
  }

}
