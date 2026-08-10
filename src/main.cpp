#include <Arduino.h>
#include <Stepper.h>

const int stepsPerRevolution = 4096;  // 28BYJ-48 (1/64 減速後)

Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

void setup() {
  myStepper.setSpeed(15);
  Serial.begin(9600);
}

void loop() {
  Serial.println("clockwise");
  myStepper.step(2048);  // 半回転
  delay(500);

  Serial.println("counterclockwise");
  myStepper.step(-2048);  // 逆方向に半回転
  delay(500);
}
