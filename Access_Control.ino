#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
const String version = "ver 0.1a";

LiquidCrystal_I2C lcd(0x27,16,2);  // Устанавливаем дисплей
void setup()
{
  lcd.init();                     
  lcd.backlight();// Включаем подсветку дисплея
  lcd.setCursor(0, 0);
  lcd.print("Access Control");

  lcd.setCursor(16 - version.length(), 1);
  lcd.print(version);

}
void loop()
{

}