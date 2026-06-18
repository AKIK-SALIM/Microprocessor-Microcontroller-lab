#include <Servo.h>

Servo sorter;

#define GAS_DO 2
#define SERVO_PIN 9

void setup() {
  Serial.begin(9600);

  pinMode(GAS_DO, INPUT);

  sorter.attach(SERVO_PIN);
  sorter.write(0);   // Home position

  Serial.println("Apple Sorter Ready");
}

void loop() {

  int gasState = digitalRead(GAS_DO);

  if (gasState == HIGH) {

    Serial.println("Spoiled Apple Detected");

    // Move servo to reject apple
    sorter.write(90);

    delay(1500);

    // Return to home position
    sorter.write(0);

    delay(1000);

  } else {

    Serial.println("Fresh Apple");

  }

  delay(500);
}