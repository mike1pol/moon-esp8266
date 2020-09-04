#pragma once

#include <GyverButton.h>
#include "ota.h"
#include "config.h"

extern GButton btn;
extern boolean state, changed;
extern int bri, prevBri;

class Button {
public:
  void tick();
};