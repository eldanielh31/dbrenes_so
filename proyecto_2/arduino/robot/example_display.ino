#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
    Serial.begin(9600);
    lcd.init();
    lcd.backlight();
}

void loop() {
    if (Serial.available()) {
        String message = Serial.readStringUntil('\n');
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(message);
    }
}