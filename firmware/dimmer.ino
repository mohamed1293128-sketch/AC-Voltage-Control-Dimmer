#include <WiFi.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include <U8g2lib.h>
#include <Preferences.h>
#include <ArduinoOTA.h>
#include <ESPmDNS.h>

// WEBSERVER
WebServer server(80);

// PINS
#define ZERO_CROSS_PIN   27
#define TRIAC_GATE_PIN   26
#define LED_PIN          2

// OLED PIN SETUP
U8G2_SH1106_128X64_NONAME_F_4W_SW_SPI u8g2(
  U8G2_R0,
  18, // SCK
  23, // SDA
  U8X8_PIN_NONE, // CS
  5, // DC
  4 // RST
);

// PREFERENCES VARIABLE
Preferences prefs;

// DIMMER SETUP
#define MAX_DELAY_US   8500
#define MIN_DELAY_US   500
#define GATE_PULSE_US  300

volatile uint16_t firingDelay_us = 8500;
volatile bool triacEnabled = true;
volatile int currentPercent = 50;

// TIMER
hw_timer_t *triacTimer = NULL;

// OLED CONTROL
unsigned long lastOLED = 0;

// WIFI STATUS
String wifiStatus = "Starting";
String wifiSSID = "";
bool wifiConnected = false;

// WIFI ICON ON OLED
void drawWiFiIcon(int x, int y, int strength) {

  if (strength > -90) {
    u8g2.drawDisc(x, y, 1);
  }

  if (strength > -80) {
    u8g2.drawArc(x, y, 4, 45, 135);
  }

  if (strength > -70) {
    u8g2.drawArc(x, y, 7, 45, 135);
  }

  if (strength > -60) {
    u8g2.drawArc(x, y, 10, 45, 135);
  }
}

// TRIAC FIRE ISR
void IRAM_ATTR fireTriacISR() {

  if (!triacEnabled) return;

  digitalWrite(TRIAC_GATE_PIN, HIGH);

  ets_delay_us(GATE_PULSE_US);

  digitalWrite(TRIAC_GATE_PIN, LOW);

  timerStop(triacTimer);
}

// ZERO CROSS ISR
void IRAM_ATTR zeroCrossISR() {

  static uint32_t lastCross = 0;

  uint32_t now = micros();

  // Noise filter
  if (now - lastCross < 7000) return;

  lastCross = now;

  if (!triacEnabled) return;

  timerWrite(triacTimer, 0);

  timerAlarm(
    triacTimer,
    firingDelay_us,
    false,
    0
  );

  timerStart(triacTimer);
}

// SET DIM LEVEL
void setDimPercent(int percent) {

  percent = constrain(percent, 0, 100);

  currentPercent = percent;

  if (percent <= 0) {

    triacEnabled = false;

    firingDelay_us = MAX_DELAY_US;

    digitalWrite(TRIAC_GATE_PIN, LOW);
  }

  else {

    triacEnabled = true;

    firingDelay_us = map(
                        percent,
                        1,
                        100,
                        8500,
                        500
                      );

    firingDelay_us = constrain(
                        firingDelay_us,
                        MIN_DELAY_US,
                        MAX_DELAY_US
                      );
  }

  prefs.putInt("brightness", percent);
}

// OLED UPDATE
void updateOLED() {

  static int lastShown = -1;
  static String lastStatus = "";

  if (
    lastShown == currentPercent &&
    lastStatus == wifiStatus
  ) return;

  lastShown = currentPercent;
  lastStatus = wifiStatus;

  u8g2.clearBuffer();

  // TOP BAR
  u8g2.setFont(u8g2_font_6x12_tf);

  u8g2.setCursor(0, 10);
  u8g2.print(wifiStatus);

  if (wifiConnected) {
    drawWiFiIcon(118, 10, WiFi.RSSI());
  }

  // PERCENTAGE
  u8g2.setFont(u8g2_font_logisoso28_tf);

  int textX = 18;

  if (currentPercent >= 100)
    textX = 2;
  else if (currentPercent >= 10)
    textX = 18;
  else
    textX = 34;

  u8g2.setCursor(textX, 42);

  u8g2.print(currentPercent);
  u8g2.print("%");

  // BRIGHTNESS BAR
  int barWidth = map(
                   currentPercent,
                   0,
                   100,
                   0,
                   120
                 );

  u8g2.drawFrame(4, 50, 120, 10);
  u8g2.drawBox(4, 50, barWidth, 10);

  u8g2.sendBuffer();
}

