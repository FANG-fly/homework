
#include <WiFi.h>
#include <WebServer.h>

// ============ WiFi 与 Web 服务器配置 ============
const char* ssid = "ESP32_Dimmer";  // 热点名
const char* password = "12345678";
WebServer server(80);

// ============ 引脚与PWM参数 ============
#define LED_PIN      2
#define PWM_FREQ     5000
#define PWM_RES      8

// ============ 服务器端请求限流 ============
unsigned long lastRequestTime = 0;
const unsigned long MIN_REQUEST_INTERVAL = 10; // 最小请求间隔 (ms)

// ============ 处理根目录 "/" ============
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>作业7：无极调光器</title>
  <style>
    .slider { width: 80%; height: 25px; margin: 20px 0; cursor: pointer; }
    #val { font-size: 28px; color: #4CAF50; font-weight: bold; }
    .status { color: #666; font-size: 14px; }
  </style>
</head>
<body style="font-family:Arial; text-align:center; margin-top:50px;">
  <h1>ESP32 Web服务器实验</h1>
  <h2>作业7：优化版无极调光器</h2>
  <p>当前亮度: <span id="val">0</span> / 255</p>
  <input type="range" min="0" max="255" value="0" class="slider" id="mySlider">
  <p class="status">状态: <span id="status">就绪</span></p>
  
  <script>
    // ============ 客户端防抖 (关键优化) ============
    let debounceTimer;
    const DEBOUNCE_TIME = 50; // 50ms 防抖时间
    
    function sendBrightness(value) {
      // 更新显示
      document.getElementById('val').textContent = value;
      
      // 防抖：清除之前的定时器
      clearTimeout(debounceTimer);
      
      // 设置新的定时器
      debounceTimer = setTimeout(() => {
        // 实际发送请求
        fetch('/set?brightness=' + value)
          .then(response => {
            if (response.ok) {
              document.getElementById('status').textContent = '已更新';
              setTimeout(() => {
                document.getElementById('status').textContent = '就绪';
              }, 500);
            }
          })
          .catch(error => {
            document.getElementById('status').textContent = '连接错误';
          });
      }, DEBOUNCE_TIME);
    }

    // 监听滑动条事件 (优化版)
    const slider = document.getElementById("mySlider");
    slider.addEventListener('input', function() {
      sendBrightness(this.value);
    });

    // 确保拖动结束时发送最终值
    slider.addEventListener('change', function() {
      sendBrightness(this.value);
    });
  </script>
</body>
</html>
)rawliteral";
  
  server.send(200, "text/html; charset=UTF-8", html);
}

// ============ 处理 "/set" 请求 (带限流) ============
void handleSet() {
  // ============ 服务器端请求限流  ============
  unsigned long now = millis();
  if (now - lastRequestTime < MIN_REQUEST_INTERVAL) {
    server.send(200, "text/plain", "Throttled");
    return;
  }
  lastRequestTime = now;

  // 原有逻辑不变
  if (server.hasArg("brightness")) {
    int brightness = server.arg("brightness").toInt();
    brightness = constrain(brightness, 0, 255);
    
    ledcWrite(LED_PIN, brightness);
    Serial.print("收到请求，设置亮度: ");
    Serial.println(brightness);
    
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}

void setup() {
  Serial.begin(115200);
  
  // 1. 初始化 PWM
  ledcAttach(LED_PIN, PWM_FREQ, PWM_RES);
  ledcWrite(LED_PIN, 0);
  
  // 2. 配置 AP 模式
  Serial.println("\n=== 作业7：优化版无极调光器 ===");
  Serial.println("正在启动 WiFi 热点: " + String(ssid));
  WiFi.softAP(ssid, password);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP 地址: ");
  Serial.println(IP);
  Serial.println("请连接该WiFi，然后在浏览器输入上述IP地址");
  
  // 3. 配置服务器路由
  server.on("/", HTTP_GET, handleRoot);
  server.on("/set", HTTP_GET, handleSet);
  
  // 4. 启动服务器
  server.begin();
  Serial.println("HTTP 服务器已启动");
}

void loop() {
  server.handleClient();
}