#pragma once

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <FastLED.h>

#include "GyverTimer.h"
#include "config.h"

extern WiFiClient espClient;
extern boolean state, changed;
extern int bri, prevBri;
extern CRGB color;

class MQTT {
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