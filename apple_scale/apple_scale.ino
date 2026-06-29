/*
 * Apple Weight Scale
 * Hardware: ESP32 + HX711 Load Cell Amplifier + I2C LCD 16x2
 *
 * Wiring:
 *   HX711  →  ESP32
 *   DOUT   →  GPIO 4
 *   SCK    →  GPIO 5
 *   VCC    →  3.3V
 *   GND    →  GND
 *
 *   I2C LCD (PCF8574 backpack) → ESP32
 *   SDA    →  GPIO 21
 *   SCL    →  GPIO 22
 *   VCC    →  3.3V
 *   GND    →  GND
 *
 * Libraries needed (install via Arduino Library Manager):
 *   - "HX711 Arduino Library" by bogde
 *   - "LiquidCrystal_I2C" by Frank de Brabander
 */

#include <HX711.h>
#include <LiquidCrystal_I2C.h>

// ── Pin definitions ──────────────────────────────────────────
#define DOUT_PIN   4
#define SCK_PIN    5

// ── HX711 calibration ────────────────────────────────────────
// Run the calibration sketch first, then paste your factor here.
// Typical range: 400 – 2000 depending on your load cell.
#define CALIBRATION_FACTOR  420.0

// ── I2C LCD ──────────────────────────────────────────────────
// Default I2C address for PCF8574 backpack is 0x27 or 0x3F.
// Use an I2C scanner sketch if unsure.
#define LCD_ADDRESS  0x27
#define LCD_COLS     16
#define LCD_ROWS     2

// ── Objects ───────────────────────────────────────────────────
HX711 scale;
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);

// ── Tare button (optional) ────────────────────────────────────
// Connect a momentary push-button between GPIO 15 and GND.
#define TARE_BUTTON_PIN  15

// ── Timing ───────────────────────────────────────────────────
unsigned long lastReadTime   = 0;
const unsigned long READ_INTERVAL = 300;  // ms between readings

// ── Custom LCD characters ─────────────────────────────────────
byte kgIcon[8] = {
  0b01110, 0b01010, 0b01110, 0b01010,
  0b00000, 0b00000, 0b00000, 0b00000
};

// ─────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);

  // LCD initialisation
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, kgIcon);

  lcd.setCursor(0, 0);
  lcd.print("  Apple Scale   ");
  lcd.setCursor(0, 1);
  lcd.print("  Initializing..");
  delay(1500);

  // HX711 initialisation
  scale.begin(DOUT_PIN, SCK_PIN);
  scale.set_scale(CALIBRATION_FACTOR);

  // Wait for stable reading
  lcd.setCursor(0, 1);
  lcd.print("  Taring...     ");
  scale.tare();   // Reset to zero

  // Tare button (active LOW with internal pull-up)
  pinMode(TARE_BUTTON_PIN, INPUT_PULLUP);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Place Apple   ");
  lcd.setCursor(0, 1);
  lcd.print("  Weight: ----  ");
  delay(1000);

  Serial.println("Apple Scale ready. Place apple on platform.");
}

// ─────────────────────────────────────────────────────────────
void loop() {
  // Handle tare button press
  if (digitalRead(TARE_BUTTON_PIN) == LOW) {
    delay(50);  // debounce
    if (digitalRead(TARE_BUTTON_PIN) == LOW) {
      scale.tare();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("   Tared!       ");
      lcd.setCursor(0, 1);
      lcd.print("  Weight: 0.000 ");
      delay(800);
      while (digitalRead(TARE_BUTTON_PIN) == LOW);  // wait for release
    }
  }

  // Read weight at interval
  if (millis() - lastReadTime >= READ_INTERVAL) {
    lastReadTime = millis();

    if (scale.is_ready()) {
      float weightKg = scale.get_units(5);  // average of 5 readings

      // Clamp negative noise to zero
      if (weightKg < 0.001f) weightKg = 0.0f;

      displayWeight(weightKg);

      // Serial output for debugging / calibration
      Serial.print("Weight: ");
      Serial.print(weightKg, 3);
      Serial.println(" kg");
    } else {
      lcd.setCursor(0, 1);
      lcd.print("  Sensor Error  ");
      Serial.println("HX711 not ready.");
    }
  }
}

// ─────────────────────────────────────────────────────────────
void displayWeight(float kg) {
  float grams = kg * 1000.0f;

  lcd.setCursor(0, 0);
  lcd.print("  Apple Weight  ");

  lcd.setCursor(0, 1);

  if (kg < 0.001f) {
    lcd.print("  Empty: 0.000kg");
  } else {
    // Format: "  XXX.X g  (kg)"
    lcd.print("  ");

    // Show grams for small weights, kg for larger
    if (grams < 1000.0f) {
      // Display in grams
      char buf[10];
      dtostrf(grams, 6, 1, buf);
      lcd.print(buf);
      lcd.print(" g      ");
    } else {
      // Display in kg
      char buf[10];
      dtostrf(kg, 5, 3, buf);
      lcd.print(buf);
      lcd.print(" kg     ");
    }
  }
}
