#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// ---------------- LCD ----------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------------- Servo ----------------
Servo myServo;
#define SERVO_PIN 13
#define SERVO_OPEN 180   // lid fully open
#define SERVO_CLOSED 0   // lid fully closed

// ---------------- Ultrasonic ----------------
#define TRIG_HAND 25
#define ECHO_HAND 26
#define TRIG_GARB 27
#define ECHO_GARB 14
const int MAX_HAND_CM = 15;
const int EMPTY_DIST = 10;
const int FULL_DIST = 2;
const int TIMEOUT_US = 25000;

// ---------------- WiFi ----------------
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// ---------------- Firebase ----------------
#define API_KEY "YOUR_FIREBASE_API_KEY"
#define DATABASE_URL "YOUR_FIREBASE_DATABASE_URL"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// ---------------- Lid State ----------------
enum LidState { IDLE, OPENING, OPEN, CLOSING };
LidState lidState = IDLE;
unsigned long lidTimer = 0;
const unsigned long OPEN_DURATION = 10000; // 10 sec

// ---------------- Ultrasonic Helper ----------------
long readDistanceCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, TIMEOUT_US);
  if(duration == 0) return 120;
  return duration * 0.034 / 2;
}

// ---------------- Display Garbage ----------------
void displayGarbageLevel() {
  long dist = readDistanceCM(TRIG_GARB, ECHO_GARB);
  dist = constrain(dist, FULL_DIST, EMPTY_DIST);
  int levelPercent = map(dist, EMPTY_DIST, FULL_DIST, 0, 100);

  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Garbage Level");
  lcd.setCursor(0, 1); lcd.print(levelPercent); lcd.print("% Full");

  Serial.print("Garbage Level: "); Serial.print(levelPercent); Serial.println("%");

  Firebase.RTDB.setInt(&fbdo, "/dustbin/garbageLevel", levelPercent);
}

// ---------------- Smooth Servo Movement ----------------
void moveServo(int startAngle, int endAngle, int stepDelay=15) {
  if(startAngle < endAngle) {
    for(int pos = startAngle; pos <= endAngle; pos++) {
      myServo.write(pos);
      delay(stepDelay);
    }
  } else {
    for(int pos = startAngle; pos >= endAngle; pos--) {
      myServo.write(pos);
      delay(stepDelay);
    }
  }
}

// ---------------- Setup ----------------
void setup() {
  Serial.begin(115200);

  pinMode(TRIG_HAND, OUTPUT); pinMode(ECHO_HAND, INPUT);
  pinMode(TRIG_GARB, OUTPUT); pinMode(ECHO_GARB, INPUT);

  myServo.attach(SERVO_PIN);
  myServo.write(SERVO_CLOSED);

  lcd.init(); lcd.backlight();
  lcd.setCursor(0, 0); lcd.print("Smart Dustbin");
  lcd.setCursor(0, 1); lcd.print("Starting...");
  delay(2000); lcd.clear();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting WiFi");
  while(WiFi.status() != WL_CONNECTED) { Serial.print("."); delay(500); }
  Serial.println("\nWiFi Connected");

  config.api_key = API_KEY; config.database_url = DATABASE_URL;
  if(Firebase.signUp(&config, &auth, "", "")) Serial.println("Firebase SignUp OK");
  else Serial.println("Firebase SignUp Error");

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Serial.println("Firebase Ready");
}

// ---------------- Loop ----------------
void loop() {
  long handDist = readDistanceCM(TRIG_HAND, ECHO_HAND);

  unsigned long now = millis();

  switch(lidState) {

    case IDLE:
      if(handDist > 0 && handDist <= MAX_HAND_CM) {
        Serial.println("HAND DETECTED - OPENING LID");
        lcd.clear(); lcd.setCursor(0,0); lcd.print("Hand Detected!");
        lcd.setCursor(0,1); lcd.print("Opening Lid");

        Firebase.RTDB.setString(&fbdo, "/dustbin/lidStatus", "OPEN");

        moveServo(SERVO_CLOSED, SERVO_OPEN, 10); // smooth open
        lidTimer = now;
        lidState = OPEN;
      }
      break;

    case OPEN:
      if(now - lidTimer >= OPEN_DURATION) {
        Serial.println("CLOSING LID");
        lcd.setCursor(0,1); lcd.print("Closing Lid");

        moveServo(SERVO_OPEN, SERVO_CLOSED, 10); // smooth close
        Firebase.RTDB.setString(&fbdo, "/dustbin/lidStatus", "CLOSED");
        displayGarbageLevel();
        lidState = IDLE;
      }
      break;
  }

  delay(50);
}
