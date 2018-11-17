#include "mqttclient.h"

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
}

void MqttClient::reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
        //Serial.print("Attempting MQTT connection...");
        if (client.connect("Toreinfahrt")) {
            //Serial.println("connected");
            client.publish("Toreinfahrt/Status", "Undefined");
            client.subscribe("Toreinfahrt/Target");
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

    //every 2 sec
    //send status changes only
    unsigned long now = millis();
    if (now - lastMsg > 2000) {
        lastMsg = now;
        if(status != sentStatus){
            client.publish("Toreinfahrt/Status", status.c_str());
            sentStatus = status;
        }
        if(targetFeedback != sentTargetFeedback){
            client.publish("Toreinfahrt/TargetFeedback", targetFeedback.c_str());
            sentTargetFeedback = targetFeedback;
        }
        client.publish("Toreinfahrt/Heartbeat", "1");
    }
}

void MqttClient::setStatus(std::string newStatus){
    status = newStatus;
}

void MqttClient::setTarget(std::string newTarget){
    targetFeedback = newTarget;
}