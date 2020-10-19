char ch1=0x00;//s1=0 s2=0 p1=0 p2=0 c3..=0x04 
char ch2=0x08;//s1=0 s2=1 p1=0 p2=0 c3..=0x04 
char ch3=0x10;//s1=0 s2=1 p1=0 p2=0 c3..=0x04 
char ch4=0x18;//s1=0 s2=1 p1=0 p2=0 c3..=0x04 

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  
Serial.write(ch1+4);//απλά προσθετουμε την τιμη που θελουμε
delay(1);
Serial.write(ch2+3);
delay(1);
Serial.write(ch3+2);
delay(1);
Serial.write(ch4+1);
}
