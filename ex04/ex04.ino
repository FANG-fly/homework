/*
 * 作业4：基于触摸传感器的"自锁"开关
 * 开发板：ESP32 Dev Module
 * IDE：Arduino IDE 2.3.9
 *
 * 功能：摸一下触摸引脚，LED亮起并保持长亮；
 *       松开手再摸一下，LED熄灭。
 *
 * 接线：
 *   - 触摸引脚：T0 (GPIO4)
 *   - LED引脚：GPIO2 (ESP32开发板自带LED)
 */

// ============ 引脚定义 ============
#define TOUCH_PIN    T0    // 触摸引脚 (GPIO4)
#define LED_PIN      2     // LED引脚 (板载LED)

// ============ 状态变量 ============
bool ledState = false;          // LED当前状态：true=亮, false=灭
bool lastTouchState = false;    // 上一次的触摸状态：true=被触摸, false=未触摸

// ============ 防抖参数 ============
const int touchThreshold = 500;  // 触摸阈值（根据实际开发板调整）
const unsigned long debounceDelay = 200; // 防抖延时(毫秒)
unsigned long lastDebounceTime = 0;      // 上次触发时间

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  // 初始状态：LED熄灭
  Serial.println("=== 作业4：触摸自锁开关 ===");
  Serial.println("触摸引脚(GPIO4)切换LED状态");
}

void loop() {
  // 1. 读取触摸传感器值
  int touchValue = touchRead(TOUCH_PIN);
  
  // 2. 判断当前是否被触摸（触摸值越小表示越接近/触摸）
  bool currentTouchState = (touchValue < touchThreshold);
  
  // 3. 边缘检测：检测"按下瞬间"
  //    条件：上一次未触摸 AND 当前被触摸 → 这就是按下瞬间
  if (currentTouchState == true && lastTouchState == false) {
    
    // 4. 软件防抖：距离上次触发是否超过防抖时间
    unsigned long currentTime = millis();
    if (currentTime - lastDebounceTime > debounceDelay) {
      lastDebounceTime = currentTime;
      
      // 5. 翻转LED状态
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);
      
      Serial.print("触摸触发！LED状态: ");
      Serial.println(ledState ? "ON" : "OFF");
    }
  }
  
  // 6. 更新触摸状态（用于下一次循环的边缘检测）
  lastTouchState = currentTouchState;
  
  // 调试输出（可选，方便观察触摸值）
  Serial.print("Touch Value: ");
  Serial.println(touchValue);
  
  delay(50); // 短暂延时，避免串口输出过快
}