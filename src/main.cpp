#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <FastLED.h>
#include <WebSocketsServer.h>

#include "config.h"
#include "utils.h"
#include "mqtt.h"
#include "ota.h"
#include "websocket.h"

// Websocket server
WebSocketsServer webSocket = WebSocketsServer(WEBSOCKET_PORT);

// OTA manager
OtaManager ota;
OtaPhase OtaManager::OtaFlag = OtaPhase::None;

CRGB leds[LED_WIDTH];

boolean state, prevState, changed;
int bri = LED_BRIGHTNESS, prevBri = LED_BRIGHTNESS;

#include <GyverButton.h>
#include "button.h"

GButton btn(BTN_PIN, LOW_PULL, NORM_OPEN);
Button button;

CRGB color = CRGB(255, 255, 255);

WiFiManager wifiManager;
WiFiClient espClient;
byte isCaptive;

#if MQTT_ENABLE
MQTT mqtt;
#endif

void setup() {
//  ESP.wdtDisable();
#if DEBUG
  Serial.begin(9600);
  Serial.println();
#endif

  delay(1000);
  if (digitalRead(BTN_PIN)) {
#if DEBUG
    Serial.println("Reset settings");
#endif
    wifiManager.resetSettings();
    ESP.restart();
  }

  FastLED.addLeds<WS2812B, LED_PIN, LED_COLOR>(leds, LED_WIDTH);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, LED_LIMIT);
  FastLED.clear();
  FastLED.setBrightness(0);
  FastLED.show();

  wifiManager.setDebugOutput(DEBUG);
  if (WiFi.SSID().length()) {
#if DEBUG
    Serial.print("Connecting to WiFi: ");
    Serial.println(WiFi.SSID());
#endif
  } else {
#if DEBUG
    Serial.println("Setting up WiFi");
#endif
    wifiManager.setBreakAfterConfig(true);
    isCaptive = 1;
  }
  wifiManager.setConnectTimeout(CONN_TIMEOUT);
  wifiManager.setConfigPortalTimeout(CONF_TIMEOUT);
  wifiManager.autoConnect(AP_NAME, AP_PASS);

  if (WiFi.status() != WL_CONNECTED) {
#if DEBUG
    Serial.println("Error connecting to WiFi");
#endif
    wifiManager.resetSettings();
    ESP.restart();
  } else if (isCaptive) {
    ESP.restart();
  }
#if DEBUG
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
#endif
#if MQTT_ENABLE
  mqtt.setup();
#endif
#if DEBUG
  Serial.println("Moon ready");
#endif
  memset8(leds, 0, sizeof(leds));
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  button.tick();
  ota.HandleOtaUpdate();
  if (WiFi.status() == WL_CONNECTED) {
#if MQTT_ENABLE
    mqtt.tick();
#endif
    webSocket.loop();
  }
  if (changed) {
    changed = false;
    if (state) {
      if (prevBri != bri) {
#if DEBUG
        Serial.printf("Brightness changed to: %d\n", bri);
#endif
        setBrightness(bri);
        prevBri = bri;
      } else if (prevState) {
#if DEBUG
        Serial.printf("Color changed %d %d %d\n", color.r, color.g, color.b);
#endif
        fillColor();
      } else {
#if DEBUG
        Serial.println("State changed to: on");
#endif
        fillColor();
        fadeIn();
      }
    } else {
#if DEBUG
      Serial.println("State changed to: off");
#endif
      fadeOut();
      setBrightness(0);
    }
#if MQTT_ENABLE
    mqtt.publish();
#endif
    prevState = state;
  }
}

