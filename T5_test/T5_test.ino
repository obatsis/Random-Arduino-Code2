#include "DHT.h" 

#define LDELAY 1 * 1000

ADC_MODE(ADC_VCC);

const int dht_pin = 2;

const int blue_pin = 16;
const int green_pin = 4;
const int red_pin = 13;

const int relay_pin = 12;
const int buzzer_pin = 3;

const int k1_pin = 5;
const int k2_pin = 14;

float temp = 0;
float set_temp = 25;
float humidity = 0;

DHT dht(dht_pin, DHT11, 15);


void init_T5(){
  pinMode(blue_pin, OUTPUT);
  digitalWrite(blue_pin, HIGH);
  pinMode(red_pin, OUTPUT);
  digitalWrite(red_pin, HIGH);
  pinMode(green_pin, OUTPUT);
  digitalWrite(green_pin, HIGH);
  
  pinMode(buzzer_pin, OUTPUT);
  digitalWrite(buzzer_pin, LOW);
  
  pinMode(relay_pin, OUTPUT);
  digitalWrite(relay_pin, LOW);

  pinMode(k1_pin, INPUT_PULLUP);
  pinMode(k2_pin, INPUT_PULLUP);
}

void heat(bool state) {
  digitalWrite(red_pin, !state);
  digitalWrite(relay_pin, state);
}

void setup() {
Serial.begin(115200);
Serial.println();
Serial.print("******************************************************");
init_T5();
dht.begin();
}

void loop() {
  float vdd = ESP.getVcc() / 1000.0;
  set_temp += (float)button_handle()/2;
  
  getSensorData();
  BFlash();

  heat(set_temp>temp);

  digitalWrite(green_pin, digitalRead(k2_pin));
  
  Serial.println();
  Serial.print("Set Temp: ");
  Serial.print(set_temp);
  Serial.print("C - Real Temp: ");
  Serial.print(temp);
  Serial.print("C - Humidity: ");
  Serial.print(humidity);
  Serial.print("% - Voltage: ");
  Serial.print(vdd);
  Serial.println("V ");
}

int button_handle(){
  int res=0;
  if(!digitalRead(k1_pin) || !digitalRead(k2_pin)) {
    int k1=0;
    int k2=0;
    while(!digitalRead(k1_pin) || !digitalRead(k2_pin))
    {
      if(!digitalRead(k1_pin)) k1++;
      if(!digitalRead(k2_pin)) k2++;
      delay(1);
    }
    if(k1>40) res+=1;
    if(k2>40) res-=1;  
    delay(20);
  }
  return res;
}

void BFlash() {
  digitalWrite(buzzer_pin, LOW);
  digitalWrite(blue_pin, LOW);
  delay(80);
  digitalWrite(buzzer_pin, HIGH);
  digitalWrite(blue_pin, HIGH);
  
}

void getSensorData() {
  temp = dht.readTemperature();
  humidity = dht.readHumidity();
  while ((isnan(temp) || isnan(humidity)) || (temp == 0 && humidity == 0)) {
    Serial.print("?");
    delay(500);
    temp = dht.readTemperature();
    humidity = dht.readHumidity();
  }
  Serial.println("readed");
}

