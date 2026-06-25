/*************************************************************
  Blynk IoT 2.0 Test Code
  Reads A, B, and C values, and sends them together when the
  SEND button (V3) is pressed.
*************************************************************/

#define BLYNK_TEMPLATE_ID "TMPL3q3Z6ziSo"
#define BLYNK_TEMPLATE_NAME "Wire Cutter"
#define BLYNK_AUTH_TOKEN "jc7BsKZm2Ro6jUersUpxIsbGOdvrU6tJ"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// --- WiFi credentials ---
char ssid[] = "UdayModi";
char pass[] = "HelloWifi";

// --- Variables for A, B, C ---
int A_val = 0;
int B_val = 0;
int C_val = 0;
bool readyToSend = false;

BlynkTimer timer;

// --- Get values from Blynk ---
BLYNK_WRITE(V1) { A_val = param.asInt(); Serial.print("A updated: "); Serial.println(A_val); }
BLYNK_WRITE(V2) { B_val = param.asInt(); Serial.print("B updated: "); Serial.println(B_val); }
BLYNK_WRITE(V3) { C_val = param.asInt(); Serial.print("C updated: "); Serial.println(C_val); }

// --- Send Button Handler ---
BLYNK_WRITE(V0) {
  int btn = param.asInt();
  if (btn == 1) {  // Button pressed
    readyToSend = true;
  }
}

// --- Function to send all values together ---
void sendAllValues() {
  if (readyToSend) {
    Serial.println("🚀 Sending all values together:");
    Serial.print("A = "); Serial.println(A_val);
    Serial.print("B = "); Serial.println(B_val);
    Serial.print("C = "); Serial.println(C_val);
    Serial.println("--------------------------");

    // If later sending to Nano, this is where you'd do:
    // Serial2.print(String(A_val) + "," + String(B_val) + "," + String(C_val) + "\n");

    readyToSend = false;
    // Optionally reset button state in Blynk:
    Blynk.virtualWrite(V0, 0);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("🔌 Starting Blynk Test (Send Button Mode)...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(500L, sendAllValues);
}

void loop() {
  Blynk.run();
  timer.run();
}