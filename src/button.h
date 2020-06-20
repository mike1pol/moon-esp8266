#pragma once

#include <GyverButton.h>
#include "ota.h"
#include "config.h"

extern GButton btn;
extern boolean state;
extern boolean changed;
extern int color;
extern int bri;
extern int prevBri;

class Button {
public:
  void tick();

private:
  boolean isClicked = false;
};