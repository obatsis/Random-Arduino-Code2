/*
IBT-2 Motor Control Board driven by Arduino.
 
Speed and direction controlled by a potentiometer attached to analog input 0.
One side pin of the potentiometer (either one) to ground; the other side pin to +5V
 
Connection to the IBT-2 board:
IBT-2 pin 1 (RPWM) to Arduino pin 5(PWM)
IBT-2 pin 2 (LPWM) to Arduino pin 6(PWM)
IBT-2 pins 3 (R_EN), 4 (L_EN), 7 (VCC) to Arduino 5V pin
IBT-2 pin 8 (GND) to Arduino GND
IBT-2 pins 5 (R_IS) and 6 (L_IS) not connected
*/
const int digitCombPer1 = 7; /* times of channel combination "11" per 1%*/

// motor control variables
const int m1 = 25;
const int m2 = 26;

// encoder's channels 
const int chA = 2;
const int chB = 8;
int RPWM_Output = 5; // Arduino PWM output pin 5; connect to IBT-2 pin 1 (RPWM)
int LPWM_Output = 6; // Arduino PWM output pin 6; connect to IBT-2 pin 2 (LPWM)
 
void setup()
{
  pinMode(RPWM_Output, OUTPUT);
  pinMode(LPWM_Output, OUTPUT);
  pinMode(m1, OUTPUT);
  pinMode(m2, OUTPUT);
}
 
void loop()
{
  int sensorValue = analogRead(SENSOR_PIN);
 
  // sensor value is in the range 0 to 1023
  // the lower half of it we use for reverse rotation; the upper half for forward rotation
  if (sensorValue < 512)
  {
    // reverse rotation
    int reversePWM = -(sensorValue - 511) / 2;
    analogWrite(LPWM_Output, 0);
    analogWrite(RPWM_Output, reversePWM);
  }
  else
  {
    // forward rotation
    int forwardPWM = (sensorValue - 512) / 2;
    analogWrite(LPWM_Output, forwardPWM);
    analogWrite(RPWM_Output, 0);
  }
}




 


  
  int rs; // rotary switch value
  int value = 0; // ------memory function
  
  while(1)
  {
    rs = digitCombPer1;
    if(rs < value)  
    {
      digitalWrite(m1, HIGH); // COUNTER CLOCKWISE MOTOR OPERATION
      digitalWrite(m2, LOW);
      // "11" combination identification
      while(1) 
      {
        if((chA == 1) && (chB == 1))
        {
          value++;
          break;
        }
      }
      do
      {}while( !( (chA == 1) && (chB == 1) ) )
    }
    else if(rs > value)
    {
      digitalWrite(m1, LOW); // CLOCKWISE MOTOR OPERATION
      digitalWrite(m2, HIGH);
      // "11" combination identification
      while(1) 
      {
        if((chA == 1) && (chB == 1))
        {
          value--;
          break;
        }
      }
      do
      {}while( !( (chA == 1) && (chB == 1) ) )
    }
    else
    {
      digitalWrite(m1, LOW); // MOTOR IS STALLED
      digitalWrite(m2, LOW);
    }
  }
} 

