/*
|| NYCCNC IMPERIALIZER
|| -version 1.5
|| -author Ed Rees
|| -1/15/2018
|| -Copyright Saunders Machine Works, LLC 2018
*/

// required libraries below. If necessary, search for them from the dropdown menu 'Sketch>Include Library>Libraries Manager' and install from there as prompted.
#include <Wire.h> // included with IDE.
#include <avr/sleep.h> // included with IDE. For whatever reason it doesn't turn orange like most libraries do, but it works fine.
#include <Keypad.h> // by Mark Stanley and Alexander Brevig, can be found in the libraries manager.
#include <LiquidCrystal.h> // available at https://arduino-info.wikispaces.com/LCD-Blue-I2C#v1 //for I2C

//LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Set the LCD I2C address to "0x3F". If LCD display is blank, try changing I2C address to "0x27"

float entry = 0; // whole number value.
float a = 0;
float b = 0;
float c = 0;
float d = 0;
float decimalValue = 0; // total decimal number value.
float decimalEntry = 0; // most recent number entered while in decimal mode.
float cursBreak = 0; // breaks main loop to position cursor.
int decimalPlace = 0; // records decimal place for decimal value calculation.
int keyCount = 0; // counts key presses to prevent overflow.
int toggleState = 0; // records state of unit toggle switch.
int cursorCount = 0;
int cursorRow = 1;
int divisor = 0; // used for unit conversion math.
int sleepDelay = 15000;
int sleepTimer = sleepDelay;
int toggleLatch = 0; // current state of the button.
int lastToggleLatch = 0; // previous state of the button.
const int togglePin = 7; // declares digital pin 11 as unit toggle pin.
bool decimalMode = false; // toggles between Inch to Millimeter and Millimeter to Inch conversion.
bool decSkip = true; // used to skip cursor decimal count when entry is less than one.
bool decLatch = false; // used to avoid cursor moving undesirably when decimal button is pushed more than once per entry.
bool wakeMode = false;
bool cursorBump = true;

char customKey;
const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
{'1', '2', '3'},
{'4', '5', '6'},
{'7', '8', '9'},
{'C', '0', 'D'}
};
byte rowPins[ROWS] = {8, 14, 15, 16}; // connect to the row pinouts of the keypad
byte colPins[COLS] = {17, 18, 190}; // connect to the column pinouts of the keypad

Keypad customKeypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);

int con=20;  //contrast value
LiquidCrystal lcd(12, 11, 6, 5, 4, 3);

void setup()
{
pinMode(togglePin, INPUT_PULLUP);
pinMode(2, INPUT_PULLUP);
pinMode(13, OUTPUT);
digitalWrite(13, HIGH);
analogWrite(9, con);
lcd.begin(16, 2);
lcd.clear();
lcd.print("0.0000");
lcd.setCursor(0, 1);
lcd.print("0.0000");

if (toggleState == HIGH) // determine initial unit toggle state and set cursor row accordingly.
{
cursorRow = 0;
}
else
{
cursorRow = 1;
}
}

