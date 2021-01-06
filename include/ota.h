#ifndef OTA_H
#define OTA_H

#define DEBUG_ESP_OTA 1
#define DEBUG_ESP_PORT Serial
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

class OTA
{
public:
    OTA(const char* name);
    ~OTA();
    void check();
};

#endif /* OTA_H */

