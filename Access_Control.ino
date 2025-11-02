#include <Wire.h>
#include <GyverButton.h>
#include <LiquidCrystal_I2C.h>

const String version = "ver 0.1a";

GButton btnUp(3, HIGH_PULL, NORM_OPEN);
GButton btnDown(4, HIGH_PULL, NORM_OPEN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

enum State { STATE_OFF, STATE_MENU, STATE_TOOL };
State state = STATE_OFF;

// --- меню ---
const char* tools[] = {"Tool1", "Tool2", "Tool3"};
const char* kits[][3] = {
  {"1Kit1", "1Kit2", "1Kit3"},
  {"2Kit1", "2Kit2", "2Kit3"},
  {"3Kit1", "3Kit2", "3Kit3"}
};
int toolIndex = 0;
int kitIndex = 0;

uint32_t t_start = 0;
uint32_t lastAction = 0;
const uint16_t INACTIVITY_TIMEOUT = 10000; // 10 сек бездействия

// для логики удержаний
uint32_t holdPendingAt = 0;
uint8_t holdPendingMask = 0;
const uint16_t HOLD_DECIDE_MS = 100;

// LCD debounce
uint32_t lastLcdUpdate = 0;
const uint16_t LCD_UPDATE_INTERVAL = 250;

void showMessage(const char* msg, bool clear = true) {
  if (millis() - lastLcdUpdate > LCD_UPDATE_INTERVAL) {
    if (clear) lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(msg);
    lastLcdUpdate = millis();
  }
}

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Access Control");
  lcd.setCursor(16 - version.length(), 1);
  lcd.print(version);
  delay(5000); // показываем заставку 5 секунд
  lcd.clear();
  lcd.noBacklight(); // "выключаем экран"
  state = STATE_OFF;

  btnUp.setTimeout(500);
  btnDown.setTimeout(500);
}

void loop() {
  btnUp.tick();
  btnDown.tick();

  // === OFF MODE ===
  if (state == STATE_OFF) {
    // Проверяем двойное удержание для входа в меню
    if (btnUp.isHold() && btnDown.isHold()) {
      state = STATE_MENU;
      lcd.backlight();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MENU MODE");
      toolIndex = 0;
      kitIndex = 0;
      delay(500);
      showMenu();
      lastAction = millis();
    }
    return;
  }

  // === MENU MODE ===
  if (state == STATE_MENU) {
    // Проверка бездействия
    if (millis() - lastAction > INACTIVITY_TIMEOUT) {
      lcd.clear();
      lcd.noBacklight();
      state = STATE_OFF;
      return;
    }

    // обработка кликов
    if (btnUp.isClick()) {
      toolIndex = (toolIndex + 1) % 3;
      showMenu();
      lastAction = millis();
    }
    if (btnDown.isClick()) {
      toolIndex = (toolIndex - 1 + 3) % 3;
      showMenu();
      lastAction = millis();
    }

    // обработка удержаний
    if (btnUp.isHolded()) {
      holdPendingMask |= 0x01;
      if (!holdPendingAt) holdPendingAt = millis();
    }
    if (btnDown.isHolded()) {
      holdPendingMask |= 0x02;
      if (!holdPendingAt) holdPendingAt = millis();
    }

    if (holdPendingAt && millis() - holdPendingAt >= HOLD_DECIDE_MS) {
      bool upHeld = btnUp.isHold();
      bool downHeld = btnDown.isHold();

      if (upHeld && downHeld) {
        // двойное удержание — ничего
      } else if (upHeld && !downHeld) {
        // войти в TOOL
        enterTool();
      } else if (!upHeld && downHeld) {
        // выход из меню
        showMessage("Cancel");
        lcd.noBacklight();
        state = STATE_OFF;
      }
      holdPendingAt = 0;
      holdPendingMask = 0;
      lastAction = millis();
    }
  }

  // === TOOL MODE ===
  if (state == STATE_TOOL) {
    if (millis() - lastAction > INACTIVITY_TIMEOUT) {
      lcd.clear();
      lcd.noBacklight();
      state = STATE_OFF;
      return;
    }

    if (btnUp.isClick()) {
      kitIndex = (kitIndex + 1) % 3;
      showTool();
      lastAction = millis();
    }
    if (btnDown.isClick()) {
      kitIndex = (kitIndex - 1 + 3) % 3;
      showTool();
      lastAction = millis();
    }

    if (btnDown.isHolded()) {
      showMessage("Cancel");
      state = STATE_MENU;
      showMenu();
      lastAction = millis();
    }
  }
}

void showMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Select:");
  lcd.setCursor(0, 1);
  lcd.print(tools[toolIndex]);
}

void enterTool() {
  state = STATE_TOOL;
  kitIndex = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ENTER ");
  lcd.print(tools[toolIndex]);
  delay(500);
  showTool();
}

void showTool() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(tools[toolIndex]);
  lcd.setCursor(0, 1);
  lcd.print(kits[toolIndex][kitIndex]);
}