void loop()
{
customKey = customKeypad.getKey();
toggleState = digitalRead(togglePin);
toggleLatch = digitalRead(togglePin);

a = (entry + decimalValue);
b = (a * 25.4);
c = (a / 25.4);
d = a;

if (sleepTimer <= 1) // puts device to sleep when sleep timer runs out.
{
pinMode(A2, OUTPUT);
digitalWrite(A2, LOW);
sleepNow();
}

if (toggleLatch != lastToggleLatch)
{
sleepTimer = sleepDelay; // reset sleep timer if unit toggle is changed
lcd.begin(16, 2);
lcd.setCursor(0, 0);

if (toggleState == HIGH)
{
cursorRow = 0;
}
else
{
cursorRow = 1;
}
lcd.clear();
cursBreak = d + .5;

}

if (cursBreak == d)
{
lcd.setCursor(cursorCount, cursorRow);
lcd.blink();
}

switch (customKey)
{
case '0' ... '9':

sleepTimer = sleepDelay; // reset sleep timer if a key is pressed.

if (decimalMode == true) // determine if in decimal entry mode:
{
if (keyCount < 7 && decimalPlace < 4) // prevents overflow.
{
keyCount++; // increment key press count by one.
cursorCount++; // increment cursor ount by one.
decimalPlace++; // increment decimal place by one.
divisor = 0.5 + pow(10, decimalPlace); // divisor is set equal to (10^decimalPlace), plus 0.5 to ensure proper rounding.
decimalEntry = (customKey - '0') / (float)divisor; // decimal entry set equal to key entered, and divided by divisor.
decimalValue = decimalValue + decimalEntry; // new decimal value set equal to previous decimal value plus last decimal entry.
}
break;
}
else
{
if (keyCount < 7 && decimalPlace < 4)
{
keyCount++;
if (entry > 0)
{
cursorCount++;
}
if (entry <= 9 && entry > 0 && cursorBump == true)
{
cursorCount++;
cursorBump = false;
}

entry = entry * 10 + (customKey - '0'); // increments digit input position. ie: 1(*10)+5 = 15.
}
break;
}

case 'D': //collects decimal keypress.
decimalMode = true;
cursBreak = (d + .5);
if (decLatch == false && entry == 0)
{
cursorCount++;
}
if (decSkip == true)
{
cursorCount++;
}
decSkip = false;
decLatch = true;
break;

case 'C': // clear- reset relevant variables to defaults and return to title screen.
if(wakeMode == false)
{
cursBreak = d;
entry -= entry;
decimalMode = false;
decimalPlace = 0;
decimalValue -= decimalValue;
decSkip = true;
decLatch = false;
keyCount -= keyCount;
cursorCount -= cursorCount;
cursorBump = true;
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("0.0000");
lcd.setCursor(0, 1);
lcd.print("0.0000");
break;
}
}

if (toggleState == HIGH && cursBreak != d) // if toggle is in inch mode:
{
lcd.setCursor(0, 0);
lcd.print(a, 4); //in
lcd.setCursor(0, 1);
lcd.print (b, 4); //mm
}
else if (cursBreak != d) //if toggle is NOT in inch mode:
{
lcd.setCursor(0, 0);
lcd.print (c, 4); //mm
lcd.setCursor(0, 1);
lcd.print(a, 4); //in
}

cursBreak = d;
sleepTimer -= 1; //counts down from sleep delay value.

if (wakeMode == true) // upon waking, reset relevant variables to defaults and return to title screen.
{
pinMode(togglePin, INPUT_PULLUP);
pinMode(13, OUTPUT);
digitalWrite(13, HIGH);
lcd.begin(16, 2);
if (toggleState == HIGH) // if toggle is in inch mode:
{
lcd.setCursor(0, 0);
lcd.print(a, 4); //in
lcd.setCursor(0, 1);
lcd.print (b, 4); //mm
}
else //if toggle is NOT in inch mode:
{
lcd.setCursor(0, 0);
lcd.print (c, 4); //mm
lcd.setCursor(0, 1);
lcd.print(a, 4); //in
}
wakeMode = false;
}
lastToggleLatch = toggleLatch;
}

//Sleep functions below:

void sleepNow ()
{
digitalWrite(13, LOW);
set_sleep_mode (SLEEP_MODE_PWR_DOWN);
sleep_enable (); // enables the sleep bit in the mcucr register
attachInterrupt (digitalPinToInterrupt (2), wake, LOW); // wake up on low level on D2
sleep_mode (); // here the device is actually put to sleep
detachInterrupt (digitalPinToInterrupt (2)); // stop LOW interrupt on D2
}

void wake () // interrupt service routine in sleep mode
{
sleep_disable (); // first thing after waking from sleep:
sleepTimer = sleepDelay;
pinMode(A2, INPUT);
wakeMode = true;
}
