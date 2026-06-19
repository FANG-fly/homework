// ex03.ino - Commit 2: LED呼吸灯优化版

const int ledPin = 2;

const int freq = 1000;      // 降低PWM频率
const int resolution = 8;

void setup() {
  Serial.begin(115200);
  ledcAttach(ledPin, freq, resolution);
  Serial.println("=== ex03 Commit 2: Breathing Optimized ===");
}

void loop() {
  for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++) {
    ledcWrite(ledPin, dutyCycle);
    delay(5);  // 加快呼吸速度
  }
  
  for(int dutyCycle = 255; dutyCycle >= 0; dutyCycle--) {
    ledcWrite(ledPin, dutyCycle);
    delay(5);
  }
  
  Serial.println("Breathing cycle V2 completed - faster");
}