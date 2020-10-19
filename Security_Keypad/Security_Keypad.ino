/* @file CustomKeypad.pde
|| @version 1.0
|| @author Alexander Brevig
|| @contact alexanderbrevig@gmail.com
||
|| @description
|| | Demonstrates changing the keypad size and key values.
|| #
*/
#include <Keypad.h>
#define number 4

const byte ROWS=4; 
const byte COLS=3;
const int beep=10;
const int red_led=11; 
const int green_led=12;
const char code[number]={'1','2','3','4'};
const char exCode[2*number]={'1','2','3','4','5','6','7','8'};
char keyCode[number]={' ',' ',' ',' '};
char exKeyCode[2*number]={' ',' ',' ',' ',' ',' ',' ',' '};
int cnt=0;
int cntb=0;
int tries=0;
int checka=0;
int checkb=0;
int sound=1;
//define the cymbols on the buttons of the keypads
char phone[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(phone), rowPins, colPins, ROWS, COLS); 

void setup(){
  Serial.begin(9600);
  pinMode(red_led,OUTPUT);
  pinMode(green_led,OUTPUT);
}
  
void loop(){
  char customKey = customKeypad.getKey();
  if(customKey) {
    if(customKey=='*') {
      sound++;
      sound%=2;
      keyCode[cnt]=' ';
      cnt--;
    }
    clkbeep(sound);
    keyCode[cnt]=customKey;
    cnt++;
    Serial.println(customKey);
  if(customKey=='#'){
    cnt=0;
    for(int i=0; i<number; i++) {keyCode[i]=' ';}
    resetbeep(sound);
    Serial.println("Clean");
  }
  if(tries<4) {
    if (cnt==number){
      tries++;
      checka=0;
      for(int i=0; i<number; i++) {
        if(keyCode[i]==code[i]) {checka=1;}
        else {
          checka=0;
          break;
        }
      }
      
      if(checka==1) {
        digitalWrite(green_led,HIGH);
        accessbeep(sound);
        delay(110);
        digitalWrite(green_led,LOW);
        Serial.println("Access Guarantee!");
        tries=0;
      }
      else if(checka==0) {
        digitalWrite(red_led,HIGH);
        failbeep(sound);
        delay(900);
        digitalWrite(red_led,LOW);
        Serial.println("Nope!");      
      }
      cnt=0;
      for(int i=0; i<number; i++) {keyCode[i]=' ';} 
    }
    
  }
  if(tries>=4){
    digitalWrite(red_led,HIGH);
    delay(2000);
    digitalWrite(red_led,LOW);
    cntb=0;
    while(1){
      char customKeyb = customKeypad.getKey();
       if(customKeyb) {
         if(customKeyb=='*') {
           sound++;
           sound%=2;
           exKeyCode[cntb]=' ';
           cntb--;
         }
         clkbeep(sound);
         exKeyCode[cntb]=customKeyb;
         cntb++;
         Serial.println(customKeyb);
         if(customKeyb=='#'){
           cntb=0;
           for(int i=0; i<=2*number-1; i++) {exKeyCode[i]=' ';}
             resetbeep(sound);
             Serial.println("Cleaaar");
           }
           if (cntb==2*number){
           checkb=0;
           for(int i=0; i<=2*number-1; i++) {
             if(exKeyCode[i]==exCode[i]) {checkb=1;}
             else {
               checkb=0;
               break;
             }
           }  
           
           if(checkb==1) {
             digitalWrite(green_led,HIGH);
             accessbeep(sound);
             delay(900);
             digitalWrite(green_led,LOW);
             Serial.println("Access Guarantee at last!!!");
             tries=0;
             break;         
           }
           else if(checkb==0) {
             digitalWrite(red_led,HIGH);
             failbeep(sound);
             delay(900);
             digitalWrite(red_led,LOW);
             Serial.println("Noooooooooooo!");      
           }
           cntb=0;
           for(int i=0; i<2*number; i++) {exKeyCode[i]=' ';} 
         }
       } 
    }
  }
}
}

void clkbeep(int k) {
  if(k==1){
   tone(beep, 3000, 100);
  }
}
void accessbeep(int k) {
  if(k==1) {
      tone(beep, 4500, 80);
      delay(120);
      tone(beep, 4500, 80);
      delay(100);
      tone(beep, 4500, 80);
  }
}
void failbeep(int k) {
  if(k==1){
    tone(beep, 300, 900);
  }
}
void resetbeep(int k) {
    if(k==1) {
      tone(beep, 4500, 100);
      delay(300);
      tone(beep, 4500, 100);
      delay(300);
      tone(beep, 4500, 100);
    }
}

