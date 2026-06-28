
#include <WiFi.h>
#include <WebServer.h>

// ============ WiFi 与 Web 服务器配置 ============
const char* ssid = "ESP32_Dashboard";    // ESP32 发射的 WiFi 名称
const char* password = "12345678";       // WiFi 密码
WebServer server(80);

// ============ 引脚配置 ============
#define TOUCH_PIN 4  // 使用 GPIO4 (T0) 作为传感器数据采集引脚

// ============ 处理根目录 "/" (返回仪表盘网页) ============
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>作业9：实时传感器Web仪表盘</title>
  <style>
    body {
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      background-color: #121212; color: #e0e0e0;
      display: flex; flex-direction: column; align-items: center; justify-content: center;
      height: 100vh; margin: 0;
    }
    h1 { color: #00e5ff; margin-bottom: 10px; letter-spacing: 2px; }
    h2 { color: #888; font-weight: normal; margin-top: 0; font-size: 16px; }
    
    .dashboard {
      position: relative; width: 280px; height: 280px;
      border-radius: 50%;
      background: radial-gradient(circle, #1e1e1e 60%, #000 100%);
      box-shadow: 0 0 30px rgba(0, 229, 255, 0.2), inset 0 0 20px rgba(0,0,0,0.8);
      display: flex; flex-direction: column; align-items: center; justify-content: center;
      border: 4px solid #333;
      transition: border-color 0.3s, box-shadow 0.3s;
    }
    
    .value-display {
      font-size: 72px; font-weight: bold; color: #00e5ff;
      text-shadow: 0 0 15px rgba(0, 229, 255, 0.8);
      margin: 0; line-height: 1;
      transition: color 0.3s, text-shadow 0.3s;
    }
    .unit { font-size: 18px; color: #888; margin-top: 5px; letter-spacing: 1px; }
    .status-text { font-size: 14px; color: #aaa; margin-top: 15px; }
    
    .dashboard.alert {
      border-color: #ff1744;
      box-shadow: 0 0 40px rgba(255, 23, 68, 0.6), inset 0 0 20px rgba(0,0,0,0.8);
    }
    .dashboard.alert .value-display {
      color: #ff1744;
      text-shadow: 0 0 20px rgba(255, 23, 68, 0.9);
    }

    .progress-container {
      width: 80%; max-width: 300px; height: 8px;
      background-color: #333; border-radius: 4px; margin-top: 40px; overflow: hidden;
    }
    .progress-bar {
      height: 100%; width: 0%; background-color: #00e5ff;
      transition: width 0.1s ease-out, background-color 0.3s;
    }
    .progress-bar.alert { background-color: #ff1744; }

    .info { margin-top: 30px; font-size: 12px; color: #555; text-align: center; }
  </style>
</head>
<body>

  <h1>ESP32 Web服务器实验</h1>
  <h2>作业9：实时传感器Web仪表盘</h2>
  
  <div class="dashboard" id="dashBoard">
    <p class="value-display" id="sensorValue">--</p>
    <p class="unit">CAPACITANCE</p>
    <p class="status-text" id="statusText">等待数据...</p>
  </div>

  <div class="progress-container">
    <div class="progress-bar" id="progressBar"></div>
  </div>

  <div class="info">
    <p>数据采集引脚: GPIO4 (T0) | 刷新率: 10Hz (100ms)</p>
    <p>提示：请将手指靠近或触摸 GPIO4 引脚观察数据变化</p>
  </div>

  <script>
    const ALERT_THRESHOLD = 500; 
    const MAX_BASELINE = 3000; 
    
    // 【修复点2】增加请求锁，防止请求堆积
    let isFetching = false; 

    function fetchData() {
      // 如果上一个请求还没完成，直接跳过本次轮询
      if (isFetching) return; 
      isFetching = true;

      fetch('/touch')
        .then(response => {
          if (!response.ok) throw new Error('Network error');
          return response.text();
        })
        .then(val => {
          const numVal = parseInt(val);
          const valEl = document.getElementById('sensorValue');
          const dash = document.getElementById('dashBoard');
          const bar = document.getElementById('progressBar');
          const status = document.getElementById('statusText');

          valEl.textContent = numVal;

          let percent = 100 - (numVal / MAX_BASELINE * 100);
          if (percent < 0) percent = 0;
          if (percent > 100) percent = 100;
          bar.style.width = percent + '%';

          if (numVal < ALERT_THRESHOLD) {
            dash.classList.add('alert');
            bar.classList.add('alert');
            status.textContent = "⚠️ 检测到触摸入侵！";
          } else {
            dash.classList.remove('alert');
            bar.classList.remove('alert');
            status.textContent = "系统监控中...";
          }
        })
        .catch(err => {
          // 只有在真正断网时才显示 Err，避免频繁闪烁
          document.getElementById('sensorValue').textContent = "Err";
        })
        .finally(() => {
          // 请求结束，释放锁
          isFetching = false; 
        });
    }

    // 【修复点1】将轮询间隔从 20ms 调整为 100ms (10Hz)
    // 10Hz 对于人手动作已经足够流畅，且能确保 ESP32 稳定响应
    setInterval(fetchData, 100); 
    
    fetchData();
  </script>
</body>
</html>
)rawliteral";
  server.send(200, "text/html; charset=UTF-8", html);
}

// ============ 处理传感器数据上报 "/touch" ============
void handleTouch() {
  int touchValue = touchRead(TOUCH_PIN);
  server.send(200, "text/plain", String(touchValue));
}

void setup() {
  Serial.begin(115200);
  
  Serial.println("\n=== 作业9：实时传感器Web仪表盘 (网络优化版) ===");
  Serial.print("正在启动 WiFi 热点: ");
  Serial.println(ssid);
  
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  
  Serial.print("AP IP 地址: ");
  Serial.println(IP);
  Serial.print("请在浏览器输入网页地址: http://");
  Serial.println(IP);
  
  server.on("/", HTTP_GET, handleRoot);
  server.on("/touch", HTTP_GET, handleTouch); 
  
  server.begin();
  Serial.println("HTTP 服务器已启动，数据采集中...");
}

void loop() {
  server.handleClient(); 
}