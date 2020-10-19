/*
Simple NMEA parsing example for Arduino.  

Handles GPGGA messages by printing out latitude and longitude (degrees:minutes) and altitude (meters).

To run this code, copy the NMEA folder into your Arduino/libraries folder.

Copyright (C) Simon D. Levy 2015

This code is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as 
published by the Free Software Foundation, either version 3 of the 
License, or (at your option) any later version.
This code is distributed in the hope that it will be useful,     
but WITHOUT ANY WARRANTY without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU Lesser General Public License 
along with this code.  If not, see <http:#www.gnu.org/licenses/>.
*/

#include <NMEA.h>
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#include "sail_data.h"
#include <avr/wdt.h>

int intens_idx=0;
int disp_idx=0;
const int pwm_pin = 13;
const int light_pin = 6;
const int disp_pin = 7;
const int left_led = 51;
const int right_led = 53;
SailData boat;
Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();
// Adafruit_LEDBackpack::setBrightness(uint8_t b)

void screen_test(){
  PORTF = 0b10001000;
  PORTK = 0b00111000;
}

void show(float number) {
  digitalWrite(left_led,LOW);
  digitalWrite(right_led,LOW);
  int val;
  int dot;
  if(number<1000) {
    if(number<10) {
      dot = 0b10;
      val = (int)(number*100);
    }
    else if(number<100) {
      dot = 0b01;
      val = (int)(number*10);
    }
    else{
      dot = 0b00;
      val = (int)number;
    }
    byte a = val%10;
    byte b = (val/10)%10;
    byte c = (val/100)%10;
//    if (c==0){
//      c=0x0F;
//      if(b==0) b=0x0F;
//    }
    
    PORTF = b<<4 | a;
    PORTK = dot<<4 | c;
  }
  else {
    
  }
}

void show_apparent_trueWindAngle(float number) {
 
  int val;
  int dot;
   
  if(number < 0){
    number = number*(-1); 
    digitalWrite(right_led,LOW);
    digitalWrite(left_led,HIGH);
  }
  else {
    digitalWrite(right_led,HIGH);
    digitalWrite(left_led,LOW);
  }
  val=(int)number;
 
  byte a=val%10;
  byte b=(val/10)%10;
  byte c=(val/100)%10;

  if(val<100) c=0x0F;
  if(val<10) {
    c=0x0F;
    b=0x0F;
  }
  dot=0b00;
  PORTF = b<<4 | a;
  PORTK = dot<<4 | c;
 // digitalWrite(left_led,LOW);
 // digitalWrite(right_led,LOW);
}

void show_angle(float number) {
  digitalWrite(left_led,LOW);
  digitalWrite(right_led,LOW);
   
  int val;
  int dot;

  val=(int)number;

  byte a=val%10;
  byte b=(val/10)%10;
  byte c=(val/100)%10;

  if(val<100) c=0x0F;
  if(val<10) {
    c=0x0F;
    b=0x0F;
  }
  dot=0b00;
  PORTF = b<<4 | a;
  PORTK = dot<<4 | c;

}



void screen_setup() {
  DDRF|=0xFF;
  DDRK|=0xFF;
}

void buttons_setup() {
  pinMode(light_pin, INPUT_PULLUP);
  pinMode(disp_pin, INPUT_PULLUP);
}

void bright_change(){
  intens_idx++;
  intens_idx = intens_idx%5;
  analogWrite(pwm_pin, intens[intens_idx]);
  alpha4.setBrightness(text_intens[intens_idx]);
  Serial.print(intens_idx);
  Serial.print(' ');
  Serial.println(intens[intens_idx]);
}

void alpha_disp(int idx) {
  alpha4.clear();
  alpha4.writeDigitAscii(0, name[idx][0]);
  alpha4.writeDigitAscii(1, name[idx][1]);
  alpha4.writeDigitAscii(2, name[idx][2]);
  alpha4.writeDigitAscii(3, ' ');
  alpha4.writeDisplay();
}

void display_change(){ 
  disp_idx++;
  disp_idx = disp_idx%len_t;
  show(boat.data[disp_idx]);
  alpha_disp(disp_idx);
  Serial.print(disp_idx);
  Serial.print(' ');
  Serial.println(name[disp_idx]);
}

class MyParser : public NMEA_Parser {

  void handleMWD(MWD_Message & mwd) 
  {
    boat.data[twd_t] = (float)mwd.windDirection;
  }
 
  void handleVWR(VWR_Message & vwr) 
  {
    float sign = (vwr.direction[0]=='R') ? 1.0 : -1.0;
    boat.data[awa_t] = sign * vwr.windAngleAbs;
    boat.data[aws_t] = vwr.windSpeedKnots;
  }

  void handleVWT(VWT_Message & vwt) 
  {
    float sign = (vwt.direction[0]=='R') ? 1.0 : -1.0;
    boat.data[twa_t] = sign * vwt.windAngleAbs;
    boat.data[tws_t] = vwt.windSpeedKnots;
  }
  
  void handleMTW(MTW_Message & mtw) 
  {
    boat.data[mtw_t] = mtw.water_temp;
  }

  void handleMTA(MTA_Message & mta) 
  {
    boat.data[mta_t] = mta.air_temp;
  }

  void handleDBT(DBT_Message & dbt) 
  {
    boat.data[dbt_t] = dbt.depth_below_tranducer_meters;
  } 

  void handleDBS(DBS_Message & dbs) 
  {
    boat.data[dbs_t] = dbs.depth_below_surface_meters;
  } 

};

MyParser parser;
long unsigned mils=0;

void setup() {
  alpha4.begin(0x70);
  Serial.begin(9600);
  Serial1.begin(4800);
  wdt_enable(WDTO_2S);
  alpha_disp(disp_idx);
  screen_setup();
  screen_test();
  wdt_reset();
  delay(500);
  wdt_reset();
  delay(500);
  wdt_reset();
  buttons_setup();
  pinMode(pwm_pin, OUTPUT);
  pinMode(left_led,OUTPUT);
  pinMode(right_led,OUTPUT);
  analogWrite(pwm_pin, intens[intens_idx]);
  alpha4.setBrightness(text_intens[intens_idx]);
  Serial.println("Hello");
  Serial1.println("hello");
}

void loop() {
  wdt_reset();
// digitalWrite(left_led,HIGH);
//  digitalWrite(right_led,HIGH);
  if (!digitalRead(light_pin)){
    while(!digitalRead(light_pin));
    bright_change();
    delay(150);
  }
  if (!digitalRead(disp_pin)){
    while(!digitalRead(disp_pin));
    display_change();
    delay(150);
  }
  if (Serial1.available() > 0) parser.parse(Serial1.read());
  if(mils%1000==0){
    boat.print_all(&Serial);
    if(disp_idx==twa_t || disp_idx==awa_t) {
     show_apparent_trueWindAngle(boat.data[disp_idx]);
    }
    else if(disp_idx==twd_t) {
      show_angle(boat.data[disp_idx]);
    }
    else show(boat.data[disp_idx]);
  }
  mils++;
  delay(1);
}



