#pragma once

#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>

#include "config.h"

extern void fillRedColor();
extern void setBrightness(int b);
extern void fillColor();

enum OtaPhase {
    None = 0,
    GotFirstConfirm,
    GotSecondConfirm,
    InProgress,
    Done
};

class OtaManager
{
    public:
        static OtaPhase OtaFlag;
        bool RequestOtaUpdate();
        void HandleOtaUpdate();
    private:
        long firstConfirmTime = 0;
        long otaStartTime = 0;
        void startOtaUpdate();
};