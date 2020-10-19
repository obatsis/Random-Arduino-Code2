 #include <SoftwareSerial.h>

SoftwareSerial Serial7Segment(7, 8);//RX,TX
const int sense=0;
const int buttonset=11;
const int buttonreset=10;
const int displaybutton=9;

const int numReadings=100;
int readings[numReadings];
int readindex=0;
int total=0;
int average=0;

int tp;
int fpmax=999;
int fpmin=0;
int h;
int p;
bool c=true;

char tempString[10];
char brightString[10];
byte bright_idx=4;
const uint8_t bright[5]={0,25,50,75,100};


void setup() {
  pinMode(buttonset,INPUT_PULLUP);
  pinMode(buttonreset,INPUT_PULLUP);
  pinMode(displaybutton,INPUT_PULLUP);
  Serial.begin(9600);
  
  for(int i=0; i<numReadings; i++){
    readings[i]=0;
  }
  Serial7Segment.begin(9600);
  Serial7Segment.write('v');
  Serial7Segment.write(0x7A);
  Serial7Segment.write((byte) 100);
  sprintf(brightString,"b%03d",100);
  Serial7Segment.print(brightString);
}

void setMin(){
   int tmin;
   while(1){
     tmin=analogRead(sense);
     Serial.println(tmin);
     sprintf(tempString, "%4d", tmin);
     Serial7Segment.print(tempString);
     if(digitalRead(displaybutton)==LOW){
       brightChange();
      digitalWrite(displaybutton,HIGH);
     }    
     if(digitalRead(buttonset)==LOW) {
      fpmin =tmin;
      h=1;
      digitalWrite(buttonset,HIGH);
      
      Serial.println("min:");
      Serial.println(fpmin);
      sprintf(tempString, "%04d", fpmin);
      Serial7Segment.print(tempString);
      delay(2000);
      break;
    }
    delay(1000);
  }
}

void  setMax(){
  int tmax;
  while(1){
    tmax=analogRead(sense);
    Serial.println(tmax);
    sprintf(tempString, "%4d", tmax);
    Serial7Segment.print(tempString);
    if(digitalRead(displaybutton)==LOW){
      brightChange();
      digitalWrite(displaybutton,HIGH);
    }
    if(digitalRead(buttonset)==LOW && h==1){
      fpmax=tmax;
      h=0;
      digitalWrite(buttonset,HIGH);
      
      Serial.println("max:");
      Serial.println(fpmax);
      sprintf(tempString, "%04d", fpmax);
      Serial7Segment.print(tempString);
      delay(2000);
      break;
    }
    delay(1000);
  }
}

void reset() {
  Serial.println("reset");
  sprintf(tempString,"%04d",0);
  Serial7Segment.print(tempString);
  delay(1500);
  fpmin=0;
  fpmax=0;
  h=0;
  c=true;
  digitalWrite(buttonreset,HIGH);  
}

void brightChange() {
  bright_idx++;
  bright_idx=bright_idx%5;
  Serial7Segment.write(0x7A);
  Serial7Segment.write((byte) bright[bright_idx]);
  sprintf(brightString,"b%03d",bright[bright_idx]);
  Serial7Segment.print(brightString);
  digitalWrite(displaybutton,HIGH);
  delay(500);

}
void loop() {
  if(c){
    setMin();
    setMax();
    c=false;
  }
  if(digitalRead(displaybutton)==LOW){
    brightChange();
    digitalWrite(displaybutton,HIGH);
  }
  if(digitalRead(buttonreset)==LOW) {
    reset();
  }
  total=total-readings[readindex];
  readings[readindex]=analogRead(sense);
  total=total+readings[readindex];
  readindex++;
  //tp=analogRead(sense);
  if(readindex>=numReadings){
    readindex=0;
    average=total/numReadings;
    
    p=map(average,fpmin,fpmax,0,1000);
    sprintf(tempString,"%4d",p);
    Serial7Segment.print(tempString); 
    Serial.println(average);
    
  }
  
  delay(10);

}
