/*
 * Remember to add if(root.isOpen()) root.close(); in SD.begin(); library 
 */

#include <SPI.h>
#include <SD.h>
#include <RGB.h>  
#include <PCD8544.h>
#include <MenuSystem.h>
#include <MemoryFree.h>

//const int red = 44;
//const int grn = 45;
//const int blu = 46;

#define ESC 3
#define ENTER 2
#define UP 1
#define DOWN 0

int back_bt = A3;
int forw_bt = A2;
int up_bt = A1;
int dw_bt = A0;

const int sd_ss = 4;
const int sd_write_protect = 3;
const int sd_insterted = 2;

int M_pin = 15;
int P_pin = 16;
int S_pin = 17;
int W_pin = 18;

int d1in_pin = 19;
int clkin_pin = 20;
int d2in_pin = 21;

int d1out_pin = 7; //40;
int clkout_pin = 6; //41;
int d2out_pin = 5; //42;

//RGB led(red,grn,blu);
static const byte glyph[] = { B01111111, B00111110, B00011100, B00001000, B00000000 };
static PCD8544 lcd = PCD8544(8, 9, 10, 12, 11);


unsigned int bit_index = 3;
bool read_first_time;
int byte_count = 0;
uint8_t data[380];
int bit1, bit2;
unsigned char d1 = 0;
unsigned char d2 = 0;

volatile unsigned long clk_microns;
volatile bool clk_flag = false;

unsigned long read_start_ms;
unsigned long event_ms;
unsigned long read_duration = 8; //seconds

File file;
const int max_file_number=200;
String filename[max_file_number];
int filename_size = 1;
String fname;
char cmd=' ';

  // Menu variables
String menu[5];

MenuSystem ms;
Menu mm("SELECT");
MenuItem mi_sync("PC sync");
MenuItem mi_load("Load Job");
MenuItem mi_read("Record Job");
MenuItem mi_write("Write Job");
MenuItem mi_test("Test");

void on_sync_selected(MenuItem* p_menu_item)
{
  lcd.clear();
//  lcd.setCursor(0,0);
//  lcd.print("PC sync");
//  delay(500); // so we can look the result on the LCD
  cmd = 'p';
}

void on_load_selected(MenuItem* p_menu_item)
{
  lcd.clear();
//  lcd.setCursor(0,0);
//  lcd.print("loading");
//  delay(500); // so we can look the result on the LCD
  cmd = 'l';
}

void on_write_selected(MenuItem* p_menu_item)
{
  lcd.clear();
//  lcd.setCursor(0,0);
//  lcd.print("write");
//  delay(500); // so we can look the result on the LCD
  cmd = 'W';
}

void on_read_selected(MenuItem* p_menu_item)
{
  lcd.clear();
//  lcd.print("read");
//  delay(500); // so we can look the result on the LCD
  cmd='R';
}

void on_test_selected(MenuItem* p_menu_item)
{
  lcd.clear();
//  lcd.print("test");
//  delay(500); // so we can look the result on the LCD
  cmd='T';
}

void displayMenu() { 
  lcd.clear();
  Menu const* cp_menu = ms.get_current_menu();
  for(int i =0; i<5; i++) {
    lcd.setCursor(0,i);
    if(String(cp_menu->get_selected()->get_name()) == menu[i]) {
      lcd.write(0);
    }
    else {
      lcd.write(' ');
    }
    lcd.println(menu[i]);
  }
  Serial.println("displayMenu()");
  Serial.flush();
}

void createJobList() {   
  SD.begin(sd_ss);
  File dir= SD.open("/");
  int i=0;
  while (true) {
    File entry =  dir.openNextFile();
    if(i>max_file_number-1) {
      break;
    }
    if (! entry) {
      // no more files
      entry.close();
      break;
    }
    filename[i]=String("");
    if (entry.isDirectory()) {
      filename[i] += "/";
    }
    filename[i] += String(entry.name());
    Serial.print(i);
    Serial.print(": ");
    Serial.println(filename[i]);
    Serial.flush();
    entry.close();
    i++;
  }
  dir.close();
  filename_size = i;
}

void printJobFromList(int selected_idx) { 
  Serial.println("printJobFromList()"); 
  Serial.flush();
  lcd.clear();
  
  // check idx 
  if (selected_idx < 0)
    selected_idx =0;
  if (selected_idx > filename_size-1)
    selected_idx =filename_size-1;
    
  for (int j=0; j<6; j++) {
    lcd.setCursor(0,j);
    int id = j-2+selected_idx;
    if ((id>=0) && (id<=filename_size-1)) {
      if (id == selected_idx) {
        lcd.write(0);
      }
      else {
        lcd.write(' ');
      }
      lcd.println(filename[id]);
    }
  }
}

