#pragma once

#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <FastLED.h>

#include "config.h"
#include "utils.h"

extern int bri;

enum OtaPhase {
  None = 0,
  GotFirstConfirm,
  GotSecondConfirm,
  InProgress,
  Done
};

class OtaManager {
public:
  static OtaPhase OtaFlag;

  bool RequestOtaUpdate();

  void HandleOtaUpdate();

private:
  int internalBri = 250;
  unsigned long firstConfirmTime = 0;
  unsigned long otaStartTime = 0;

  void startOtaUpdate();
};