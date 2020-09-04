#include "utils.h"

void show() {
  FastLED.show();
}

void setBrightness(int b) {
  FastLED.setBrightness(b);
  show();
}

void fadeOut() {
  int ib = bri;
  while (ib > 0) {
    setBrightness(ib);
    ib -= 1;
    delay(LED_FADE);
  }
}

void fadeIn() {
  int ib = 0;
  while (ib < bri) {
    setBrightness(ib);
    ib += 1;
    delay(LED_FADE);
  }
  setBrightness(bri);
}

void fillColor() {
  delay(2);
  fillColor(color, 1);
}

void fillColor(const CRGB &c) {
  fillColor(c, 0);
}

void fillColor(const CRGB &c, byte d) {
  for (auto &led : leds) {
    led = c;
    if (d) {
      show();
      delay(LED_FADE);
    }
  }
}

