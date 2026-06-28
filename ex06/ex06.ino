

// ============ 引脚与PWM参数定义 ============
#define LED_A_PIN    4     // 灯A引脚
#define LED_B_PIN    16     // 灯B引脚
#define PWM_FREQ     5000  // PWM频率
#define PWM_RES      8     // PWM分辨率 (8位: 0-255)

// ============ 呼吸灯控制变量 ============
int brightness = 0;         // 当前灯A的亮度 (0-255)
int fadeAmount = 5;         // 每次亮度变化步长
bool fadingUp = true;       // 亮度变化方向：true=变亮, false=变暗

// ============ 非阻塞时间控制 ============
unsigned long lastFadeTime = 0;
const unsigned long fadeInterval = 20; // 呼吸速度间隔(ms)，数值越小渐变越快

void setup() {
  Serial.begin(115200);
  
  // 初始化双通道PWM (ESP32 Core 3.x 新版API)
  // ledcAttach 会自动为每个引脚分配独立的 PWM 通道
  ledcAttach(LED_A_PIN, PWM_FREQ, PWM_RES);
  ledcAttach(LED_B_PIN, PWM_FREQ, PWM_RES);
  
  Serial.println("=== 作业6：警车双闪灯效（双通道PWM） ===");
}

void loop() {
  unsigned long currentTime = millis();
  
  // 非阻塞式更新亮度
  if (currentTime - lastFadeTime >= fadeInterval) {
    lastFadeTime = currentTime;
    
    // 1. 更新灯A亮度
    if (fadingUp) {
      brightness += fadeAmount;
      if (brightness >= 255) {
        brightness = 255;
        fadingUp = false; // 达到最亮，准备变暗
      }
    } else {
      brightness -= fadeAmount;
      if (brightness <= 0) {
        brightness = 0;
        fadingUp = true;  // 达到全灭，准备变亮
      }
    }
    
    // 2. 计算灯B亮度（反相逻辑：255 - 灯A亮度）
    int brightnessB = 255 - brightness;
    
    // 3. 输出双通道PWM
    ledcWrite(LED_A_PIN, brightness);
    ledcWrite(LED_B_PIN, brightnessB);
  }
}