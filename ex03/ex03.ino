// ex03.ino - Commit 1: LED呼吸灯基础版

const int ledPin = 2;

const int freq = 5000;
const int resolution = 8;

void setup() {
  Serial.begin(115200);
  ledcAttach(ledPin, freq, resolution);
  Serial.println("=== ex03 Commit 1: Breathing Basic ===");
}

void loop() {
  for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++) {
    ledcWrite(ledPin, dutyCycle);
    delay(10);
  }
  
  for(int dutyCycle = 255; dutyCycle >= 0; dutyCycle--) {
    ledcWrite(ledPin, dutyCycle);
    delay(10);
  }
  
  Serial.println("Breathing cycle completed");
}