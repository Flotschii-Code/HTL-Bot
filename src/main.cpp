#include <Arduino.h>
#include "drv8835.h"
#include <WiFi.h>
#include <WebServer.h>

const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>HTL-BOT Control</title>
  <style>
    body {
      font-family: Arial;
      text-align: center;
      padding: 20px;
    }

    .slider-container {
      display: inline-block;
      margin: 40px;
      height: 300px;
      width: 60px;
      position: relative;
    }

    input[type=range] {
      width: 250px;
      transform: rotate(-90deg);
      position: absolute;
      top: 120px;
      left: -95px;
    }

    label {
      display: block;
      margin-bottom: 10px;
    }
  </style>
</head>

<body>
  <h1>HTL-BOT Motorsteuerung</h1>
  <p><b>WASD</b> = fahren | Slider = manuell</p>

  <div class="slider-container">
    <label>Links<br><span id="leftVal">0</span></label>
    <input type="range" id="left" min="-255" max="255" value="0"
           oninput="updateMotors()">
  </div>

  <div class="slider-container">
    <label>Rechts<br><span id="rightVal">0</span></label>
    <input type="range" id="right" min="-255" max="255" value="0"
           oninput="updateMotors()">
  </div>

  <script>
  const SPEED = 180;

  function sendMotors(l, r) {
    document.getElementById('left').value  = l;
    document.getElementById('right').value = r;

    document.getElementById('leftVal').innerText  = l;
    document.getElementById('rightVal').innerText = r;

    fetch(`/motor?l=${l}&r=${r}`);
  }

  document.addEventListener('keydown', (e) => {
    if (e.repeat) return;

    switch (e.key.toLowerCase()) {
      case 'a':
        sendMotors(0, SPEED);
        break;

      case 'd':
        sendMotors(SPEED, 0);
        break;

      case 'w':
        sendMotors(SPEED, SPEED);
        break;
    }
  });

  document.addEventListener('keyup', (e) => {
    if (['a', 'd', 'w'].includes(e.key.toLowerCase())) {
      sendMotors(0, 0);
    }
  });
</script>


</body>
</html>
)rawliteral";


const char* ssid = "HTL-BOT";
const char* password = "12345678"; // min. 8 Zeichen

WebServer server(80);

void handle_motor() {
    int left  = server.arg("l").toInt();  
    int right = server.arg("r").toInt();

    motor_set_left(left);
    motor_set_right(right);

    server.send(200, "text/plain", "OK");
}


void setup() {
  Serial.begin(115200);
  motor_init();

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", []() {
  server.send(200, "text/html", htmlPage);
});

  server.on("/motor", handle_motor);
  server.begin();

  Serial.println("Webserver gestartet");


}

void loop() {
    server.handleClient();
}
