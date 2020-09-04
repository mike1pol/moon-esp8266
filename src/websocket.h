#pragma once

#include <WebSocketsServer.h>
#include <FastLED.h>

#include "config.h"

extern WebSocketsServer webSocket;
extern boolean state, changed;
extern int bri, prevBri;
extern CRGB color;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);