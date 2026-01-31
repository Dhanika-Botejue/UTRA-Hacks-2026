// Define pins for TCS3200/TCS230
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8

// Variables for raw frequency and mapped RGB values
int redFreq, greenFreq, blueFreq;
int r, g, b;

void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  // Set Frequency scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  Serial.begin(9600);
  Serial.println("--- Color Sensor Ready ---");
}

void loop() {
  // 1. Read RED frequency
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  redFreq = pulseIn(sensorOut, LOW);
  // Map raw values (13 = white/255, 148 = dark/0)
  r = map(redFreq, 13, 148, 255, 0);
  r = constrain(r, 0, 255); // Keep within 0-255 range
  delay(100);

  // 2. Read GREEN frequency
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  greenFreq = pulseIn(sensorOut, LOW);
  // Map raw values (13 = white/255, 170 = dark/0)
  g = map(greenFreq, 13, 170, 255, 0);
  g = constrain(g, 0, 255);
  delay(100);

  // 3. Read BLUE frequency
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  blueFreq = pulseIn(sensorOut, LOW);
  // Map raw values (13 = white/255, 150 = dark/0)
  b = map(blueFreq, 13, 150, 255, 0);
  b = constrain(b, 0, 255);
  delay(100);

  // --- PRINT RGB VALUES ---
  Serial.print("RGB: (");
  Serial.print(r); Serial.print(", ");
  Serial.print(g); Serial.print(", ");
  Serial.print(b); Serial.print(") ");

  // --- CALIBRATED COLOR GATE ---

  // 1. BLACK: Nearly zero light (Your data: 34, 38, 21)
  if (r < 60 && g < 60 && b < 60) {
    Serial.println("-> DETECTED: BLACK");
  }

  // 2. NOTHING: Ambient light/Empty air (Your data: 170, 106, 73)
  // We catch this specifically so it doesn't get called "Red" or "Brown"
  else if (r > 150 && r < 185 && g < 120 && b < 90) {
    Serial.println("-> DETECTED: NOTHING");
  }

  // 3. WHITE: Full blast (Your data: 250, 250, 250)
  else if (r > 240 && g > 240 && b > 240) {
    Serial.println("-> DETECTED: WHITE");
  }

  // 4. BROWN: Bright but mixed (Your data: 212, 190, 170)
  // Brown has high Green and Blue compared to pure Red.
  else if (r > 200 && g > 170 && b > 150) {
    Serial.println("-> DETECTED: BROWN");
  }

  // 5. RED: Pure and high (Your data: 238, 124, 140)
  // Notice Green is MUCH lower here than in Brown.
  else if (r > 220 && g < 150) {
    Serial.println("-> DETECTED: RED");
  }

  // 6. GREEN: (Your data: 130, 212, 150)
  else if (g > r && g > b) {
    Serial.println("-> DETECTED: GREEN");
  }

  // 7. BLUE: (Your data: 75, 152, 200)
  else if (b > r && b > g) {
    Serial.println("-> DETECTED: BLUE");
  }

  else {
    Serial.println("-> DETECTED: UNKNOWN");
  }

  delay(500);
}

