#include "mqttclient.h"
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

MqttClient::MqttClient(std::function<void(char* topic, byte* payload, unsigned int length)> cmdCallback)
{
    setupWifi();

    client.setServer(mqtt_server, 1883);
    client.setCallback(cmdCallback);
}

void MqttClient::setupWifi() {
    delay(10);
    //Serial.println();
    //Serial.print("Connecting to ");
    //Serial.println(ssid);

    WiFi.mode(WIFI_OFF);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        //Serial.print(".");
    }

    //Serial.println("");
    //Serial.println("WiFi connected");
    //Serial.println("IP address: ");
    //Serial.println(WiFi.localIP());

    initOta();
}

void MqttClient::initOta(){
    ArduinoOTA.setHostname("torschalter");
    ArduinoOTA.setPassword("tor");
    ArduinoOTA.onStart([]() {
        Serial.println("Start");
        });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.begin();
}

void MqttClient::reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
        //Serial.print("Attempting MQTT connection...");
        if (client.connect("Toreinfahrt")) {
            //Serial.println("connected");
            client.publish("Toreinfahrt/Status", "Undefined");
            client.subscribe("Toreinfahrt/Target", 1);//qos "at least once"
        } else {
            //Serial.print("failed, rc=");
            //Serial.print(client.state());
            //Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void MqttClient::operate(){
    if (!client.connected()){
        reconnect();
    }
    client.loop();
    ArduinoOTA.handle();

    //every 0.5 sec
    unsigned long now = millis();
    if (now - lastMsg > 500) {
        lastMsg = now;
        client.publish("Toreinfahrt/Status", status.c_str());
        client.publish("Toreinfahrt/TargetFeedback", targetFeedback.c_str());
    }
}

void MqttClient::setStatus(std::string newStatus){
    status = newStatus;
}

void MqttClient::setTarget(std::string newTarget){
    targetFeedback = newTarget;
}