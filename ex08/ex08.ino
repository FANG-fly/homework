

#include <WiFi.h>
#include <WebServer.h>

// ============ WiFi 与 Web 服务器配置 ============
const char* ssid = "ESP32_Security";     // ESP32 发射的 WiFi 名称
const char* password = "12345678";       // WiFi 密码
WebServer server(80);

// ============ 引脚配置 ============
#define TOUCH_PIN   4     // 使用 GPIO4 (对应触摸通道 T0) 模拟入侵传感器
#define ALARM_PIN   2     // 模拟报警灯 (板载 LED)

// ============ 触摸阈值配置 ============
// ESP32 触摸原理：未触摸时数值较大，手指触摸时数值急剧变小。
// 当读取到的值 < TOUCH_THRESHOLD 时，判定为"被触摸(入侵)"
int TOUCH_THRESHOLD = 500; //

// ============ 系统状态定义 ============
// 0: 撤防 (Disarmed), 1: 布防 (Armed), 2: 报警 (Alarming)
int systemState = 0;      

// ============ 处理根目录 "/" (返回网页) ============
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>作业8：物联网安防报警器</title>
  <style>
    body { font-family: Arial; text-align: center; margin-top: 50px; transition: background-color 0.5s; }
    .status-box { padding: 20px; border-radius: 10px; margin: 20px auto; width: 80%; max-width: 400px; font-size: 24px; font-weight: bold; color: white; }
    .disarmed { background-color: #4CAF50; } 
    .armed { background-color: #ff9800; }    
    .alarming { background-color: #f44336; animation: blink 0.5s infinite; } 
    @keyframes blink { 50% { opacity: 0.5; } }
    button { padding: 15px 30px; font-size: 18px; margin: 10px; border: none; border-radius: 5px; cursor: pointer; color: white; }
    .btn-arm { background-color: #ff9800; }
    .btn-disarm { background-color: #4CAF50; }
    .btn-clear { background-color: #2196F3; }
    p { color: #666; font-size: 14px; }
    .pin-info { color: #999; font-size: 12px; margin-top: 20px; }
  </style>
</head>
<body>
  <h1>ESP32 Web服务器实验</h1>
  <h2>作业8：物联网安防报警器</h2>
  
  <div id="statusBox" class="status-box disarmed">🟢 已撤防 (安全)</div>
  
  <div>
    <button class="btn-arm" onclick="sendCmd('arm')">🔒 一键布防</button>
    <button class="btn-disarm" onclick="sendCmd('disarm')">🔓 一键撤防</button>
    <button class="btn-clear" onclick="sendCmd('clear')" id="clearBtn" style="display:none;">🛑 解除报警</button>
  </div>
  
  <p>提示：布防后，请用手指 <b>触摸 ESP32 的 GPIO4 引脚</b> 模拟入侵触发。</p>
  <p class="status">网页状态自动刷新中...</p>
  <p class="pin-info">* 传感器引脚: GPIO4 (T0) | 报警灯: GPIO2 (板载LED)</p>

  <script>
    function sendCmd(cmd) {
      fetch('/cmd?action=' + cmd).then(() => {
        setTimeout(updateStatus, 100); 
      });
    }

    function updateStatus() {
      fetch('/status').then(response => response.text()).then(state => {
        const box = document.getElementById('statusBox');
        const clearBtn = document.getElementById('clearBtn');
        
        box.className = 'status-box'; 
        
        if (state === '0') {
          box.classList.add('disarmed');
          box.innerHTML = '🟢 已撤防 (安全)';
          clearBtn.style.display = 'none';
          document.body.style.backgroundColor = 'white';
        } else if (state === '1') {
          box.classList.add('armed');
          box.innerHTML = '🟡 布防中 (警戒...)';
          clearBtn.style.display = 'none';
          document.body.style.backgroundColor = 'white';
        } else if (state === '2') {
          box.classList.add('alarming');
          box.innerHTML = '🔴 警报！！！检测到入侵！';
          clearBtn.style.display = 'inline-block'; 
          document.body.style.backgroundColor = '#ffebee'; 
        }
      }).catch(err => console.log('状态获取失败'));
    }

    setInterval(updateStatus, 1000);
    updateStatus();
  </script>
</body>
</html>
)rawliteral";
  server.send(200, "text/html; charset=UTF-8", html);
}

// ============ 处理控制指令 "/cmd" ============
void handleCmd() {
  if (server.hasArg("action")) {
    String action = server.arg("action");
    if (action == "arm") {
      systemState = 1; 
      Serial.println("收到指令：系统布防");
    } else if (action == "disarm") {
      systemState = 0; 
      digitalWrite(ALARM_PIN, LOW); 
      Serial.println("收到指令：系统撤防");
    } else if (action == "clear") {
      systemState = 0; 
      digitalWrite(ALARM_PIN, LOW);
      Serial.println("收到指令：解除报警");
    }
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}

// ============ 处理状态查询 "/status" ============
void handleStatus() {
  server.send(200, "text/plain", String(systemState));
}

void setup() {
  Serial.begin(115200);
  
  // 1. 初始化引脚
  pinMode(ALARM_PIN, OUTPUT);
  digitalWrite(ALARM_PIN, LOW);
  
  // 2. 配置 AP 模式
  Serial.println("\n=== 作业8：物联网安防报警器 (触摸版) ===");
  Serial.print("正在启动 WiFi 热点: ");
  Serial.println(ssid);
  WiFi.softAP(ssid, password);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP 地址: ");
  Serial.println(IP);
  
  // 输出完整的网页访问地址
  Serial.print("请在浏览器输入网页地址: http://");
  Serial.println(IP);
  
  // 3. 配置路由
  server.on("/", HTTP_GET, handleRoot);
  server.on("/cmd", HTTP_GET, handleCmd);
  server.on("/status", HTTP_GET, handleStatus);
  
  // 4. 启动服务器
  server.begin();
  Serial.println("HTTP 服务器已启动，等待操作...");
  
  // 5. 触摸传感器校准提示
  Serial.println("\n--- 触摸传感器校准 ---");
  Serial.println("请在 3 秒内不要触摸 GPIO4 引脚...");
  delay(3000);
  int baseValue = touchRead(TOUCH_PIN);
  Serial.print("环境基准触摸值: ");
  Serial.println(baseValue);
  Serial.print("当前触发阈值设定为: < ");
  Serial.println(TOUCH_THRESHOLD);
  Serial.println("------------------------\n");
}

void loop() {
  server.handleClient(); // 处理 Web 请求
  
  // ============ 核心硬件逻辑：触摸检测与报警 ============
  if (systemState == 1) {
    int touchValue = touchRead(TOUCH_PIN);
    
    // 当手指触摸 GPIO4 时，数值会急剧下降
    if (touchValue < TOUCH_THRESHOLD) {
      systemState = 2; // 触发报警！
      Serial.print("⚠️ 警告：检测到触摸入侵！触摸值: ");
      Serial.println(touchValue);
    }
  }
  
  // 如果系统处于“报警状态 (2)”
  if (systemState == 2) {
    digitalWrite(ALARM_PIN, !digitalRead(ALARM_PIN)); 
    delay(100); 
  }
}