bool selectJobFromList(int idx) {
  bool isSelected=false;
  Serial.println("selectJobFromList()");
  Serial.flush();
  printJobFromList(idx);
  while(1) {
    if (idx < 0) {
      idx =0;
    }
    if (idx > filename_size-1) {
      idx =filename_size-1;
    }
    int b=0;
    int f=0;
    int u=0;
    int d =0;
    if(!digitalRead(back_bt) || !digitalRead(forw_bt) || !digitalRead(up_bt) || !digitalRead(dw_bt) ) {
      while(!digitalRead(back_bt) || !digitalRead(forw_bt) || !digitalRead(up_bt) || !digitalRead(dw_bt) ){ 
        if(!digitalRead(back_bt))
          b++;
        if(!digitalRead(forw_bt))
          f++;
        if(!digitalRead(up_bt))
          u++;  
        if(!digitalRead(dw_bt))
          d++;
        delay(1);
      }
      if (b+f+u+d > 70) {
        if(b>70) {
          displayMenu();
          break;
        }
        if(f>70) {
          fname = String(filename[idx]);
          isSelected = true;
          break;
        }
        if(u>70) {
          idx--;
          printJobFromList(idx);
        }
        if(d>70) {
          idx++;
          printJobFromList(idx);
        }  
      }
    }
  }
  return isSelected;
}


void buttonInit() {
  pinMode(back_bt, INPUT_PULLUP);
  pinMode(forw_bt, INPUT_PULLUP);
  pinMode(up_bt, INPUT_PULLUP);
  pinMode(dw_bt, INPUT_PULLUP);
}

void buttonHandler() {
  int b=0;
  int f=0;
  int u=0;
  int d=0;
  if(!digitalRead(back_bt) || !digitalRead(forw_bt) || !digitalRead(up_bt) || !digitalRead(dw_bt) ) {
    while(!digitalRead(back_bt) || !digitalRead(forw_bt) || !digitalRead(up_bt) || !digitalRead(dw_bt) ){ 
      if(!digitalRead(back_bt))
        b++;
      if(!digitalRead(forw_bt))
        f++;
      if(!digitalRead(up_bt))
        u++;  
      if(!digitalRead(dw_bt))
        d++;
      delay(1);
    }
    if (b+f+u+d > 70) {
      if(b>70) {
        ms.back();
        displayMenu();
      }
      if(f>70) {
        ms.select();
        displayMenu();
      }
      if(u>70) {
        ms.prev();
        displayMenu();
      }
      if(d>70) {
        ms.next();
        displayMenu();
      }  
    }
  }
}

bool loadFile() {
    if (SD.exists(fname)) {
    file = SD.open(fname);
    if (file) {
      Serial.print("loading file: ");
      Serial.println(fname);
      int j = 0;
      while (file.available()) {
        byte b = file.read();
        char buf[5];
        int i = 0;
        while (b != '\n') {
          buf[i] = b;
          b = file.read();
          i++;
        }
        data[j] = (uint8_t)atoi(buf);
        Serial.print("data[");
        Serial.print(j);
        Serial.print("]: ");
        Serial.println(data[j]);
        j++;
      }
      byte_count = j;
      file.close();
//      Serial.print(byte_count);
    } 
    else {
      Serial.print("error opening: ");
      Serial.println(fname);
    }
  } 
  else {
    Serial.print("cant find file");
    Serial.println(fname);
  }
}

bool sdInit() {
  Serial.println("sdInit()");
  Serial.flush();
  bool ans=false;
  pinMode(sd_write_protect, INPUT_PULLUP);
  pinMode(sd_insterted, INPUT_PULLUP);
  //attachInterrupt();
  if(!digitalRead(sd_insterted)) {  
    if(!digitalRead(sd_write_protect)) {
      if (SD.begin(sd_ss)) {
        ans=true;
      }      
      else {
        Serial.println("SD initialization failed!");
        Serial.flush();
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.println("    Bad FS");
      }
    }
    else {
      Serial.println("SD Write Protect");
      Serial.flush();
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.println("   SD Write");
      lcd.setCursor(0,2);
      lcd.println("   Protect!");
    }
  }
  else {
    Serial.println("SD not inserted");
    Serial.flush();
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.println("  Insert SD!");
  }
  return ans;
}

