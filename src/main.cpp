#include <Arduino.h>
#include "statemachine.h"
#include "mqttclient.h"
#include "filteredbutton.h"

#define RADIOPIN 0  //in, must be held high at boot
#define BUTTONPIN 2 //in/out, must be held high at boot
#define LIMITPIN 3  //in (this is the Rx Pin), 

//define the cmd sources. pins and network
DoorState mqttCmd {DoorState::Undefined};
DoorState keyCmd {DoorState::Undefined};

//use the button input pin as an output to overwrite trigger
void executeStateCmd(DoorState state, bool activateOutput){
    if(activateOutput){
        //Serial.print("StateCmd ");
        //Serial.println(StateStrings.at(state).c_str());
    }

    if(activateOutput)
    {
      digitalWrite(BUTTONPIN, HIGH);
      pinMode(BUTTONPIN, OUTPUT);
      digitalWrite(BUTTONPIN, LOW);
      delay(1000);
      digitalWrite(BUTTONPIN, HIGH);
      pinMode(BUTTONPIN, INPUT);
    }
}

MqttClient* mqttClient = nullptr;
StateMachine sm = StateMachine(executeStateCmd);

//callback from mqtt receiver. translates cmd String into DoorState Target
void translateMqttCmd(std::string cmd){
    mqttCmd = DoorState::Undefined;
    auto findResult = std::find_if(std::begin(MqttStateStrings), std::end(MqttStateStrings), [cmd](const std::pair<DoorState,std::string>& s)
    {
        return s.second == cmd;
    });
    if(findResult != MqttStateStrings.end())
    {
        mqttCmd = findResult->first;
        //char buf[20];
        //std::sprintf(buf, "%d", mqttCmd);
        //Serial.print(buf);
        //Serial.println();
    } 
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    std::string topicStr {topic};
    if(topicStr.compare("Toreinfahrt/Target")==0){
        //Serial.print("Target ");
        auto cmd = std::string(reinterpret_cast<const char*>(payload), length);
        //Serial.print(cmd.c_str());
        //Serial.println();
        translateMqttCmd(cmd);
    }    
}

void setup() {
    pinMode(LIMITPIN, INPUT);
    pinMode(RADIOPIN, INPUT);
    pinMode(BUTTONPIN, INPUT);

    Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY, 1);
    mqttClient = new MqttClient(mqttCallback);
    mqttClient->setStatus(StateStrings.at(DoorState::Stopped));
}

void loop() {
    static long lastMsg = 0;
    mqttClient->operate();

    //call state machine each 100ms
    //todo timer stuff
    unsigned long now = millis();
    if (now - lastMsg > 100) {
        lastMsg = now;
        //check if mqtt delivered a valid cmd
        bool triggerByRadio = !digitalRead(RADIOPIN);//remote controller triggers high level, but we have a inverter in place (because pin must be high at boot)
        bool triggerByButton = !digitalRead(BUTTONPIN);//button triggers by pulling low
        bool limitSwitchHit = !digitalRead(LIMITPIN);//reed triggers by pulling low
        static FilteredButton input;

        sm.operate(input.isOn(triggerByButton || triggerByRadio), mqttCmd, limitSwitchHit);

        mqttClient->setStatus(StateStrings.at(sm.getState()));
        mqttClient->setTarget(StateStrings.at(sm.getTarget()));
        if(sm.getTarget() == mqttCmd){
            mqttCmd = DoorState::Undefined;//prevent retriggering
        }

        //report for debugging
        mqttClient->sendButtonState(triggerByButton);
        mqttClient->sendRadioState(triggerByRadio);
        mqttClient->sendLimitSwState(limitSwitchHit);
        mqttClient->sendRssi(WiFi.RSSI());
    }
    delay(10);
}
