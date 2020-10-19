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
#include <Adafruit_GFX.h>
#include <gfxfont.h>
#include <epd.h>
#include <Fonts/FreeSans24pt7b.h>
#include "sail_data.h"
#include <avr/wdt.h>

HardwareSerial *nmeaPort;
int intens_idx=0;
int disp_idx=0;
const int wake_up = 15;
const int reset = 14;
const int pwm_pin = 13;
const int light_pin = A0;
const int disp_pin = A1;
SailData boat;
Adafruit_epd display(wake_up, reset, &Serial2);
int i=0;

void screen_setup() {
  pinMode(pwm_pin, OUTPUT);
  analogWrite(pwm_pin, intens[intens_idx]);
  display.wakeup();
  display.set_memory(MEM_NAND);
  display.set_color(BLACK,WHITE);
  display.setFont(&FreeSans24pt7b);
}

void screen_test(){
  display.clear();
  display.setCursor(0,550);
  display.setTextSize(9);
  display.print("HI!");
  display.udpate();
  delay(1000);
}

void show(float number) {
  display.clear();
  display.setTextSize(4);
  display.setTextColor(BLACK);
  display.setCursor(20,150);
  display.print(name[disp_idx]);
  display.setCursor(0,550);
  display.setTextSize(9);
  display.print(number);
  display.udpate();
}

void buttons_setup() {
  pinMode(light_pin, INPUT_PULLUP);
  pinMode(disp_pin, INPUT_PULLUP);
}

void bright_change(){
  intens_idx++;
  intens_idx = intens_idx%5;
  analogWrite(pwm_pin, intens[intens_idx]);
  Serial.print("Brightness ");
  Serial.print(intens_idx);
  Serial.print(' ');
  Serial.println(intens[intens_idx]);
}

void display_change(){ 
  disp_idx++;
  disp_idx = disp_idx%len_t;
  show(boat.data[disp_idx]);
//  alpha_disp(disp_idx);
  Serial.print("Display ");
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
  Serial.begin(9600);
  Serial.println("boot");
  nmeaPort = &Serial1;
  nmeaPort->begin(4800);
  wdt_enable(WDTO_8S);
  screen_setup();
  screen_test();
  wdt_reset();
  delay(500);
  wdt_reset();
  delay(500);
  wdt_reset();
  buttons_setup();
}

void loop() {
  wdt_reset();
  
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
  while(nmeaPort->available() > 0) parser.parse(nmeaPort->read());
  if(mils%1000==0){
    boat.print_all(&Serial);
    show(boat.data[disp_idx]);
  }
  mils++;
  delay(1);
}



