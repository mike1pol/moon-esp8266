#pragma once

#define DEBUG true

// OTA
#define OTA_CONFIRMATION_TIME 5

// BUTTON
#define BTN_PIN 4

// Websocket
#define WEBSOCKET_PORT 81

// MQTT
#define MQTT_ENABLE true
#define MQTT_SERVER "192.168.10.30"
#define MQTT_PORT 1883
#define MQTT_TOPIC_BASE "MoonLamp"
#define MQTT_CLIENT_PREFIX "MoonLamp"
#define MQTT_TOPIC_STATE "state" // TOPIC_BASE / TOPIC_CLIENT_PREFIX + _ + TOPIC_CLIENT_ID / TOPIC_STATE
#define MQTT_TOPIC_CMD "cmd" // TOPIC_BASE / TOPIC_CLIENT_PREFIX + _ + TOPIC_CLIENT_ID / TOPIC_CMD

// LED setup
#define LED_PIN 14
#define LED_WIDTH 13
#define LED_LIMIT 1000
#define LED_COLOR GRB
#define LED_MIN_BRIGHTNESS 10
#define LED_BRIGHTNESS 80
#define LED_FADE 30

// WiFi
#define AP_NAME "MoonLamp"
#define AP_PASS "12345678"
#define CONN_TIMEOUT 10
#define CONF_TIMEOUT long(60 * 10)