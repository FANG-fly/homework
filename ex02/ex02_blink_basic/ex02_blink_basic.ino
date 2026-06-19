// ex02_blink_basic.ino
// 基础LED闪烁 - 使用delay实现1Hz

const int ledPin = 2;  // ESP32板载LED在GPIO2

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  Serial.println("Basic Blink Started");
}

void loop() {
  digitalWrite(ledPin, HIGH);
  Serial.println("LED ON");
  delay(1000);  // 亮1秒
  
  digitalWrite(ledPin, LOW);
  Serial.println("LED OFF");
  delay(1000);  // 灭1秒
}