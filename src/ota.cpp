#include "ota.h"

void startOTA()
{
    Serial.println("Starting OTA update\n");
}

void endOTA()
{
    Serial.println("OTA update finished\n");
}

void errorOTA(ota_error_t error)
{
    Serial.printf("Error [%u] caused: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
    else Serial.println("Unknown error");
}

void progressOTA(unsigned int progress, unsigned int total)
{
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
}

OTA::OTA(const char* name)
{
    ArduinoOTA.setHostname(name);
//    ArduinoOTA.setPassword((const char*)"123");
    ArduinoOTA.onStart(startOTA);
    ArduinoOTA.onEnd(endOTA);
    ArduinoOTA.onProgress(progressOTA);
    ArduinoOTA.onError(errorOTA);
    ArduinoOTA.begin();
}

OTA::~OTA()
{
    
}

void OTA::check()
{
    ArduinoOTA.handle();
}

