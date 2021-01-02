#ifndef TOR_MQTTCLIENT_H
#define TOR_MQTTCLIENT_H

#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <functional>
#include <string>

class MqttClient{
    const char* ssid = "myssid";
    const char* password = "secret";
    const char* mqtt_server = "192.168.168.112";

    WiFiClient espClient;
    PubSubClient client {espClient};

    unsigned int lastMsg = 0;
    std::string status {"Undefined"};
    std::string targetFeedback {"Undefined"};
    std::string sentStatus {"Undefined"};

    void setupWifi();
    void initOta();
    void reconnect();
    
public:
    MqttClient(std::function<void(char* topic, byte* payload, unsigned int length)> cmdCallback);
    void operate();
    void setStatus(std::string newStatus);
    void setTarget(std::string newTarget);
    //for debugging
    void sendButtonState(bool isActivated){client.publish("Toreinfahrt/Button", isActivated?"on":"off");}
    void sendRadioState(bool isActivated){client.publish("Toreinfahrt/Radio", isActivated?"on":"off");}
    void sendLimitSwState(bool isActivated){client.publish("Toreinfahrt/LimitSw", isActivated?"on":"off");}
    void sendRssi(int rssi){char buf[10]; std::sprintf(buf, "%d", rssi); client.publish("Toreinfahrt/RSSI", buf);}
};

#endif