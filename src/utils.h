#pragma once

#include <FastLED.h>

#include "config.h"

extern int bri;
extern CRGB color;
extern CRGB leds[LED_WIDTH];

void fillColor();

void fillColor(const CRGB &c);

void fillColor(const CRGB &c, byte d);

void setBrightness(int b);

void fadeIn();

void fadeOut();

void show();

