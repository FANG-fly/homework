// ex02.ino - Commit 2: 使用millis()实现非阻塞1Hz闪烁

const int ledPin = 2;

unsigned long previousMillis = 0;
const long interval = 500;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  Serial.println("=== ex02 Commit 2: Millis Blink ===");
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    if (digitalRead(ledPin) == LOW) {
      digitalWrite(ledPin, HIGH);
      Serial.println("LED ON");
    } else {
      digitalWrite(ledPin, LOW);
      Serial.println("LED OFF");
    }
  }
}