void setup() {
  fname = "KOMORI.TXT";
  Serial.begin(115200);
  pinMode(clkin_pin, INPUT);
  pinMode(d1in_pin, INPUT);
  pinMode(d2in_pin, INPUT);
  pinMode(M_pin, INPUT_PULLUP);
  pinMode(P_pin, OUTPUT);
  pinMode(S_pin, OUTPUT);
  pinMode(W_pin, INPUT_PULLUP);
  pinMode(clkout_pin, OUTPUT);
  pinMode(d1out_pin, OUTPUT);
  pinMode(d2out_pin, OUTPUT);
  pinMode(sd_write_protect, INPUT_PULLUP);
  pinMode(sd_insterted, INPUT_PULLUP);

  pinMode(13, OUTPUT);
  analogWrite(13, 100);
  lcd.begin(84, 48);
  lcd.createChar(0, glyph);

  menu[0]="PC sync";
  menu[1]="Load Job";
  menu[2]="Record Job";
  menu[3]="Write Job";
  menu[4]="Test";

  mm.add_item(&mi_sync, &on_sync_selected);
  mm.add_item(&mi_load, &on_load_selected);
  mm.add_item(&mi_read, &on_read_selected);
  mm.add_item(&mi_write, &on_write_selected);
  mm.add_item(&mi_test, &on_test_selected);
  ms.set_root_menu(&mm);

  buttonInit();

//  led.setColour(0,0,0);
  
  digitalWrite(S_pin, LOW);
  digitalWrite(P_pin, HIGH);

//  if(!sdInit()) {
//    delay(5000);
//  }
//  else {
//    createJobList();
//  }

  displayMenu();
//  Serial.println("\nReady\n");
}

