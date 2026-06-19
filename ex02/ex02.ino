// ex02.ino - Commit 3: 使用delay()实现SOS求救信号

const int ledPin = 2;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  Serial.println("=== ex02 Commit 3: SOS with delay ===");
}

void loop() {
  // S: 短闪3次
  for(int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
  }
  delay(500);  // 字母间隔
  
  // O: 长闪3次
  for(int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);
    delay(600);
    digitalWrite(ledPin, LOW);
    delay(200);
  }
  delay(500);  // 字母间隔
  
  // S: 短闪3次
  for(int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
  }
  delay(2000);  // 单词间隔
  
  Serial.println("SOS cycle completed");
}