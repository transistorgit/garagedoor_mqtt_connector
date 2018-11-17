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
    std::string sentTargetFeedback {"Undefined"};

    void setupWifi();
    void reconnect();
    
public:
    MqttClient(std::function<void(char* topic, byte* payload, unsigned int length)> cmdCallback);
    void operate();
    void setStatus(std::string newStatus);
    void setTarget(std::string newTarget);
};


#endif