void loop() {
//  led.setColour(0,255,0);
  buttonHandler();
  
  // ************************************************************************** READ
  if (cmd == 'R')  {
    simCardInsert();
    
    Serial.print("Waiting to read card during Write ");
    Serial.print(read_duration);
    Serial.println(" seconds");
    Serial.flush();
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("  Waiting to");
    lcd.setCursor(0,2);
    lcd.print("   read Job");
    initWriteDecoder();

    while (millis() - read_start_ms < read_duration * 1000) {
      writeDecoder();
      if (byte_count > 371) break;
    }
    stopWriteDecoder();
    if (byte_count > 371) {
      Serial.print("\nRead Finished: ");
      Serial.println(byte_count);
      Serial.print("Job: ");
      Serial.println(getJobNumber(data));
      Serial.flush();
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("Job:");
      lcd.print(getJobNumber(data));
      lcd.setCursor(0,3);
      lcd.print("    Readed");
      delay(2500);
      cmd='s';
    }
    else {
      Serial.println("\nRead Timedout");
      Serial.flush();
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("   Read Fail");
      delay(2500);
      cmd=' ';
    }
    displayMenu();
  }
  // ************************************************************************** TEST
  if (cmd == 'T')  {
    simCardInsert();
    
    Serial.print("Test mode.... waiting to read card during Write ");
    Serial.print(read_duration);
    Serial.println(" seconds");
    Serial.flush();
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("  Waiting");
    lcd.setCursor(0,2);
    lcd.print(" Test Pattern");
    
    while(digitalRead(M_pin)){
      
    }
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("  Reading");   
    initWriteDecoder();

    while (millis() - read_start_ms < read_duration * 1000) {
      writeDecoder();
      if (byte_count > 371) break;
      //led.setColour(0,255,255);
    }
    stopWriteDecoder();
    if (byte_count > 371) {
      Serial.print("\nFinished Test. Results: ");
      int success = 0;
      for (int i = 0; i < 372; i++) {
        if (data[i] == i % 256) success++;
      }
      Serial.print(success);
      Serial.print('/');
      Serial.println(byte_count);
      Serial.flush();
      lcd.clear();
      lcd.setCursor(2,1);
      lcd.print("Results:");
      lcd.setCursor(2,2);
      lcd.print(success);
      lcd.print('/');
      lcd.print(byte_count);
      delay(2500);
    }
    else {
      Serial.println("\nRead Timedout");
      Serial.flush();
      lcd.clear();    
      lcd.setCursor(0,1);
      lcd.print("Nothing Readed");
      delay(2500);
    }
    if(popUp("VERIFY", 5000)) cmd='W';
    else {
      cmd = ' ';
      displayMenu();
    }
  }
  // ***************************************************************************SAVE
  if (cmd == 's') {
    sdInit();
    fname = getJobNumber(data); 
    file = SD.open(fname, FILE_WRITE);
    if (file) {
      Serial.flush();
      file.seek(0);
//      Serial.print("byte to Save: ");
//      Serial.print(byte_count);
//      Serial.print(" to position: ");
//      Serial.println(file.position());
//      Serial.flush();
      for (int i = 0; i < byte_count; i++) {
        file.println(data[i]);
      }
      file.close();
      Serial.print("Saved filename: ");
      Serial.println(fname);
      Serial.flush();
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("Job:");
      lcd.print(fname);
      lcd.setCursor(0,3);
      lcd.println("     Saved"); 
      delay(2500);
    }
    else {
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print(" Error Saving");
      Serial.print("error opening ");
      Serial.println(fname);
      Serial.flush();
      delay(2500);
    }
    if(popUp("VERIFY", 5000)) cmd='W';
    else {
      cmd = ' ';
      displayMenu();
    }
  }
  
  // ********************************************* ******************************LOAD
  if (cmd == 'l') {
    if(!sdInit()) {
      delay(5000);
      Serial.println("sdInit Error");
      Serial.flush();
      cmd=' ';
    }
    else {
      createJobList();
//      Serial.println("lala");
//      Serial.flush();
      if(selectJobFromList(0)) {
        loadFile();
        cmd='W';
      } 
      else cmd = ' ';
    }
    displayMenu();
  }

  // ************************************************************************* WRITE
  if (cmd == 'W') {
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.println("   Writing...");
    int w = simCardInsert();
    lcd.setCursor(0,2);
    lcd.println(w);
    readEncoder(event_ms);
    displayMenu();
    cmd=' ';
  }

  //*************************************************************************PC sync
  if(cmd=='p') {
    cmd=' ';
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.println("    PC Sync"); 
    while(1) {  
      if(Serial.available()) {
        char var = Serial.read();
        switch (var) {
          case 'g':
            {
//              lcd.clear();
//              lcd.setCursor(0,1);
//              lcd.println(" PC Sending...");
              unsigned long t=millis();
              byte_count=0;
              while(millis()<t+5000) {
                if(Serial.available()) {
                  data[byte_count] = Serial.read();
                  byte_count++;
                  if(byte_count>371) break;//protect overflow of data index
                }
              }
              while(Serial.available()) Serial.read(); //empty buffer
              if (byte_count>371) {
                if(isDataOk(data)) {//check file integrity with komori method
                  lcd.clear();
                  lcd.setCursor(0,1);
                  lcd.print("    PC Job:");
                  lcd.setCursor(15,2);
                  lcd.print(getJobNumber(data));
                  delay(2000);
  //                for(int i=0;i<372;i++) Serial.println(data[i]);
                  lcd.setCursor(0,4);
                  lcd.println("   Writing...");
                  simCardInsert();
                  readEncoder(event_ms);
                }
                else {
                  lcd.clear();
                  lcd.setCursor(0,1);
                  lcd.println(" Data Error");
                  delay(2500);                  
                }
              }
              else {
                lcd.clear();
                lcd.setCursor(0,1);
                lcd.println(" PC send Fail");
                delay(2500);
              }
            }
            break;
          case 'h':
            {
              simCardInsert();           
              lcd.clear();
              lcd.setCursor(0,1);
              lcd.print("PC Waiting to");
              lcd.setCursor(0,2);
              lcd.print("   read Job");
              initWriteDecoder();
          
              while (millis() - read_start_ms < read_duration * 1000) {
                writeDecoder();
                if (byte_count > 371) break;
              }
              stopWriteDecoder();
//              byte_count=372;///////////////////////////////////
              if (byte_count > 371) {
                lcd.clear();
                lcd.setCursor(0,1);
                lcd.print("Job:");
                lcd.print(getJobNumber(data));
                lcd.setCursor(0,3);
                lcd.print("    Readed");
                lcd.setCursor(0,4);
                lcd.print("Sending to PC");
                Serial.write(data, byte_count);
                delay(2500);
                //verify
                lcd.clear();
                lcd.setCursor(0,1);
                lcd.println("   Verifing...");
                simCardInsert();
                readEncoder(event_ms);
              }
              else {
                lcd.clear();
                lcd.setCursor(0,1);
                lcd.print("   Read Fail");
                delay(2500);
              }
            }
            break;
        }
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.println("    PC Sync"); 
      }
      if(!digitalRead(back_bt)) break; //like break
    }
  }
}

