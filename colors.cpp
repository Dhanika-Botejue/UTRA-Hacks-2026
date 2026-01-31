// Define pins for TCS3200/TCS230
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8

// --- COLOR DICTIONARY ---
// Replace these {R, G, B} values with the ones from your Calibration Script
struct ColorTarget {
  const char* name;
  int r, g, b;
};

ColorTarget targets[] = {
  {"WHITE",   252, 251, 251},
  {"BLACK",   111, 89,  105}, 
  {"RED",     243, 156, 176}, 
  {"GREEN",   198, 229, 203}, 
  {"BLUE",    155, 191, 224}, 
  {"BROWN",   235, 218, 211}, 
};

const int numColors = sizeof(targets) / sizeof(targets[0]);

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
  Serial.println("--- Vector Distance Color Recognition Ready ---");
}

void loop() {
  // 1. READ AND MAP SENSOR VALUES
  digitalWrite(S2, LOW); digitalWrite(S3, LOW);
  int r = constrain(map(pulseIn(sensorOut, LOW), 15, 260, 255, 0), 0, 255);
  delay(20);

  digitalWrite(S2, HIGH); digitalWrite(S3, HIGH);
  int g = constrain(map(pulseIn(sensorOut, LOW), 16, 260, 255, 0), 0, 255);
  delay(20);

  digitalWrite(S2, LOW); digitalWrite(S3, HIGH);
  int b = constrain(map(pulseIn(sensorOut, LOW), 14, 260, 255, 0), 0, 255);
  delay(20);

  // 2. FIND THE CLOSEST VECTOR (Euclidean Distance)
  int closestIndex = 0;
  long minDistance = 1000000; // Initialize with a very high value

  for (int i = 0; i < numColors; i++) {
    // We calculate the squared distance: (r1-r2)^2 + (g1-g2)^2 + (b1-b2)^2
    // It's faster than using sqrt() and gives the same comparison result.
    long dist = pow(r - targets[i].r, 2) + 
                pow(g - targets[i].g, 2) + 
                pow(b - targets[i].b, 2);

    if (dist < minDistance) {
      minDistance = dist;
      closestIndex = i;
    }
  }

  // 3. PRINT RESULTS
  Serial.print("Current RGB: (");
  Serial.print(r); Serial.print(", ");
  Serial.print(g); Serial.print(", ");
  Serial.print(b); Serial.print(") ");

  Serial.print("-> MATCH: ");
  Serial.println(targets[closestIndex].name);

  delay(1000); // Check every 1 second
}
