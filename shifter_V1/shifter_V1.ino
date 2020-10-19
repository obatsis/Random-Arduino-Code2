#include <Servo.h>
#define debounceDelay 50 
#define total_gears 5
#define shift_down 3
#define shift_up 4
#define port1 5
#define port2 6
#define clutch_pin 9
#define sparkcut  13 //Gearcut pin at ECU
#define pot_clutch_MIN 939
#define pot_clutch_MAX 1023
#define pot_error 10
#define shift_t 150 //milliseconds to keep valve ON on a full shift
#define hshift_t 15 ////milliseconds to keep valve ON on a half shift(for NEUTRAL)

unsigned long up_DebounceTime=0, down_DebounceTime=0;
int current_gear=0, target_gear=0, gear_pos;
uint16_t pot_pos, clutch_pos, prev_pot_pos=0;
uint8_t shift_up_unpressed, shift_down_unpressed;

Servo clutch;

void setup() {
  Serial.begin(9600);
  pinMode(shift_up,INPUT_PULLUP);
  pinMode(shift_down,INPUT_PULLUP);
  pinMode(port1,OUTPUT);          //moves lever up
  pinMode(port2, OUTPUT);        //moves lever down
  pinMode(sparkcut, OUTPUT);
  
  clutch.attach(clutch_pin, 1000, 1900);  //1000->1600ms = 0->60 degrees
  clutch.writeMicroseconds(1900); //initialize servo's position
  Serial.println("----Racing KTM----");
  Serial.println("--------------------");
}

void loop() {
  pot_pos = analogRead(A0);
  gear_pos=analogRead(A1);
  
  //check if the clutch is pressed
  //Serial.print("Gear pos: ");
  //Serial.println(gear_pos);
 if(Serial.available()) {
    int c_gear = Serial.parseInt(); 
    current_gear = c_gear;
    target_gear = current_gear;
 }
 Serial.println(current_gear);
 //Serial.println(clutch.readMicroseconds());
  if(pot_pos<(pot_clutch_MIN+pot_error)) {   //the clutch is not disengaged
    clutch.writeMicroseconds(1900);
   // Serial.println("mpainei");
     if ((millis() - up_DebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
    
      if(digitalRead(shift_up)==LOW) shift_up_unpressed=true;   //up shift
      else{
        delay(debounceDelay);
        if(shift_up_unpressed==true and digitalRead(shift_up)==HIGH){      
           target_gear++;
          if(target_gear <= total_gears && target_gear>1)  {    //check if we passed the total number of gears
              current_gear=target_gear;
              Serial.print(current_gear);  
              Serial.println(" Gear | Up Shift");
              Serial.println("--------------------");  
              clutch.writeMicroseconds(1500);
              delay(94);
              digitalWrite(port1, HIGH);
              delay(shift_t);   
              clutch.writeMicroseconds(1800);
              digitalWrite(port1, LOW);   
          }else if(target_gear == 1) target_gear=0;
          else target_gear=5;
            up_DebounceTime = millis();
            shift_up_unpressed=false; 
        }
      }
   }
  
  if ((millis() - down_DebounceTime) > debounceDelay) {
      // whatever the reading is at, it's been there for longer
      // than the debounce delay, so take it as the actual current state:
    
     if(digitalRead(shift_down)==LOW) shift_down_unpressed=true;
     else{
        delay(debounceDelay);
        if(shift_down_unpressed==true and digitalRead(shift_down)==HIGH){        
          target_gear--;
        if(target_gear>=1) {    
            current_gear=target_gear;
            Serial.print(current_gear);  
            Serial.println(" Gear | Down Shift");      
            Serial.println("--------------------");
            clutch.writeMicroseconds(1500);
            delay(94);
            digitalWrite(port2, HIGH);
            delay(shift_t);
            clutch.writeMicroseconds(1800);
            digitalWrite(port2, LOW); 
           // delay(100);       
         } else if(target_gear < 0) target_gear = 0;
         else target_gear=1;
         down_DebounceTime = millis();
          shift_down_unpressed=false; 
        }
      }
    }
  }else if(pot_pos >(pot_clutch_MIN+pot_error) and pot_pos<=pot_clutch_MAX)  { //the clutch is disengaged
    
    //move servo
    //Serial.println(pot_pos);
    clutch_pos = map(pot_pos, pot_clutch_MIN, pot_clutch_MAX, 1800, 1000);
    //if(prev_pot_pos-pot_pos>30 || pot_pos - prev_pot_pos>30)  {
      //Serial.println(clutch_pos);
      clutch.writeMicroseconds(clutch_pos);
      //prev_pot_pos = pot_pos;
    //}
    
    //delay(15);
    
    if ((millis() - up_DebounceTime) > debounceDelay) {
      // whatever the reading is at, it's been there for longer
      // than the debounce delay, so take it as the actual current state:
     
        if(digitalRead(shift_up)==LOW) shift_up_unpressed=true; 
          else{
            delay(debounceDelay);
            if(shift_up_unpressed==true and digitalRead(shift_up)==HIGH){      
              target_gear++;
              if(target_gear <= total_gears && target_gear > 1)  {
                 current_gear=target_gear;
                 Serial.print(current_gear);  
                 Serial.print(" Gear | Up Shift | Clutch: ");  
                 Serial.println(clutch_pos);
                 Serial.println("--------------------");
                 digitalWrite(port1, HIGH);
                 delay(shift_t);
                 digitalWrite(port1, LOW);
               }else if (target_gear == 1) {
                current_gear=target_gear;
                 Serial.print(current_gear);  
                 Serial.print(" Gear | Up Shift | Clutch: ");  
                 Serial.println(clutch_pos);
                 Serial.println("--------------------");
                 digitalWrite(port2, HIGH);
                 delay(shift_t);
                 digitalWrite(port2, LOW);
               }
               else{target_gear=5;}
               up_DebounceTime = millis();
               shift_up_unpressed=false; 
             }
          }
      }
        
    if ((millis() - down_DebounceTime) > debounceDelay) {
      // whatever the reading is at, it's been there for longer
      // than the debounce delay, so take it as the actual current state:
    
     if(digitalRead(shift_down)==LOW) shift_down_unpressed=true;
      else{
        delay(debounceDelay);
        if(shift_down_unpressed==true and digitalRead(shift_down)==HIGH){        
          target_gear--;
          if(target_gear>=1) {
            current_gear=target_gear;
            Serial.print(current_gear);   
            Serial.print(" Gear | Down Shift | Clutch: ");     
            Serial.println(clutch_pos);
            Serial.println("--------------------");
            digitalWrite(port2, HIGH);
            delay(shift_t);
            digitalWrite(port2, LOW);   
         }
         else if(target_gear==0){      
            current_gear=target_gear;      
            Serial.print(current_gear);   
            Serial.print(" Gear | Down Shift | Clutch: ");     
            Serial.println(clutch_pos);
            Serial.println("--------------------");
            digitalWrite(port1, HIGH);
            delay(hshift_t);
            digitalWrite(port1, LOW);   
         }
         else{target_gear=0;}
         down_DebounceTime = millis();
         shift_down_unpressed=false;
       }
     }
   }
  }
 }







