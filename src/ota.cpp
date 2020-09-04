#include "ota.h"

bool OtaManager::RequestOtaUpdate() {
  if (!WiFi.isConnected()) {
#if DEBUG
    Serial.println("WiFi not connected");
#endif
    return false;
  } else if (OtaFlag == OtaPhase::None) {
    OtaFlag = OtaPhase::GotFirstConfirm;
    firstConfirmTime = millis();
    fillColor(CRGB::Yellow);
    setBrightness(internalBri);
    delay(200);
    setBrightness(0);
    delay(200);
    setBrightness(internalBri);
    delay(200);
    setBrightness(0);
    delay(200);
    setBrightness(bri);
    fillColor();
#if DEBUG
    Serial.println("Got first confirmation for update");
#endif
  } else if (OtaFlag == OtaPhase::GotFirstConfirm) {
    OtaFlag = OtaPhase::GotSecondConfirm;
    otaStartTime = millis();
#if DEBUG
    Serial.println("Got second confirmation for update");
#endif
    fillColor(CRGB::Red);
    setBrightness(internalBri);
    startOtaUpdate();
    return true;
  }
  return false;
}

void OtaManager::HandleOtaUpdate() {
  if (OtaFlag == OtaPhase::GotFirstConfirm &&
      millis() - firstConfirmTime >= (OTA_CONFIRMATION_TIME * 1000)) {
    OtaFlag = OtaPhase::None;
    firstConfirmTime = 0;
#if DEBUG
    Serial.println("Timeout for confirm OTA update failed");
#endif
    return;
  }
  if ((OtaFlag == OtaPhase::GotSecondConfirm || OtaFlag == OtaPhase::InProgress) &&
      millis() - otaStartTime >= (OTA_CONFIRMATION_TIME * 60 * 1000)) {
    OtaFlag = OtaPhase::None;
    otaStartTime = 0;
#if DEBUG
    Serial.println("Timeout for wait firmware is out, OTA update failed");
#endif
    fillColor();
  }
  if (OtaFlag == OtaPhase::InProgress)
    ArduinoOTA.handle();
}

void OtaManager::startOtaUpdate() {
  char espHostname[65];
  sprintf_P(espHostname, "%s-%u", AP_NAME, ESP.getChipId());
  ArduinoOTA.setPort(8266);
  ArduinoOTA.setHostname(espHostname);
  ArduinoOTA.setPassword(AP_PASS);
  ArduinoOTA.onStart([this]() {
    OtaFlag = OtaPhase::InProgress;
    char type[16];
    if (ArduinoOTA.getCommand() == U_FLASH)
      strcpy_P(type, "sketch");
    else
      strcpy_P(type, "filesystem");
#if DEBUG
    Serial.printf("Start updating %s\n", type);
#endif
  });
  ArduinoOTA.onEnd([this]() {
    OtaFlag = OtaPhase::Done;
    firstConfirmTime = 0;
    otaStartTime = 0;
#if DEBUG
    Serial.println("Firmware was update. Reboot");
    delay(500);
#endif
  });
  ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
#if DEBUG
    Serial.printf("Firmwave update progress: %d\n", (progress / (total / 100)));
#endif
  });
  ArduinoOTA.onError([this](ota_error_t error) {
    OtaFlag = OtaPhase::None;
    firstConfirmTime = 0;
    otaStartTime = 0;
#if DEBUG
    Serial.printf("Error updating firmware [%u]: ", error);
    if (error == OTA_AUTH_ERROR)
        Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
        Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
        Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
        Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
        Serial.println("End Failed");
    Serial.print("Reset all states\nWaiting new firmware\n");
#endif
  });
  ArduinoOTA.setRebootOnSuccess(true);
  ArduinoOTA.begin();
  OtaFlag = OtaPhase::InProgress;
#if DEBUG
  Serial.printf("Update started on port %s\n", espHostname);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("Timeout for upload new firmware is: %u seconds\nPassword: %s\n", (CONF_TIMEOUT * 1000), AP_PASS);
#endif
}