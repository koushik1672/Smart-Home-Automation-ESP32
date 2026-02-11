#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include <DHT.h>

// Configuration
#define BLYNK_AUTH_TOKEN "YourAuthToken"   // From Blynk App
#define WIFI_SSID "YourWiFi"
#define WIFI_PASS "YourPassword"

#define DHTPIN 5        // DHT22 on GPIO5
#define PIR_PIN 4       // PIR on GPIO4
#define RELAY_PIN 12    // Relay on GPIO12
#define FAN_PIN 13      // PWM pin for fan speed

#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// -------- Setup --------
void setup() {
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);

  dht.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Blynk.config(BLYNK_AUTH_TOKEN);

  timer.setInterval(1000L, sendSensorData);   // Send data every 1s
}

// -------- Main Loop --------
void loop() {
  Blynk.run();
  timer.run();
  checkMotion();
}

// -------- Send temperature & humidity --------
void sendSensorData() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  Blynk.virtualWrite(V1, t);   // Virtual Pin V1 for temperature
  Blynk.virtualWrite(V2, h);   // Virtual Pin V2 for humidity
}

// -------- Motion Activated Light --------
void checkMotion() {
  if (digitalRead(PIR_PIN) == HIGH) {   // (Corrected syntax from document)
    digitalWrite(RELAY_PIN, HIGH);
    Blynk.virtualWrite(V3, "Motion Detected!");
  } else {
    digitalWrite(RELAY_PIN, LOW);
  }
}

// -------- Manual Light Control from Blynk --------
BLYNK_WRITE(V4) {
  int state = param.asInt();
  digitalWrite(RELAY_PIN, state);
}

// -------- Fan Speed Control from Blynk --------
BLYNK_WRITE(V5) {
  int speed = param.asInt();
  analogWrite(FAN_PIN, speed);
}

// -------- Auto Fan Based on Temperature --------
void autoFanControl() {
  float t = dht.readTemperature();

  if (t > 28)
    analogWrite(FAN_PIN, 200);   // Medium speed
  else if (t > 32)
    analogWrite(FAN_PIN, 255);   // Full speed
  else
    analogWrite(FAN_PIN, 0);     // Off
}

// -------- Security Add-on (BLE Fallback) --------
#include <BLEDevice.h>

void setupBLE() {
  BLEDevice::init("ESP32-Home");
  // Add BLE service for local control
}

// -------- SSL Secure Connection --------
#include <BlynkSimpleEsp32_SSL.h>

void secureSetup() {
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS, "blynk.cloud", 80);
}
