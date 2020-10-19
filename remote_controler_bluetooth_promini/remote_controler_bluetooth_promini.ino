#include <SoftwareSerial.h>
SoftwareSerial BTserial(8, 9); // RX | TX 
  
const int buttonscrollf=4;
const int buttonscrollb=3;
const int buttonselectsemi=5;
const int buttonselectfull=6;

void setup(){
   BTserial.begin(9600);  
   pinMode(buttonscrollf,INPUT_PULLUP);
   pinMode(buttonscrollb,INPUT_PULLUP);
   pinMode(buttonselectsemi,INPUT_PULLUP);
   pinMode(buttonselectfull,INPUT_PULLUP);
   pinMode(7,OUTPUT);
   digitalWrite(7,HIGH);
}

void loop(){
   if(digitalRead(buttonscrollf)==LOW){
     BTserial.write("scrollf");
     digitalWrite(buttonscrollf,HIGH);
     delay(400);
   }
   if(digitalRead(buttonscrollb)==LOW){
    BTserial.write("scrollb"); 
    digitalWrite(buttonscrollb,HIGH); 
    delay(400);
   }
   if(digitalRead(buttonselectsemi)==LOW){
     BTserial.write("selectsemi");
     digitalWrite(buttonselectsemi,HIGH);
     delay(400); 
   }
   if(digitalRead(buttonselectfull)==LOW){
     BTserial.write("selectfull");
     digitalWrite(buttonselectfull,HIGH);
     delay(400);
   }
   delay(1);
}
