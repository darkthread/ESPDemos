#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "GuineapigLittleFSWeb.h"
#include "LittleFS.h"

//參考 https://swf.com.tw/?p=905
//參考 https://github.com/me-no-dev/ESPAsyncWebServer
//參考 https://github.com/khoih-prog/ESPAsync_WiFiManager
//參考 https://randomnerdtutorials.com/esp32-web-server-spiffs-spi-flash-file-system/

#define LED_ON LOW
#define LED_OFF HIGH

void saveStatus(String key, String value) {
  auto f = LittleFS.open("/status/" + key, "w");
  f.print(value);
  f.close();
}
String readStatus(String key) {
  if (!LittleFS.exists("/status/" + key)) return "";
  auto f = LittleFS.open("/status/" + key, "r");
  auto v = f.readString();
  f.close();
  return v;
}

const char* ledKey = "led_builtin";

void setup()
{
  Serial.begin(115200);
  delay(2000);
  Serial.println("Guineapig ESP8266 LittleFS Web");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, (readStatus(ledKey) == "Y" ? LED_ON : LED_OFF));
  if (wifiAutoConfig())
  {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      return request->redirect("/html/");
    });
    server.on("/led/status", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(200, "application/javascript", 
        String("vm.LedSwitch=") + (readStatus(ledKey) == "Y" ? "true" : "false"));
    });    
    //Regular expression，cool!!
    server.on("/led/switch", HTTP_POST, [](AsyncWebServerRequest *request) {
      auto ledOn = request->arg("v") == "on";
      //Save LED status to file
      digitalWrite(LED_BUILTIN, ledOn ? LED_ON : LED_OFF);
      saveStatus(ledKey, ledOn ? "Y" : "N");
      request->send(200, "text/plain", "OK");
    });

    initWebServer();
  }
}

void loop()
{
}