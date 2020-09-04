#include "button.h"

extern OtaManager ota;

void Button::tick() {
  btn.tick();
  if (btn.isSingle() && !btn.isHolded() && !btn.isHold()) {
    state = !state;
    changed = true;
#if DEBUG
    Serial.println("Change state");
#endif
  } else if (btn.isTriple()) {
    ota.RequestOtaUpdate();
#if DEBUG
    Serial.println("Handle OTA");
#endif
  } else if (btn.isStep()) {
#if DEBUG
    Serial.println("Change bri");
#endif
    prevBri = bri;
    bri = constrain((btn.getHoldClicks() >= 1) ? bri - 5 : bri + 5, LED_MIN_BRIGHTNESS, 255);
    changed = true;
  }
}
