#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266WiFi.h>

extern AsyncWebServer server;

bool wifiAutoConfig();

void initWebServer(String  webUserId = "", String webUserPasswd = "");

const char wifi_setting_html[] PROGMEM = R"====(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { width: 180px; }
        input, div { width: 100%; margin: 6px; font-size: 16pt; }
    </style>
</head>
<body>
<div>Setup WiFi<div>
<form action="/" method="POST">
<input name="ssid" placeholder="SSID" />
<input type="password" name="passwd" placeholder="Password" />
<input type="submit" value="Save &amp; Reset" />
</form>
</body></html>
)===="; 