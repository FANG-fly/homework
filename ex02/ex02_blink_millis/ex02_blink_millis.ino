// ex02_blink_millis.ino
// 使用millis()实现非阻塞1Hz LED闪烁

const int ledPin = 2;

unsigned long previousMillis = 0;
const long interval = 500;  // 500ms切换一次，实现1Hz（1秒一个周期）

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  Serial.println("Millis Blink Started");
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // 切换LED状态
    if (digitalRead(ledPin) == LOW) {
      digitalWrite(ledPin, HIGH);
      Serial.println("LED ON");
    } else {
      digitalWrite(ledPin, LOW);
      Serial.println("LED OFF");
    }
  }
}