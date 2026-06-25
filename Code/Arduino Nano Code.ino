#include <Arduino.h>
#include "BasicStepperDriver.h"
#include "MultiDriver.h"
#include "SyncDriver.h"
#include <Servo.h>
#include <SoftwareSerial.h>

Servo myservo;
int A = 0, B = 0, C = 0;
int Start = 88;
int End = 81;
bool stopSignal = false;

#define MOTOR_STEPS 200
#define DIR_X 7
#define STEP_X 4
#define DIR_Y 6
#define STEP_Y 3
#define MICROSTEPS 16
#define MOTOR_X_RPM 20
#define MOTOR_Y_RPM 200

BasicStepperDriver stepperX(MOTOR_STEPS, DIR_X, STEP_X);
BasicStepperDriver stepperY(MOTOR_STEPS, DIR_Y, STEP_Y);
SyncDriver controller(stepperX, stepperY);

SoftwareSerial espSerial(2, 3); // RX, TX

// --- Function declarations ---
void readESP();
void safeDelay(unsigned long ms);
void moveWithStop(long xSteps, long ySteps);

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);

  myservo.attach(9);
  myservo.write(Start);

  stepperX.begin(MOTOR_X_RPM, MICROSTEPS);
  stepperY.begin(MOTOR_Y_RPM, MICROSTEPS);

  Serial.println("Nano ready — waiting for A,B,C or STOP from ESP32...");
}

void loop() {
  readESP();
  if (!stopSignal) runStepperLogic();
}

// --- Read serial data from ESP32 ---
void readESP() {
  while (espSerial.available()) {
    String data = espSerial.readStringUntil('\n');
    data.trim();

    if (data.equalsIgnoreCase("STOP")) {
      stopSignal = true;
      Serial.println("🚨 STOP received!");
      return;
    }

    int firstComma = data.indexOf(',');
    int secondComma = data.indexOf(',', firstComma + 1);
    if (firstComma > 0 && secondComma > firstComma) {
      A = data.substring(0, firstComma).toInt();
      B = data.substring(firstComma + 1, secondComma).toInt();
      C = data.substring(secondComma + 1).toInt();
      Serial.print("Parsed A="); Serial.print(A);
      Serial.print(" B="); Serial.print(B);
      Serial.print(" C="); Serial.println(C);
    }
  }
}

// --- Safe delay that checks for STOP ---
void safeDelay(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    readESP();
    if (stopSignal) return;
    delay(10);
  }
}

// --- Move with STOP interrupt ---
void moveWithStop(long xSteps, long ySteps) {
  const long stepChunk = 50; // move in small bursts
  long stepsMoved = 0;

  while (stepsMoved < abs(xSteps) && !stopSignal) {
    long stepNow = min(stepChunk, abs(xSteps - stepsMoved));
    controller.move((xSteps > 0 ? stepNow : -stepNow), (ySteps > 0 ? stepNow : -stepNow));
    stepsMoved += stepNow;
    readESP();
  }
}

void runStepperLogic() {
  if (A >= 0 && B > 0 && C > 0) {
    Serial.println("Starting sequence...");
    safeDelay(500);

    for (int i = 0; i < C; i++) {
      if (stopSignal) break;

      moveWithStop(A * (-4.29), 0);
      safeDelay(500);

      controller.rotate(0, (-360));
      safeDelay(500);

      moveWithStop(B * (-4.29), 0);
      safeDelay(500);

      controller.rotate(0, (-360));
      safeDelay(500);

      moveWithStop(A * (-4.29), 0);
      safeDelay(500);

      myservo.write(End);
      safeDelay(500);
      controller.rotate(0, (-360));
      safeDelay(500);
      myservo.write(Start);
      safeDelay(500);

      Serial.print("Cycle ");
      Serial.print(i + 1);
      Serial.println(" complete.");
    }

    A = B = C = 0;
    if (stopSignal) Serial.println("Sequence stopped early!");
    stopSignal = false;
    Serial.println("Ready for next input...");
  }
}