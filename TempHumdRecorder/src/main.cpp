#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ESPAsyncWebServer.h>
#include <Guineapig.WiFiConfig.h>
#include <FS.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#ifdef ESP32 
#include <SPIFFS.h>
#define WebFS SPIFFS
#else
#include <LittleFS.h>
#define WebFS LittleFS
#define USE_LITTLE_FS
#endif


//溫溼度偵測
// https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-web-server-arduino-ide/
#define DHTTYPE DHT11 // DHT 11
#ifdef ESP32
#define DHTPIN 5
#else
#define DHTPIN D3
#endif
DHT_Unified dht(DHTPIN, DHTTYPE);

//網站
AsyncWebServer server(80);

const char indexHtml[] PROGMEM = R"===(
<html>
<head>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <style>
    dl { width: 320px; margin: 12px auto; }
    dt { 
      font-size: 20pt; color: #444; background-color: #ddd;
      margin: 6pt 0; padding: 6pt 12pt;
    }
    dd {
      text-align: right; padding-right: 6pt;
    }
    .time { font-size: 24pt; color: seagreen; }
    .num { font-size: 36pt; color: dodgerblue; }
  </style>
</head>
<body>
  <dl>
    <dt>現在時刻</dt>
    <dd class=time>%TIME%</dd>
    <dt>溫度</dt>
    <dd class=num>%TEMP% °C</dd>
    <dt>濕度</dt>
    <dd class=num>%HUMD% %</dd>
  </dl>
</body>
</html>
)===";

//網路校時
//https://randomnerdtutorials.com/esp8266-nodemcu-date-time-ntp-client-server-arduino/
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

String currTime("");
String currTemp("");
String currHumd("");

String tmplProcessor(const String &var)
{
  if (var == "TIME")
    return currTime;
  else if (var == "TEMP")
    return currTemp;
  else if (var == "HUMD")
    return currHumd;
  return "?";
}

String historyFilePath = "/history.txt";

void setup()
{
  Serial.begin(9600);
  // 啟動 DHT11 溫溼度偵測器
  dht.begin();

  // 啟動 SPIFFS/LittleFS 檔案系統
#ifdef USE_LITTLE_FS  
  LittleFS.begin();
#else
  SPIFFS.begin(true);
#endif

  if (WiFiConfig.connectWiFi())
  {
    // 使用 NTP 校時
    // https://randomnerdtutorials.com/esp32-ntp-client-date-time-arduino-ide/
    timeClient.begin();
    timeClient.setTimeOffset(8 * 3600); //UTC+8
    while (!timeClient.update()) {
      timeClient.forceUpdate();
    }
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      auto *response = request->beginResponse_P(200, "text/html", indexHtml, tmplProcessor);
      request->send(response);
    });
    server.on("/history", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(WebFS, historyFilePath, "text/plain");
    });
    server.begin();
  }
}

String cmd = "";
int lastMin = -1;

void loop()
{
  // Delay between measurements.
  delay(1000);
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime((time_t *)&epochTime);
  currTime = String(ptm->tm_year + 1900) + "-" + String(ptm->tm_mon + 1) + "-" + String(ptm->tm_mday) + " " + timeClient.getFormattedTime();
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  currTemp = isnan(event.temperature) ? "N/A" : String(event.temperature);
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  currHumd = isnan(event.relative_humidity) ? "N/A" : String(event.relative_humidity);

  if (lastMin != ptm->tm_min)
  {
    lastMin = ptm->tm_min;
    auto file = WebFS.open(historyFilePath, "a");
    file.println(currTime + "," + currTemp + "," + currHumd);
    file.close();
  }

  while (Serial.available())
  {
    auto c = Serial.read();
    if (c == 13)
    {
      Serial.println();
      if (cmd == "clear")
      {
        SPIFFS.remove(historyFilePath);
        Serial.println(historyFilePath + " is deleted.");
      }
      else
      {
        Serial.println("unkown command - " + cmd);
      }
      cmd = "";
    }
    else if (c >= 32)
    {
      cmd += (char)c;
      Serial.print((char)c);
    }
  }
}