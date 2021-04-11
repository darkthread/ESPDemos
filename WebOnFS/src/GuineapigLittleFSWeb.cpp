#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include "GuineapigLittleFSWeb.h"
AsyncWebServer server(80);
bool wifiSettingMode = false;
const char *credFileName = "wificred.txt";
void setupWifiSettingWeb()
{
    //Set WiFi AP mode
    auto apSsid = "ESP-" + String(ESP.getChipId(), HEX);
    WiFi.softAP(apSsid);
    IPAddress apIp = WiFi.softAPIP();
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", wifi_setting_html);
    });
    server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
        String ssid = (*request->getParam("ssid", true)).value();
        String passwd = (*request->getParam("passwd", true)).value();
        auto file = LittleFS.open(credFileName, "w");
        file.println(ssid);
        file.println(passwd);
        file.close();
        request->send(200, "text/plain", "Rebooting");
        WiFi.mode(WIFI_STA);
        ESP.restart();
    });

    Serial.print("AP Mode SSID: ");
    Serial.print(apSsid);
    Serial.print(" IP=");
    Serial.println(apIp);
    server.begin();
    Serial1.println("WiFi setup web is ready");
}

bool wifiAutoConfig()
{
    LittleFS.begin();
    String ssid = WiFi.SSID();
    String pwd = WiFi.psk();
    if (LittleFS.exists(credFileName))
    {
        Serial.print("read stored credential: ");
        auto f = LittleFS.open(credFileName, "r");
        if (f.available())
        {
            ssid = f.readStringUntil('\n');
            ssid.trim();
            Serial.println(ssid);
        }
        if (f.available())
        {
            pwd = f.readStringUntil('\n');
            pwd.trim();
        }
        f.close();
    }
    int timeoutCount = 50;
    bool connected = false;
    if (ssid != "" && pwd != "")
    {
        Serial.println(String("conneting ") + ssid + "...");
        WiFi.begin(ssid, pwd);
        while (WiFi.status() != WL_CONNECTED && timeoutCount > 0)
        {
            Serial.print(".");
            delay(100);
            timeoutCount--;
        }
        Serial.println("");
        connected = timeoutCount > 0;
    }
    if (!connected)
    {
        wifiSettingMode = true;
        setupWifiSettingWeb();
        return false;
    }
    else
    {
        Serial.println("");
        Serial.print("Connected to ");
        Serial.println(ssid);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        Serial.println("");
        return true;
    }
}

void initWebServer(String webUserId, String webUserPasswd)
{
    auto staticWebHandler = server.serveStatic("/html", LittleFS, "/wwwroot").setDefaultFile("index.html");
    if (webUserId != "" && webUserPasswd != "")
        staticWebHandler.setAuthentication(webUserId.c_str(), webUserPasswd.c_str());
    server.onNotFound([](AsyncWebServerRequest *request) {
        request->send(404);
    });
    server.begin();
    Serial.println("Web server is ready");
}