void waitConsole(long wait){
  long comp = wait + millis();
  while(digitalRead(M_pin) && comp>millis() ) {}
}

bool simCardInsert() {
  digitalWrite(P_pin, LOW);
  event_ms = millis();
  delay(36);
  digitalWrite(S_pin, HIGH);
  delay(100);
  return digitalRead(W_pin);
}

void stopWriteDecoder() {
  detachInterrupt(digitalPinToInterrupt(d1in_pin));
  detachInterrupt(digitalPinToInterrupt(clkin_pin));
  detachInterrupt(digitalPinToInterrupt(d2in_pin));
  delay(1000);
  digitalWrite(P_pin, HIGH);
  delay(86);
  digitalWrite(S_pin, LOW);
  delay(100);
}

void initWriteDecoder() {
  pinMode(d1in_pin, INPUT);
  pinMode(clkin_pin, INPUT);
  pinMode(d2in_pin, INPUT);

  detachInterrupt(digitalPinToInterrupt(d1in_pin));
  detachInterrupt(digitalPinToInterrupt(clkin_pin));
  detachInterrupt(digitalPinToInterrupt(d2in_pin));
  attachInterrupt(digitalPinToInterrupt(d1in_pin), d1Isr, CHANGE);
  attachInterrupt(digitalPinToInterrupt(clkin_pin), clkIsr, CHANGE);
  attachInterrupt(digitalPinToInterrupt(d2in_pin), d2Isr, CHANGE);

  read_start_ms = millis();
  byte_count = 0;
  bit_index = 3;
  clk_flag = false;
}

void clkIsr() {
  clk_microns = micros();
  clk_flag = true;
  bit1 = 0;
  bit2 = 0;
}

void d1Isr() {
  if (micros() - clk_microns < 250) { //period of 1ms
    bit1 = 1;
  }
}

void d2Isr() {
  if (micros() - clk_microns < 250) { //period of 1ms
    bit2 = 1;
  }
}
void writeDecoder() {
  if (clk_flag) { //check if clk interrupt executed
    if (micros() - clk_microns >= 251) {
      d1 |= bit1 << bit_index;
      d2 |= bit2 << bit_index;
      bit_index--;
      if (bit_index == -1) {
        //        Serial.print(d1);
        //        Serial.print(", ");
        //        Serial.println(d2);
        data[byte_count] = (d2 << 4) | d1;
        //        Serial.print("data[");
        //        Serial.print(byte_count);
        //        Serial.print("]: ");
        //        Serial.println(data[byte_count]);
        byte_count++;
        bit_index = 3;
        d1 = 0;
        d2 = 0;
//        Serial.print('.');
      }
      clk_flag = false;
    }
    //  Serial.println(byte_count);
  }
}

void readEncoder(unsigned long stamp) {
  pinMode(clkout_pin, OUTPUT);
  pinMode(d1out_pin, OUTPUT);
  pinMode(d2out_pin, OUTPUT);
  Serial.println("Read encoder Started");
  Serial.flush();
  
  digitalWrite(clkout_pin, LOW);
  digitalWrite(d1out_pin, LOW);
  digitalWrite(d2out_pin, LOW);

  //init clk pulse
  while ( millis() - stamp < 315 );

  digitalWrite(d1out_pin, LOW);
  digitalWrite(d2out_pin, HIGH);
  delayMicroseconds(40);
  digitalWrite(clkout_pin, HIGH);
  delayMicroseconds(80);
  digitalWrite(d1out_pin, LOW);
  digitalWrite(d2out_pin, LOW);
  delayMicroseconds(40);
  digitalWrite(clkout_pin, LOW);
  delayMicroseconds(825);
  Serial.print("Send init clk pulse after: ");
  Serial.print(millis() - stamp);
  Serial.println(" ms");
  Serial.flush();
  stamp = millis();

  delay(210);
  Serial.print("Begin transmit data after: ");
  Serial.print(millis() - stamp);
  Serial.println(" ms");
  Serial.flush();
  stamp = millis();

  for (int i = 0; i < 372; i++)
  {
    for (byte mask = 0b00001000; mask >= 1; mask >>= 1)
    {
      byte w2 = data[i] / 16;
      byte w1 = data[i] % 16;
      digitalWrite(d1out_pin, w1 & mask ? 1 : 0);
      digitalWrite(d2out_pin, w2 & mask ? 1 : 0);
      delayMicroseconds(40);
      digitalWrite(clkout_pin, HIGH);
      delayMicroseconds(80);
      digitalWrite(d1out_pin, LOW);
      digitalWrite(d2out_pin, LOW);
      delayMicroseconds(40);
      digitalWrite(clkout_pin, LOW);      
      delayMicroseconds(950);

//      Serial.print((w1&mask)?1:0);
//      Serial.print(", ");
//      Serial.println((w2&mask)?1:0);
    }
  }

  Serial.print("End transmit data after: ");
  Serial.print(millis() - stamp);
  Serial.println(" ms");
  Serial.flush();
  stamp = millis();

  delay(1446);
  digitalWrite(P_pin, HIGH);
  
  Serial.print("P released after: ");
  Serial.print(millis() - stamp);
  Serial.println(" ms");
  Serial.flush();
  stamp = millis();

  delay(85);
  digitalWrite(S_pin, LOW);
  
  Serial.print("S released after: ");
  Serial.print(millis() - stamp);
  Serial.println(" ms");
  Serial.flush();
  stamp = millis();

  Serial.println("Read encoder Finished");
  Serial.flush();
//  pinMode(clkout_pin, INPUT);
//  pinMode(d1out_pin, INPUT);
//  pinMode(d2out_pin, INPUT);
}

