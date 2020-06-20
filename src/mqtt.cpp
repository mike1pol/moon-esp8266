#include "mqtt.h"

PubSubClient pubSub(espClient);
GTimer_ms reconnectTimer(5000);

void MQTT::setup()
{
    char chipId[30];
    sprintf_P(chipId, PSTR("%u"), ESP.getChipId());
    uint8_t clientNameLength = strlen(MQTT_CLIENT_PREFIX) + 1 + strlen(chipId) + 1;
    clientName = (char *)malloc(clientNameLength);
    sprintf_P(clientName, PSTR("%s_%s"), MQTT_CLIENT_PREFIX, chipId);
#if DEBUG
    Serial.printf("ClientName: %s\n", clientName);
#endif

    uint8_t topicCmdLength = strlen(MQTT_TOPIC_BASE) + 1 + strlen(clientName) + 1 + strlen(MQTT_TOPIC_CMD) + 1;
    topicCmd = (char *)malloc(topicCmdLength);
    sprintf(topicCmd, "%s/%s/%s", MQTT_TOPIC_BASE, clientName, MQTT_TOPIC_CMD);
#if DEBUG
    Serial.printf("topicCmd: %s\n", topicCmd);
#endif

    uint8_t topicStateLength = strlen(MQTT_TOPIC_BASE) + 1 + strlen(clientName) + 1 + strlen(MQTT_TOPIC_STATE) + 1;
    topicState = (char *)malloc(topicStateLength);
    sprintf(topicState, "%s/%s/%s", MQTT_TOPIC_BASE, clientName, MQTT_TOPIC_STATE);
#if DEBUG
    Serial.printf("topicState: %s\n", topicState);
#endif

    pubSub.setServer(MQTT_SERVER, MQTT_PORT);
    pubSub.setCallback(callback);
}

void MQTT::tick()
{
    if (!pubSub.connected() && reconnectTimer.isReady())
    {
        while (!pubSub.connected())
        {
#if DEBUG
            Serial.println("Attempting MQTT connection");
#endif
            if (pubSub.connect(clientName))
            {
#if DEBUG
                Serial.println("Connected");
#endif
                if (!pubSub.subscribe(topicCmd))
                {
#if DEBUG
                    Serial.printf("Error subscribe to: %s\n", topicCmd);
#endif
                }
                publish();
            }
            else
            {
#if DEBUG
                Serial.print("failed, rc=");
                Serial.print(pubSub.state());
                Serial.println(" try again in 5 seconds");
#endif
                delay(5000);
            }
        }
    }
    if (pubSub.connected())
        pubSub.loop();
}

void MQTT::publish()
{
    updateMsg();
    if (!pubSub.publish(topicState, currentState))
    {
#if DEBUG
        Serial.printf("Error publish to: %s message: %s\n", topicCmd, currentState);
#endif
    }
}

void MQTT::updateMsg()
{
    uint8_t currentStateLength = 8;
    currentState = (char *)malloc(currentStateLength);
    sprintf(currentState, "%d %d %d", state, bri, color);
}

void MQTT::callback(char *topic, byte *payload, unsigned int length)
{
    char buff[100];
    char input[length];
    for (int i = 0; i < length; i += 1)
        input[i] = (char)payload[i];
    input[length] = '\0';
#if DEBUG
    Serial.printf("Message arrived [%s] %s\n", topic, input);
#endif
    if (!strncmp_P(input, PSTR("P_ON"), 4) && !state)
    {
        changed = true;
        state = true;
    }
    else if (!strncmp_P(input, PSTR("P_OFF"), 5) && state)
    {
        changed = true;
        state = false;
    }
    else if (!strncmp_P(input, "BRI", 3))
    {
        prevBri = bri;
        memcpy(buff, &input[3], strlen(input));
        bri = constrain(atoi(buff), LED_MIN_BRIGHTNESS, 255);
        if (state)
            changed = true;
    }
    else if (!strncmp_P(input, "MODE", 4))
    {
        memcpy(buff, &input[4], strlen(input));
        color = constrain(atoi(buff), 0, 5);
        if (state)
            changed = true;
    }
    else if (!strncmp_P(input, "NEXT", 4))
    {
        color += 1;
        if (color >= LED_COLOR_LEN)
            color = 0;
        if (state)
            changed = true;
    }
}
