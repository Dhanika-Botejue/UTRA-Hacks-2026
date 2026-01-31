#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8

void setup() {
  pinMode(S0, OUTPUT); pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT); pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  digitalWrite(S0, HIGH); digitalWrite(S1, LOW); // 20% scaling

  Serial.begin(9600);
  Serial.println("--- COLOR CALIBRATOR ---");
  Serial.println("Type color name (e.g., 'red') to start 5-second sampling...");
}

void loop() {
  if (Serial.available() > 0) {
    String colorName = Serial.readStringUntil('\n');
    colorName.trim();

    Serial.print("Sampling for: ");
    Serial.println(colorName);
    Serial.println("Ready? 3... 2... 1...");
    delay(1000);

    long sumR = 0, sumG = 0, sumB = 0;
    int samples = 10;

    for (int i = 0; i < samples; i++) {
      // Read Red
      digitalWrite(S2, LOW); digitalWrite(S3, LOW);
      sumR += constrain(map(pulseIn(sensorOut, LOW), 15, 260, 255, 0), 0, 255);
      delay(100);

      // Read Green
      digitalWrite(S2, HIGH); digitalWrite(S3, HIGH);
      sumG += constrain(map(pulseIn(sensorOut, LOW), 16, 260, 255, 0), 0, 255);
      delay(100);

      // Read Blue
      digitalWrite(S2, LOW); digitalWrite(S3, HIGH);
      sumB += constrain(map(pulseIn(sensorOut, LOW), 14, 260, 255, 0), 0, 255);
      
      Serial.print("."); // Progress indicator
      delay(400); // 500ms total per sample cycle
    }

    int avgR = sumR / samples;
    int avgG = sumG / samples;
    int avgB = sumB / samples;

    Serial.println("\n--- CALIBRATION COMPLETE ---");
    Serial.print("Target for "); Serial.print(colorName);
    Serial.print(": {"); 
    Serial.print(avgR); Serial.print(", ");
    Serial.print(avgG); Serial.print(", ");
    Serial.print(avgB); Serial.println("}");
    Serial.println("Copy these values into your ColorTarget array.\n");
    Serial.println("Waiting for next color name...");
  }
}
