

// ============ 引脚与PWM参数定义 ============
#define TOUCH_PIN    T0    // 触摸引脚 (GPIO4)
#define LED_PIN      2     // LED引脚 (板载LED)
#define PWM_FREQ     5000  // PWM频率
#define PWM_RES      8     // PWM分辨率 (8位: 0-255)

// ============ 状态与手势变量 ============
int currentGear = 0;            // 当前档位：0=关, 1=慢, 2=中, 3=快
bool lastTouchState = false;    // 上一次触摸状态
bool isTouching = false;        // 当前是否正在触摸
unsigned long touchStartTime = 0; // 记录按下瞬间的时间戳

// ============ 防抖与阈值参数 ============
const int touchThreshold = 500;          // 触摸阈值
const unsigned long debounceDelay = 200; // 短按防抖延时
unsigned long lastDebounceTime = 0;      
const unsigned long longPressTime = 800; // 长按判定时间 (毫秒)

// ============ 呼吸灯非阻塞控制变量 ============
int brightness = 0;             // 当前亮度 (0-255)
int fadeAmount = 5;             // 每次亮度变化步长
bool fadingUp = true;           // 亮度变化方向：true=变亮, false=变暗
unsigned long lastFadeTime = 0; // 上次更新亮度的时间

void setup() {
  Serial.begin(115200);
  
  // 初始化PWM (ESP32 Core 3.x 新版API)
  ledcAttach(LED_PIN, PWM_FREQ, PWM_RES);
  ledcWrite(LED_PIN, 0); // 初始全灭
  
  Serial.println("=== 作业5：多档位触摸调速呼吸灯 ===");
  Serial.println("短按切换档位(慢/中/快)，长按(>0.8s)关闭");
}

void loop() {
  // ================= 1. 触摸手势检测 =================
  int touchValue = touchRead(TOUCH_PIN);
  bool currentTouchState = (touchValue < touchThreshold);
  
  // 检测"按下瞬间" (边缘检测)
  if (currentTouchState == true && lastTouchState == false) {
    unsigned long currentTime = millis();
    if (currentTime - lastDebounceTime > debounceDelay) {
      lastDebounceTime = currentTime;
      isTouching = true;
      touchStartTime = currentTime; // 记录按下时间
    }
  }
  
  // 检测"松开瞬间"
  if (currentTouchState == false && lastTouchState == true && isTouching == true) {
    isTouching = false;
    unsigned long pressDuration = millis() - touchStartTime;
    
    if (pressDuration >= longPressTime) {
      // 【长按】关闭LED
      currentGear = 0;
      Serial.println(">> 长按检测：关闭呼吸灯");
    } else {
      // 【短按】切换档位 (1->2->3->1)
      currentGear++;
      if (currentGear > 3) currentGear = 1;
      Serial.print(">> 短按检测：切换至档位 ");
      Serial.println(currentGear);
    }
  }
  
  // 更新触摸状态
  lastTouchState = currentTouchState;

  // ================= 2. 非阻塞呼吸灯执行 =================
  executeBreathingLED();

  delay(10); // 极短延时，稳定循环
}

// ================= 呼吸灯执行函数 =================
void executeBreathingLED() {
  unsigned long currentTime = millis();
  unsigned long interval = 0; // 呼吸速度间隔
  
  // 根据档位设定速度 (数值越小呼吸越快)
  switch (currentGear) {
    case 1: interval = 30; break; // 慢速
    case 2: interval = 15; break; // 中速
    case 3: interval = 5;  break; // 快速
    case 0: // 关闭状态
      brightness = 0;
      ledcWrite(LED_PIN, 0);
      return; 
  }
  
  // 非阻塞时间判断：只有达到间隔时间才更新亮度
  if (currentTime - lastFadeTime >= interval) {
    lastFadeTime = currentTime;
    
    if (fadingUp) {
      brightness += fadeAmount;
      if (brightness >= 255) {
        brightness = 255;
        fadingUp = false;
      }
    } else {
      brightness -= fadeAmount;
      if (brightness <= 0) {
        brightness = 0;
        fadingUp = true;
      }
    }
    
    ledcWrite(LED_PIN, brightness);
  }
}