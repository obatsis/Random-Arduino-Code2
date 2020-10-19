#include <SoftwareSerial.h>
const int buttonscrollf=3;
const int buttonscrollb=2;  
const int buttonselect=4;
const int displaybutton=5;
int spot[10];

const int one=22;
const int two=23;
const int three=24;
const int four=25;
const int five=26;
const int six=27;
const int seven=28;
const int eight=29;
const int nine=30;
const int ten=31;
int j=22;
int cnt=0;
int state; //pinakas me deikti 0/1 kalitera


char tempString[10];
char brightString[10];
byte bright_idx=4;
const uint8_t  bright[5]={0,25,50,75,100};

SoftwareSerial s7s(7,8); //RX,TX 

void setup() {
  Serial.begin(9600);
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
  pinMode(buttonselect,INPUT_PULLUP);
  pinMode(displaybutton,INPUT_PULLUP);

  pinMode(one,OUTPUT);
  pinMode(two,OUTPUT);
  pinMode(three,OUTPUT);
  pinMode(four,OUTPUT);
  pinMode(five,OUTPUT);
  pinMode(six,OUTPUT);
  pinMode(seven,OUTPUT);
  pinMode(eight,OUTPUT);
  pinMode(nine,OUTPUT);
  pinMode(ten,OUTPUT);
  
  for(int i=0; i<10; i++){  
    spot[i]=j;
    j++;
  }
}

void brightChange() {
  bright_idx++;
  bright_idx=bright_idx%5;
  s7s.write(0x7A);
  s7s.write((byte) bright[bright_idx]);
  sprintf(brightString,"b%03d",bright[bright_idx]);
  s7s.print(brightString);
  digitalWrite(displaybutton,HIGH);
  delay(500);
}
void loop() {
  sprintf(tempString,"%4d",cnt+1);
  s7s.print(tempString);  
  if(digitalRead(buttonscrollf)==LOW){
    cnt++;
    if(cnt==10){  
      cnt=0;   
    }
    sprintf(tempString,"%4d",cnt+1);
    s7s.print(tempString);  
    Serial.println(cnt+1);
    digitalWrite(buttonscrollf,HIGH);
    delay(600); //600
  }
  
  if(digitalRead(buttonscrollb)==LOW) {
    cnt--;
    if(cnt==-1){ 
      cnt=9;
    }
    sprintf(tempString,"%4d",cnt+1);
    s7s.print(tempString);
    Serial.println(cnt+1);
    digitalWrite(buttonscrollb,HIGH);
    delay(600);
  }
  
  if(digitalRead(buttonselect)==LOW){
    digitalWrite(spot[cnt],HIGH);
    //s7s.write(0x77); //vlakeia
    //s7s.write(0b111111);
    Serial.print(spot[cnt]);
    Serial.println(", on");
    delay(800); //800
    digitalWrite(spot[cnt],LOW);
    s7s.print(tempString);
    Serial.print(spot[cnt]);
    Serial.println(", off");
    digitalWrite(buttonselect,HIGH);
    delay(500);
  }
  
  if(digitalRead(displaybutton)==LOW){
    brightChange();
  }
  delay(10);
}
