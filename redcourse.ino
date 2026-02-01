// --- PIN DEFINITIONS ---
#define S0 12
#define S1 13
#define S2 7
#define S3 6
#define sensorOut 8
#include <Servo.h>

const int trigPin = A3;
const int echoPin = A2;
long duration;
int distance;

Servo servoMarker;
int currentPos = 180;

// Motor Pins
int motor1pin1 = 2; int motor1pin2 = 3; // Left Motor
int motor2pin1 = 4; int motor2pin2 = 5; // Right Motor
int ENA = 9;  int ENB = 10;

int turn = 0;

bool turn_left = false;

// --- COLOR DICTIONARY ---
struct ColorTarget { const char* name; int r, g, b; };
ColorTarget targets[] = {
  {"WHITE", 233, 230, 233},
  {"BLACK", 111, 89,  105}, 
  {"RED",   226, 124, 151}, 
  {"GREEN", 198, 229, 203}, 
  {"BLUE",  155, 191, 224}, 
  {"BROWN", 235, 218, 211}, 
};
const int numColors = sizeof(targets) / sizeof(targets[0]);

void setup() {
  pinMode(S0, OUTPUT); pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT); pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  
  pinMode(motor1pin1, OUTPUT); pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT); pinMode(motor2pin2, OUTPUT);
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);

  digitalWrite(S0, HIGH); digitalWrite(S1, LOW); // 20% Scaling
  
  Serial.begin(9600);
  Serial.print("--- Color Seeking System Ready ---");

  servoMarker.attach(11);
  servoMarker.write(currentPos);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);

}

void loop() {
  // Set speed (Lower this if the robot overshoots colors)
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  
  if (distance < 15) {
    moveRight();
    delay(1200);
    moveForward();
    delay(4000);
    moveLeft();
    delay(1200);
    moveForward();
    delay(5000);
    moveLeft();
    delay(600);
    moveForward();
    delay(5500);
  }
  

  while (getStableColor() == "WHITE") {
    analogWrite(ENA, 255); 
    analogWrite(ENB, 45);
    Serial.println("On Color: Moving Forward...");
    moveForward();
    turn = 0;
  }

  analogWrite(ENA, 255); 
  analogWrite(ENB, 255);

  moveRight();
  
  //Serial.println("Hit White. Searching...");

  // 3. IF STILL ON WHITE AFTER FULL SWEEP
  Serial.println("No color found in arc. Checking again...");
  delay(200);
}


void markercontrol(int targetPos, int speedDelay) {
  if (targetPos > currentPos) {
    for (int i = currentPos; i <= targetPos; i++) {
      servoMarker.write(i);
      delay(speedDelay); // Higher number = slower speed
    }
  } else {
    for (int i = currentPos; i >= targetPos; i--) {
      servoMarker.write(i);
      delay(speedDelay);
    }
  }
  currentPos = targetPos; // Update the global position
}


// --- MOVEMENT FUNCTIONS ---

void moveForward() {
  digitalWrite(motor1pin1, LOW); digitalWrite(motor1pin2, HIGH);
  digitalWrite(motor2pin1, LOW); digitalWrite(motor2pin2, HIGH);
}

void moveBack() {
  digitalWrite(motor1pin1, HIGH); digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, HIGH); digitalWrite(motor2pin2, LOW);
}

void moveLeft() {
  digitalWrite(motor1pin1, LOW); digitalWrite(motor1pin2, HIGH);
  digitalWrite(motor2pin1, HIGH); digitalWrite(motor2pin2, LOW);
}

void moveRight() {
  digitalWrite(motor1pin1, HIGH); digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, LOW); digitalWrite(motor2pin2, HIGH);
}

void stopMotors() {
  digitalWrite(motor1pin1, LOW); digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, LOW); digitalWrite(motor2pin2, LOW);
}

// --- SENSOR LOGIC (With Noise Filtering) ---

String getStableColor() {
  String c1 = readSensorRaw();
  delay(5); // Tiny gap to let electrical noise settle
  String c2 = readSensorRaw();
  
  if (c1 == c2) {
    return c1;
  }
  return "WHITE"; // If the sensor is flickering, assume it hasn't found a solid color yet
}

String readSensorRaw() {
  digitalWrite(S2, LOW); digitalWrite(S3, LOW);
  int r = constrain(map(pulseIn(sensorOut, LOW), 15, 260, 255, 0), 0, 255);
  digitalWrite(S2, HIGH); digitalWrite(S3, HIGH);
  int g = constrain(map(pulseIn(sensorOut, LOW), 16, 260, 255, 0), 0, 255);
  digitalWrite(S2, LOW); digitalWrite(S3, HIGH);
  int b = constrain(map(pulseIn(sensorOut, LOW), 14, 260, 255, 0), 0, 255);

  int closestIndex = 0;
  long minDistance = 1000000;
  for (int i = 0; i < numColors; i++) {
    long dist = pow(r - targets[i].r, 2) + pow(g - targets[i].g, 2) + pow(b - targets[i].b, 2);
    if (dist < minDistance) {
      minDistance = dist;
      closestIndex = i;
    }
  }
  //Serial.print("Target for "); Serial.print(targets[closestIndex].name);
  return targets[closestIndex].name;
}