bool isDataOk(uint8_t * din) {
  bool res=true;
  uint8_t zoneCksum[6];
  for (int c = 0; c < 6; c++) {
      zoneCksum[c] = 0;
      for (int i = 0; i < 48; i++) zoneCksum[c] += data[c * 48 + i];
      if(!(data[365 + c] == zoneCksum[c])) res = (res && false);
  }
  uint8_t restCksum = 0;
  for (int i = 288; i < 365; i++) restCksum += data[i];
  if(!(data[371] == restCksum)) res = (res && false);
  return res;
}

String getJobNumber(uint8_t * din) {
  String buf;
  for (int i = 0; i < 8; i++) {
    switch (din[i + 288]) {
      case 96:
        buf += '1';
        break;
      case 181:
        buf += '2';
        break;
      case 244:
        buf += '3';
        break;
      case 102:
        buf += '4';
        break;
      case 214:
        buf += '5';
        break;
      case 215:
        buf += '6';
        break;
      case 114:
        buf += '7';
        break;
      case 247:
        buf += '8';
        break;
      case 246:
        buf += '9';
        break;
      case 243:
        buf += '0';
        break;
    }
  }
  buf += ".txt";
  return buf;
}

char getJobNameChar(uint8_t var) {
  char val;
  switch (var) {
    case 96:
      val = '1';
      break;
    case 181:
      val = '2';
      break;
    case 244:
      val = '3';
      break;
    case 102:
      val = '4';
      break;
    case 214:
      val = '5';
      break;
    case 215:
      val = '6';
      break;
    case 114:
      val = '7';
      break;
    case 247:
      val = '8';
      break;
    case 246:
      val = '9';
      break;
    case 243:
      val = '0';
      break;
    default:
      val = 0;
      // if nothing else matches, do the default
      // default is optional
      break;
  }
  return val;
}

bool popUp(String msg, long int _time_ms) {
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("Press Enter to");
  lcd.setCursor(0,3);
  lcd.print(msg);
  if(buttonWaitEvent(_time_ms)==ENTER) return true;
  else return false;
  
}

int buttonWaitEvent(long int _ms) {
//  #define ESC 3
//#define ENTER 2
//#define UP 1
//#define DOWN 0
  int b=0;
  int f=0;
  int u=0;
  int d=0;
  for(long int i=0; i<_ms; i++) {
    if(!digitalRead(back_bt) || !digitalRead(forw_bt) || !digitalRead(up_bt) || !digitalRead(dw_bt) ) {
      while(!digitalRead(back_bt) || !digitalRead(forw_bt) || !digitalRead(up_bt) || !digitalRead(dw_bt) ){ 
        if(!digitalRead(back_bt))
          b++;
        if(!digitalRead(forw_bt))
          f++;
        if(!digitalRead(up_bt))
          u++;  
        if(!digitalRead(dw_bt))
          d++;
        delay(1);
      }
      if (b+f+u+d > 70) {
        if(b>70) {
          return ESC;
        }
        if(f>70) {
          return ENTER;
        }
        if(u>70) { 
          return UP;
        }
        if(d>70) {
          return DOWN;
        }
        break;  
      }
    }
    delay(1);
  }
  return ESC;
}

