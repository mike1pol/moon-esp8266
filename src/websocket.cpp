#include "websocket.h"

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  char buff[100];
  char* input = (char*)payload;
  switch(type) {
    case WStype_DISCONNECTED:
#if DEBUG
      Serial.printf("[%u] Disconnected!\n", num);
#endif
      break;
    case WStype_CONNECTED:
    {
      IPAddress ip = webSocket.remoteIP(num);
#if DEBUG
      Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
#endif
      char output[100];
      sprintf(output, "CURR %d %d %d %d %d", state, bri, color.r, color.g, color.b);
      webSocket.sendTXT(num, output);
    }
      break;
    case WStype_TEXT:
#if DEBUG
      Serial.printf("[%u] get Text: %s\n", num, payload);
#endif
      if (!strncmp_P(input, "P_ON", 4) && !state) {
        changed = true;
        state = true;
      } else if (!strncmp_P(input, "P_OFF", 5) && state) {
        changed = true;
        state = false;
      } else if (!strncmp_P(input, "BRI", 3)) {
        prevBri = bri;
        memcpy(buff, &input[3], strlen(input));
        bri = constrain(atoi(buff), LED_MIN_BRIGHTNESS, 255);
        if (state)
          changed = true;
      } else if (!strncmp_P(input, "CURR", 4)) {
        char output[100];
        sprintf(output, "CURR %d %d %d %d %d", state, bri, color.r, color.g, color.b);
        webSocket.sendTXT(num, output);
        return;
      }
      webSocket.broadcastTXT(payload);
      break;
    case WStype_PING:
    case WStype_PONG:
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      break;
  }
}