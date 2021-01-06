// Программа для управления RGB-лентой на светодиодах типа ws281x.
// Коммерческое использование проекта без согласия автора запрещено.
// Есть вопросы? Пишите: sergey@getbyte.ru

#include <stdint.h>
#include "smartled.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <math.h>
#include "ota.h"

const uint16_t PIXEL_COUNT = 10;
const uint8_t PIN_PIXEL = 5;    // D1
const uint8_t PIN_NEXT = 14;    // D5
const uint8_t PIN_RND  = 15;    // D8

typedef struct AccessPoint
{
  const char* apName;
  const char* apPassword;
} AccessPoint;

const AccessPoint ap[] = {
   {"APName", "Password"}
};
int apCount = 1;
const char* host = "host";

ESP8266WebServer server(80);
MDNSResponder mdns;

SmartLED* smart;
OTA* ota;

String getContentType(String filename){
  if(server.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".bmp")) return "image/bmp";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}

void handleRoot()
{
    String path = "/index.html";
    String contentType = getContentType(path);
    if (SPIFFS.exists(path))
    {
        File file = SPIFFS.open(path, "r");
        size_t sent = server.streamFile(file, contentType);
        file.close();
    }
}

bool handleFileRead(String path)
{
    String contentType = getContentType(path);
    if (SPIFFS.exists(path))
    {
        File file = SPIFFS.open(path, "r");
        size_t sent = server.streamFile(file, contentType);
        file.close();
        return true;
    }
    return false;
}

void setup(void)
{
    Serial.begin(115200);
    Serial.print("\n");
    SPIFFS.begin();
    ota = new OTA(host);

    int apIdx = 0;
// comment from here
    WiFi.mode(WIFI_STA);
    WiFi.begin(ap[apIdx].apName, ap[apIdx].apPassword);
    Serial.print("Try to connect to ");
    Serial.print(ap[apIdx].apName);

    // Wait for connection
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(" .");
        if (++attempts > 5)
        {
            delay(500);
            attempts = 0;
            Serial.print(": 5 attempts to connect to ");
            Serial.print(ap[apIdx].apName);
            Serial.print(", connection status is ");
            Serial.println(WiFi.status());
            WiFi.disconnect(true);
            WiFi.mode(WIFI_STA);
            delay(500);
            apIdx++;
            if (apIdx >= apCount)
            {
                apIdx = 0;
            }
            Serial.print("Now try to connect to ");
            Serial.print(ap[apIdx].apName);
            WiFi.begin(ap[apIdx].apName, ap[apIdx].apPassword);
            delay(500);
        }
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.print(ap[apIdx].apName);
    Serial.print(", IP address: ");
    Serial.println(WiFi.localIP());

////////////////////////////////// until here to remove connection to router, and uncomment next block
/*  const char* apname = "MyAP";
  const char* appassword = "password";
  
    WiFi.mode(WIFI_AP);
//    WiFi.softAP(apname, appassword);

        WiFi.softAPConfig(IPAddress(192, 168, 100, 1), IPAddress(192, 168, 100, 1), IPAddress(255, 255, 255, 0));
        WiFi.softAP(host, appassword);
//        WiFi.begin(host, "christmas-tree");
        delay(1000);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("localIP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("softAPIP address: ");
    Serial.println(WiFi.softAPIP());
*/
//////////////////////////////////// until here

    smart = new SmartLED(PIXEL_COUNT, PIN_PIXEL, NEO_GRB, true);
    

    mdns.begin(host, WiFi.softAPIP());
    
    server.on("/", handleRoot);
    server.on("/index.html", handleRoot);

    server.onNotFound([]()
    {
        if (!handleFileRead(server.uri()))
            server.send(404, "text/plain", "FileNotFound");
    });

    server.begin();
}

void loop(void)
{
    server.handleClient();
    smart->process();
    ota->check();
    delay(2);
}
