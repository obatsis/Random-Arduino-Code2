#include <SoftwareSerial.h>
#include <SPI.h>
#include "RF24.h"

SoftwareSerial s7s(9,10); //RX,TX 
RF24 radio(7, 8);
int dataReceived;
const uint64_t pipe = 0x123456789ABC;

const int buttonscrollf=3;
const int buttonscrollb=2;  
const int buttonselectsemi=4;
const int buttonselectfull=5;
const int displaybutton=6;
                       
int spot[20];

int j=22;
int cnt=0;
int state[20];

char tempString[10];
char brightString[10]; 
byte bright_idx=4;
const uint8_t  bright[5]={0,25,50,75,100};


void setup(){
  Serial.begin(115200);
  s7s.begin(9600);
  s7s.write(0x7A);
  s7s.write((byte) 100);
  sprintf(brightString,"b%03d",100);
  s7s.print(brightString);
  sprintf(tempString,"%4d",cnt+1);
  s7s.print(tempString);
  s7s.write(0x76);  
  
  pinMode(buttonscrollf,INPUT_PULLUP);
  pinMode(buttonscrollb,INPUT_PULLUP);
  pinMode(buttonselectsemi,INPUT_PULLUP);
  pinMode(buttonselectfull,INPUT_PULLUP);
  pinMode(displaybutton,INPUT_PULLUP);


  TCCR1B = (TCCR1B & 0b11111000) | 0x02;
  radio.begin();
  delay(100);
  radio.setChannel(115); 
  radio.setPALevel(RF24_PA_MAX);
  radio.openReadingPipe(1, pipe); 
  radio.startListening();
  
  for(int i=0; i<20; i++){  
    spot[i]=j;
    pinMode(spot[i],OUTPUT);
    state[i]=0;
    j++;
  }
}

void brightChange(){
  bright_idx++;
  bright_idx=bright_idx%5;
  s7s.write(0x7A);
  s7s.write((byte) bright[bright_idx]);
  sprintf(brightString,"b%03d",bright[bright_idx]);
  s7s.print(brightString);
  digitalWrite(displaybutton,HIGH);
  delay(500);
  s7s.print(tempString);
  if(state[cnt]==1){
    blinkstate();
  }
}

void scrollf(){
  cnt++;
  if(cnt==20){  
    cnt=0;   
  }
  sprintf(tempString,"%4d",cnt+1);
  s7s.print(tempString);
  if(state[cnt]==1){
    blinkstate();
  }
  delay(300); 
}

void scrollb(){
  cnt--;
  if(cnt==-1){
    cnt=19;
  }
  sprintf(tempString,"%4d",cnt+1);
  s7s.print(tempString);
  if(state[cnt]==1){
    blinkstate();
  }
  delay(300);
}

void selectsemi(int k){
  digitalWrite(spot[cnt],HIGH);
  blinkstate();
  delay(k);
  digitalWrite(spot[cnt],LOW);
  s7s.write(0x76);
  s7s.print(tempString);
  delay(300);
}
void selectfull(){  
  if(state[cnt]==0){
    digitalWrite(spot[cnt],HIGH);
    blinkstate();
    state[cnt]=1;
  }
  else if(state[cnt]==1){
    digitalWrite(spot[cnt],LOW); 
    s7s.print(tempString);
   state[cnt]=0;
  }
 
  delay(300);
}
void blinkstate(){
  s7s.write(0x7B);
  s7s.write(0b01001001);
}
void checkData(int data){  
  if(data==100){
    scrollf();
  }
  else if(data==101){    
    scrollb();
  }
  else if(data==102){ 
    if(cnt>=0 && cnt<=9){
      selectsemi(500);
    }
    else if(cnt>=10 && cnt<=19){
      selectfull(); 
    }
  }
  else if(data==103){
    if(cnt>=0 && cnt<=9){
      selectsemi(2000);
    }
    else if(cnt>=10 && cnt<=19){
      selectfull(); 
    }
  }
} 

void loop() {
  if(state[cnt]==0){ 
    sprintf(tempString,"%4d",cnt+1);
    s7s.print(tempString); 
  }
  else if(state[cnt]==1){
   blinkstate();
  }
  if ( radio.available()){
    while (radio.available()){
      radio.read( &dataReceived, sizeof(dataReceived) );
    }
    radio.stopListening();
  
    Serial.println(dataReceived);
    checkData(dataReceived);
    radio.startListening();
  }
    if(digitalRead(buttonscrollf)==LOW){
    scrollf();
    }
    if(digitalRead(buttonscrollb)==LOW){
      scrollb();
    }
    if(digitalRead(buttonselectsemi)==LOW){
      if(cnt>=0 && cnt<=9){
        selectsemi(500);
      }
      else if(cnt>=10 && cnt<=19){
        selectfull(); 
      }
    }
    if(digitalRead(buttonselectfull)==LOW){
      if(cnt>=0 && cnt<=9){
        selectsemi(2000);
      }
      else if(cnt>=10 && cnt<=19){
        selectfull(); 
      }
    }
    if(digitalRead(displaybutton)==LOW){
      brightChange();
    }
}
