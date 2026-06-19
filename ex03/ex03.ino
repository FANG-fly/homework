// ex03.ino - Commit 3: 使用millis()实现非阻塞SOS求救信号

const int ledPin = 2;

unsigned long previousMillis = 0;
int step = 0;
bool ledState = false;

const int timings[] = {
  200, -200,
  200, -200,
  200, -500,
  
  600, -200,
  600, -200,
  600, -500,
  
  200, -200,
  200, -200,
  200, -2000
};

const int totalSteps = sizeof(timings) / sizeof(timings[0]);

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  Serial.println("=== ex03 Commit 3: SOS with millis() ===");
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= abs(timings[step])) {
    previousMillis = currentMillis;
    
    step++;
    if (step >= totalSteps) {
      step = 0;
      Serial.println("SOS cycle completed");
    }
    
    ledState = (timings[step] > 0);
    digitalWrite(ledPin, ledState ? HIGH : LOW);
  }
}