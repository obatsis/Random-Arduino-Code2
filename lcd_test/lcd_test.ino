#include <LiquidCrystal.h>

int con=20;  //contrast value

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  analogWrite(6, con);
  lcd.begin(16, 2);
    lcd.print("Arduino");
    delay(2000);
    lcd.setCursor(2, 1);
    lcd.print("LCD_TEST");
    delay(2000);

}

void loop() {


 
  lcd.scrollDisplayLeft();
  delay(1000);
 // lcd.clear();

 /* lcd.blink();
  delay(4000);
  lcd.setCursor(7, 1);
  delay(2000);
  lcd.noBlink();

  lcd.cursor();
  delay(2000);
  lcd.noCursor();
  lcd.clear();
  
*/

}
