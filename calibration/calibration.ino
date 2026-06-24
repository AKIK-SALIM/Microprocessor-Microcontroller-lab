/*
 * HX711 Calibration Sketch
 * Run this FIRST to find your CALIBRATION_FACTOR.
 *
 * Steps:
 *   1. Upload this sketch.
 *   2. Open Serial Monitor at 115200 baud.
 *   3. Make sure the scale platform is EMPTY, then send 't' to tare.
 *   4. Place a known weight (e.g. 200g or 500g object).
 *   5. Send '+' or '-' to tune the factor until Serial shows the correct weight.
 *   6. Note the calibration factor printed — paste it into apple_scale.ino.
 */

#include <HX711.h>

#define DOUT_PIN  4
#define SCK_PIN   5

HX711 scale;
float calibrationFactor = 400.0;

void setup() {
  Serial.begin(115200);
  scale.begin(DOUT_PIN, SCK_PIN);
  scale.set_scale();
  scale.tare();

  Serial.println("HX711 Calibration");
  Serial.println("Commands: t=tare  +=increase factor  -=decrease factor");
  Serial.println("Place a known weight, then tune until reading matches.");
}

void loop() {
  scale.set_scale(calibrationFactor);
  float reading = scale.get_units(10);

  Serial.print("Reading: ");
  Serial.print(reading, 3);
  Serial.print(" kg  |  Factor: ");
  Serial.println(calibrationFactor, 1);

  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == 't' || cmd == 'T') { scale.tare(); Serial.println("Tared."); }
    if (cmd == '+') calibrationFactor += 10;
    if (cmd == '-') calibrationFactor -= 10;
  }
  delay(500);
}
