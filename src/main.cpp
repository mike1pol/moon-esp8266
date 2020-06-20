#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>

#include <FastLED.h>

#include "config.h"
#include "mqtt.h"
#include "ota.h"

// Declare functions
void fillColor();
void fillRedColor();
void fadeIn();
void fadeOut();
void show();
void setBrightness(int b);

OtaManager ota;
OtaPhase OtaManager::OtaFlag = OtaPhase::None;

CRGB leds[LED_WIDTH];

boolean state, prevState, changed;
int bri = LED_BRIGHTNESS, prevBri = LED_BRIGHTNESS;
int color = 0;

#include <GyverButton.h>
#include "button.h"
GButton btn(BTN_PIN, LOW_PULL, NORM_OPEN);
Button button;

CRGB colors[LED_COLOR_LEN] = {
    CRGB::WhiteSmoke,
    CRGB::LightSkyBlue,
    CRGB::SkyBlue,
    CRGB::DeepSkyBlue,
    CRGB::Orange,
    CRGB::DarkOrange};

WiFiManager wifiManager;
WiFiClient espClient;
byte isCaptive;

MQTT mqtt;

void setup()
{
//  ESP.wdtDisable();
#if DEBUG
  Serial.begin(115200);
  Serial.println();
#endif

  delay(1000);
  if (digitalRead(BTN_PIN))
  {
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
  if (WiFi.SSID().length())
  {
#if DEBUG
    Serial.print("Connecting to WiFi: ");
    Serial.println(WiFi.SSID());
#endif
  }
  else
  {
#if DEBUG
    Serial.println("Setting up WiFi");
#endif
    wifiManager.setBreakAfterConfig(true);
    isCaptive = 1;
  }
  wifiManager.setConnectTimeout(CONN_TIMEOUT);
  wifiManager.setConfigPortalTimeout(CONF_TIMEOUT);
  wifiManager.autoConnect(AP_NAME, AP_PASS);

  if (WiFi.status() != WL_CONNECTED)
  {
#if DEBUG
    Serial.println("Error connecting to WiFi");
#endif
    wifiManager.resetSettings();
    ESP.restart();
  }
  else if (isCaptive)
  {
    ESP.restart();
  }
#if DEBUG
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
#endif
  mqtt.setup();
#if DEBUG
  Serial.println("Moon ready");
#endif
  memset8(leds, 0, sizeof(leds));
}

void loop()
{
  button.tick();
  ota.HandleOtaUpdate();
  if (WiFi.status() == WL_CONNECTED)
  {
    mqtt.tick();
  }
  if (changed)
  {
    changed = false;
    if (state)
    {
      if (prevBri != bri)
      {
#if DEBUG
        Serial.printf("Brightness changed to: %d\n", bri);
#endif
        setBrightness(bri);
        prevBri = bri;
      }
      else if (prevState)
      {
#if DEBUG
        Serial.printf("Color changed %d\n", color);
#endif
        fillColor();
      }
      else
      {
#if DEBUG
        Serial.println("State changed to: on");
#endif
        fillColor();
        fadeIn();
      }
    }
    else
    {
#if DEBUG
      Serial.println("State changed to: off");
#endif
      fadeOut();
      setBrightness(0);
    }
    mqtt.publish();
    prevState = state;
  }
}

void show()
{
  FastLED.show();
}

void setBrightness(int b)
{
  FastLED.setBrightness(b);
  show();
}

void fadeOut()
{
  int ib = bri;
  while (ib > 0)
  {
    setBrightness(ib);
    ib -= 1;
    delay(LED_FADE);
  }
}

void fadeIn()
{
  int ib = 0;
  while (ib < bri)
  {
    setBrightness(ib);
    ib += 1;
    delay(LED_FADE);
  }
  setBrightness(bri);
}

void fillRedColor()
{
  delay(2);
  for (int i = 0; i < LED_WIDTH; i++)
    leds[i] = CRGB::Red;
  setBrightness(150);
}

void fillColor()
{
  delay(2);
  for (int i = 0; i < LED_WIDTH; i++)
  {
    leds[i] = colors[color];
    show();
    delay(LED_FADE);
  }
}
