#include <Arduino.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_SSD1306.h>
#include <ESPAsyncWebServer.h>
#include <Guineapig.WiFiConfig.h>
#ifdef ESP8266
#define WebFS LittleFS
#include "LittleFS.h"
#define USE_LITTLEFS
#else
#define WebFS SPIFFS
#include <FS.h>
#include <SPIFFS.h>
#endif

//參考 https://swf.com.tw/?p=905
//參考 https://github.com/me-no-dev/ESPAsyncWebServer
//參考 https://github.com/khoih-prog/ESPAsync_WiFiManager
//參考 https://randomnerdtutorials.com/esp32-web-server-spiffs-spi-flash-file-system/

#ifdef ESP8266
#define LED_ON LOW
#define LED_OFF HIGH
#else
#define LED_ON HIGH
#define LED_OFF LOW
#endif

void saveStatus(String key, String value)
{
  auto f = WebFS.open("/status/" + key, "w");
  f.print(value);
  f.close();
}
String readStatus(String key)
{
  if (!WebFS.exists("/status/" + key))
    return "";
  auto f = WebFS.open("/status/" + key, "r");
  auto v = f.readString();
  f.close();
  return v;
}

const char *ledKey = "led_builtin";

AsyncWebServer server(80);

#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(128, 64, &Wire, -1);
void printOled(String msg)
{
  display.ttyPrint(msg);
  display.display();
}

void setup()
{
  Serial.begin(115200);
#ifdef USE_LITTLEFS 
  if (!WebFS.begin())
  {
    Serial.println("LittleFS Mount Failed");
    return;
  }
#else  
  if (!WebFS.begin(false))
  {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
#endif
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.drawBitmap(32, 0, guineapig_logo, 64, 64, WHITE);
  display.display();
  delay(2500);
  display.setTextColor(WHITE, BLACK);
  display.setCursor(0, 56);
  display.ttyPrintln();
  WiFiConfig.logCallback = printOled;  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, (readStatus(ledKey) == "Y" ? LED_ON : LED_OFF));
  if (WiFiConfig.connectWiFi())
  {
    auto staticWebHandler =
        server.serveStatic("/html", WebFS, "/wwwroot/")
            .setDefaultFile("index.html");  
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      return request->redirect("/html/");
    });
    server.on("/led/status", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(200, "application/javascript",
                    String("vm.LedSwitch=") + (readStatus(ledKey) == "Y" ? "true" : "false"));
    });
    server.on("/led/switch", HTTP_POST, [](AsyncWebServerRequest *request) {
      auto ledOn = request->arg("v") == "on";
      //Save LED status to file
      digitalWrite(LED_BUILTIN, ledOn ? LED_ON : LED_OFF);
      saveStatus(ledKey, ledOn ? "Y" : "N");
      request->send(200, "text/plain", "OK");
    });
    //網站可設定帳號密碼
    //staticWebHandler.setAuthentication(webUserId.c_str(), webUserPasswd.c_str());
    server.onNotFound([](AsyncWebServerRequest *request) {
      request->send(404);
    });
    server.begin();
  }
}

void loop()
{
}