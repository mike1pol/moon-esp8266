#pragma once

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "GyverTimer.h"
#include "config.h"

extern WiFiClient espClient;
extern boolean state;
extern boolean changed;
extern int bri;
extern int prevBri;
extern int color;

class MQTT
{
public:
    void setup();
    void tick();
    void publish();

private:
    char *clientName;
    char *topicCmd;
    char *topicState;
    char *currentState;
    void updateMsg();
    static void callback(char *topic, byte *payload, unsigned int length);
};