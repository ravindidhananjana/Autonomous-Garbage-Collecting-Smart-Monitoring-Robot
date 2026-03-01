#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

// ---------- IR Sensors ----------
#define IR_L2 32
#define IR_L1 33
#define IR_C  35
#define IR_R1 34
#define IR_R2 39

// ---------- Motor Driver ----------
#define ENA 25
#define ENB 26
#define IN1 27
#define IN2 14
#define IN3 12
#define IN4 13

// ---------- Ultrasonic ----------
#define TRIG 5
#define ECHO 18

// ---------- PWM & DISTANCE ----------
#define BASE_SPEED 120
#define TURN_SPEED 95
#define OBSTACLE_DIST 15   // cm

// ---------- States ----------
enum State {
  IDLE,
  LINE_FOLLOW,
  FIRST_JUNCTION,
  WAIT_AT_TARGET,
  TURN_BACK,
  RETURN_HOME
};

State robotState = IDLE;

// ---------- Control Variables ----------
char targetPosition = '0';
bool robotEnabled = false;
int junctionCount = 0;

// ---------- Motor Functions ----------
void stopMotor() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  ledcWrite(ENA, 0);
  ledcWrite(ENB, 0);
}

void forward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  ledcWrite(ENA, BASE_SPEED);
  ledcWrite(ENB, BASE_SPEED);
}

void leftCorrection() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  ledcWrite(ENA, TURN_SPEED);
  ledcWrite(ENB, BASE_SPEED);
}

void rightCorrection() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  ledcWrite(ENA, BASE_SPEED);
  ledcWrite(ENB, TURN_SPEED);
}

void turnLeft90() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  ledcWrite(ENA, TURN_SPEED);
  ledcWrite(ENB, TURN_SPEED);
  delay(700);
  stopMotor();
}

void turnRight90() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  ledcWrite(ENA, TURN_SPEED);
  ledcWrite(ENB, TURN_SPEED);
  delay(700);
  stopMotor();
}

void turn180() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  ledcWrite(ENA, BASE_SPEED);
  ledcWrite(ENB, BASE_SPEED);
  delay(1200);
  stopMotor();
}

// ---------- Ultrasonic ----------
long getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long d = pulseIn(ECHO, HIGH, 30000);
  return d * 0.034 / 2;
}

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);
  SerialBT.begin("MYrobot");

  pinMode(IR_L2, INPUT);
  pinMode(IR_L1, INPUT);
  pinMode(IR_C, INPUT);
  pinMode(IR_R1, INPUT);
  pinMode(IR_R2, INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  ledcAttach(ENA, 1000, 8);
  ledcAttach(ENB, 1000, 8);

  stopMotor();
}

// ---------- Loop ----------
void loop() {

  // ---------- Bluetooth ----------
  if (SerialBT.available()) {
    char cmd = SerialBT.read();

    if ((cmd == '1' || cmd == '2' || cmd == '3') && robotState == IDLE) {
      targetPosition = cmd;
      robotEnabled = true;
      robotState = LINE_FOLLOW;
      junctionCount = 0;
    }
    else if (cmd == 'X') {
      robotEnabled = false;
      robotState = IDLE;
      stopMotor();
    }
  }

  if (!robotEnabled) {
    stopMotor();
    return;
  }

  // ---------- Obstacle ----------
  if (getDistance() < OBSTACLE_DIST) {
    stopMotor();
    return;
  }

  int L2 = digitalRead(IR_L2);
  int L1 = digitalRead(IR_L1);
  int C  = digitalRead(IR_C);
  int R1 = digitalRead(IR_R1);
  int R2 = digitalRead(IR_R2);

  // ---------- State Machine ----------
  switch (robotState) {

    case LINE_FOLLOW:
    case RETURN_HOME:

      if (C && !L1 && !R1) forward();
      else if (L1 || L2) leftCorrection();
      else if (R1 || R2) rightCorrection();

      // Node detection
      if (L2 && L1 && C && R1 && R2) {
        delay(150); // debounce
        if (L2 && L1 && C && R1 && R2) {
          junctionCount++;
          stopMotor();
          delay(200);

          if (junctionCount == 1) {
            robotState = FIRST_JUNCTION;
          }
          else if (junctionCount == 2) {
            robotState = WAIT_AT_TARGET;
          }
          else if (junctionCount == 3 && robotState == RETURN_HOME) {
            robotEnabled = false;
            robotState = IDLE;
            junctionCount = 0;
          }
        }
      }
      break;

    case FIRST_JUNCTION:
      if (targetPosition == '1') {
        turnLeft90();
      }
      else if (targetPosition == '2') {
        forward();
        delay(400);   // go straight through junction
        stopMotor();
      }
      else if (targetPosition == '3') {
        turnRight90();
      }
      robotState = LINE_FOLLOW;
      break;

    case WAIT_AT_TARGET:
      stopMotor();
      delay(5000);   // wait 5 seconds
      robotState = TURN_BACK;
      break;

    case TURN_BACK:
      turn180();
      robotState = RETURN_HOME;
      break;

    case IDLE:
      stopMotor();
      break;
  }
}