// WEB PAGE
void handleRoot() {

  String page = R"rawliteral(

<!DOCTYPE html>
<html>
<head>

<meta name="viewport"
      content="width=device-width, initial-scale=1">

<style>

body{
  margin:0;
  padding:0;
  font-family:Arial;
  background:linear-gradient(135deg,#0f0f0f,#1d1d1d);
  color:white;
  text-align:center;
}

.container{
  margin-top:30px;
}

h1{
  font-size:42px;
  margin-bottom:20px;
}

#valueText{
  font-size:72px;
  font-weight:bold;
  margin:20px;
}

.slider{
  width:85%;
  height:18px;
  appearance:none;
  border-radius:20px;
  background:#333;
  outline:none;
}

.slider::-webkit-slider-thumb{
  appearance:none;
  width:35px;
  height:35px;
  border-radius:50%;
  background:white;
}

.buttons{
  margin-top:30px;
}

button{
  width:120px;
  height:70px;
  margin:10px;
  border:none;
  border-radius:18px;
  background:#2a2a2a;
  color:white;
  font-size:24px;
  transition:0.2s;
}

button:hover{
  transform:scale(1.05);
  background:#444;
}

.card{
  width:90%;
  max-width:500px;
  margin:auto;
  margin-top:40px;
  padding:25px;
  border-radius:25px;
  background:rgba(255,255,255,0.05);
  backdrop-filter:blur(12px);
  box-shadow:0 0 25px rgba(255,255,255,0.08);
}

</style>
</head>

<body>

<div class="container">
<div class="card">

<h1>ESP32 DIMMER</h1>

<div id="valueText">)rawliteral" + String(currentPercent) + R"rawliteral(%</div>

<input type="range"
       min="0"
       max="100"
       value=")rawliteral" + String(currentPercent) + R"rawliteral("
       class="slider"
       id="slider"
       oninput="updateSlider(this.value)">

<div class="buttons">

<button onclick="sendVal(0)">0%</button>
<button onclick="sendVal(25)">25%</button>
<button onclick="sendVal(50)">50%</button>
<button onclick="sendVal(75)">75%</button>
<button onclick="sendVal(100)">100%</button>

</div>
</div>
</div>

<script>

function updateSlider(val){

  document.getElementById("valueText").innerHTML =
    val + "%";

  fetch("/set?value=" + val);
}

function sendVal(val){

  document.getElementById("slider").value = val;

  document.getElementById("valueText").innerHTML =
    val + "%";

  fetch("/set?value=" + val);
}

</script>

</body>
</html>

)rawliteral";

  server.send(200, "text/html", page);
}

// HANDLE SET
void handleSet() {

  int val =
    server.arg("value").toInt();

  setDimPercent(val);

  server.send(
    200,
    "text/plain",
    "OK"
  );
}

// SETUP
void setup() {

  Serial.begin(115200);

  // PINS
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  pinMode(ZERO_CROSS_PIN, INPUT);

  pinMode(TRIAC_GATE_PIN, OUTPUT);
  digitalWrite(TRIAC_GATE_PIN, LOW);

  // OLED
  u8g2.begin();

  updateOLED();

  // PREFERENCES
  prefs.begin("dimmer", false);

  // TIMER
  triacTimer = timerBegin(1000000);

  timerAttachInterrupt(
    triacTimer,
    &fireTriacISR
  );

  timerStop(triacTimer);

  // ZERO CROSS INTERRUPT
  attachInterrupt(
    digitalPinToInterrupt(ZERO_CROSS_PIN),
    zeroCrossISR,
    FALLING
  );

  // WIFI MANAGER
  WiFiManager wm;

  wifiStatus = "Searching";
  updateOLED();

  wifiStatus = "Connect Portal";
  updateOLED();

  bool res;

  res = wm.autoConnect(
          "ESP32-DIMMER",
          "12345678"
        );

  if (!res) {

    wifiStatus = "Failed";

    updateOLED();

    delay(3000);

    ESP.restart();
  }

  else {

    wifiStatus = "Connected";

    wifiSSID = WiFi.SSID();

    wifiConnected = true;

    updateOLED();

    digitalWrite(LED_PIN, HIGH);

    // MDNS
    if (MDNS.begin("esp32-dimmer")) {

      Serial.println("mDNS started");

      Serial.println(
        "Open: http://esp32-dimmer.local"
      );
    }

    else {

      Serial.println("mDNS failed");
    }
  }

  // OTA
  ArduinoOTA.setHostname("ESP32-DIMMER");

  ArduinoOTA.onStart([]() {

    wifiStatus = "OTA Update";

    updateOLED();
  });

  ArduinoOTA.onEnd([]() {

    wifiStatus = "Restarting";

    updateOLED();
  });

  ArduinoOTA.onProgress([](
                           unsigned int progress,
                           unsigned int total
                         ) {

    int percent =
      (progress * 100) / total;

    wifiStatus =
      "OTA " + String(percent) + "%";

    updateOLED();
  });

  ArduinoOTA.begin();

  // MDNS SERVICES
  MDNS.addService(
    "http",
    "tcp",
    80
  );

  MDNS.addService(
    "arduino",
    "tcp",
    3232
  );

  // SERVER
  server.on("/", handleRoot);

  server.on("/set", handleSet);

  server.begin();

  // WIFI
  WiFi.setSleep(false);

  // LOAD SAVED BRIGHTNESS
  int saved =
    prefs.getInt("brightness", 50);

  setDimPercent(saved);

  Serial.println();
  Serial.println("================================");
  Serial.println("ESP32 DIMMER READY");
  Serial.println("================================");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Web Control:");
  Serial.println("http://esp32-dimmer.local");

  Serial.println("OTA Ready");
}

// MAIN LOOP
void loop() {

  server.handleClient();

  ArduinoOTA.handle();

  if (millis() - lastOLED > 150) {

    lastOLED = millis();

    updateOLED();
  }
}