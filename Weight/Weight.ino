#include "HX711.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// HX711 Pins
#define DT 4
#define SCK 5

HX711 scale;

// LCD Address (usually 0x27)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Replace with your calibration factor
float calibration_factor = 490;

void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();

  scale.begin(DT, SCK);

  scale.set_scale(calibration_factor);
  scale.tare(); // Reset scale to 0

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Weight System");
  delay(2000);
}

void loop() {

  float weight = scale.get_units(10);

  if (weight < 0)
    weight = 0;

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Wt:");
  lcd.print(weight, 1);
  lcd.print(" g");

  lcd.setCursor(0, 1);

  if (weight < 80) {
    lcd.print("Spoiled");
  } else {
    lcd.print("NORMAL apple");
  }

  Serial.print("Weight: ");
  Serial.print(weight);
  Serial.println(" g");

  delay(500);
}