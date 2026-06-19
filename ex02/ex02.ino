// ex02.ino - Commit 1: 基础LED闪烁（delay版1Hz）

const int ledPin = 2;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  Serial.println("=== ex02 Commit 1: Basic Blink ===");
}

void loop() {
  digitalWrite(ledPin, HIGH);
  Serial.println("LED ON");
  delay(1000);
  
  digitalWrite(ledPin, LOW);
  Serial.println("LED OFF");
  delay(1000);
}