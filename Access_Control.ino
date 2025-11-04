#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// -- Buttons ----
#include <EncButton.h>
#define EB_NO_FOR // отключить поддержку pressFor/holdFor/stepFor и счётчик степов
#define EB_NO_CALLBACK // отключить обработчик событий attach (экономит 2 байта оперативки)
#define EB_NO_COUNTER // отключить счётчик энкодера (экономит 4 байта оперативки)
#define EB_NO_BUFFER // отключить буферизацию энкодера (экономит 1 байт оперативки)
#define EB_DEB_TIME 50 // таймаут гашения дребезга кнопки (кнопка)
#define EB_CLICK_TIME 500 // таймаут ожидания кликов (кнопка)

Button btnUp(3);
Button btnDown(4);
MultiButton bothBtn; 




const String version = "ver 0.1a";
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Access Control");
  lcd.setCursor(16 - version.length(), 1);
  lcd.print(version);
  delay(3000); // ShowLogo
  lcd.clear();
  // lcd.noBacklight();
  btnUp.setTimeout(500);
  btnDown.setTimeout(500);
  Serial.begin(9600);
}

void loop() {
  bothBtn.tick(btnUp, btnDown);
  if (btnUp.click()){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Click up  ");
    Serial.println("CLICK UP");
  }
  if (btnDown.click()){
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Click down");
    Serial.println("CLICK DOWN");
  }
  if (bothBtn.hold()){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("HOLD DOUBLE");
    Serial.println("HOLD DOUBLE");
    }
  else if (btnUp.hold()){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("HOLD Up   ");
    Serial.println("HOLD Up   ");
  }
  else if (btnDown.hold()){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("HOLD down");
    Serial.println("HOLD DOWN");
  }
}
