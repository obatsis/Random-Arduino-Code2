//Prom racing
//Pneumatic system controlled from arduino nano
//shifter 2018

#include <Servo.h>

#define debounceDelay 50 
#define total_gears 5
#define shift_down 3
#define shift_up 4
#define port1 5
#define port2 6
#define clutch_pin 9
#define sparkcut  13 //Gearcut pin at ECU
#define pot_clutch_MIN 205   
#define pot_clutch_MAX 390
#define pot_error 15
#define shift_t 100 //milliseconds to keep valve ON on a full shift
#define hshift_t 30 ////milliseconds to keep valve ON on a half shift(for NEUTRAL)

unsigned long up_DebounceTime=0, down_DebounceTime=0;
int current_gear=0, target_gear=0;
uint16_t pot_pos, clutch_pos, prev_pot_pos=0;
uint8_t shift_up_unpressed, shift_down_unpressed;
unsigned long int prev_time;
uint8_t gearbox[total_gears+1] = {0,207,10,10,10,10};

Servo clutch;

void setup() {
  Serial.begin(9600);
  pinMode(shift_up,INPUT_PULLUP);
  pinMode(shift_down,INPUT_PULLUP);
  pinMode(port1,OUTPUT);          //moves lever up
  pinMode(port2, OUTPUT);        //moves lever down
  pinMode(sparkcut, OUTPUT);
  
  clutch.attach(clutch_pin, 1000, 1600);  //1000->1600ms = 0->60 degrees
  clutch.writeMicroseconds(1000); //initialize servo's position
  Serial.println("----Racing KTM----");
  Serial.println("--------------------");
  current_gear = analogRead(A1);
  for(int i; i<6; i++)  {
      if((current_gear >= (gearbox[i] - pot_error)) or (current_gear <= (gearbox[i] + pot_error))) {
        current_gear = i;
        target_gear=i;
      }
  }
  Serial.print("Gear: ");
  Serial.println(current_gear);
  delay(1000);
}

void loop() {
  pot_pos = analogRead(A0);
  //check if the clutch is pressed
  //Serial.println(pot_pos);
  if(pot_pos<(pot_clutch_MIN-pot_error)) {   //the clutch is engaged
     clutch.writeMicroseconds(1800);
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
              clutch.writeMicroseconds(1400);
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
            clutch.writeMicroseconds(1400);
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
  }else if(pot_pos >pot_clutch_MIN and pot_pos<pot_clutch_MAX)  { //the clutch is disengaged

    //move servo
    Serial.println(pot_pos);
    clutch_pos = map(pot_pos, pot_clutch_MIN, pot_clutch_MAX, 1800, 1000);
    clutch.writeMicroseconds(clutch_pos);
    
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
            /*============================NEUTRAL=================================*/
            /*====================================================================*/
            prev_time = millis();
            while(analogRead(A1)<gearbox[target_gear] and millis() - prev_time<=1000)  {
              digitalWrite(port1, HIGH);
               delay(5);
            }        
            digitalWrite(port1, LOW);
            /*====================================================================*/
            /*====================================================================*/
         }
         else{target_gear=0;}
         down_DebounceTime = millis();
         shift_down_unpressed=false;
       }
     }
   }
  }
 }







