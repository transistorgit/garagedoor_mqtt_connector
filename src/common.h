#ifndef TOR_COMMON_H
#define TOR_COMMON_H

#include <map>
#include <string>

enum class DoorState: int{
    Undefined,
    Closed,
    Opening,
    Stopped,
    Open,
    Closing
};

//map mqtt cmds to doorstates
const std::map<DoorState,std::string> MqttStateStrings = {
    { DoorState::Undefined, "Undefined"},
    { DoorState::Closed, "Close"},
    { DoorState::Open, "Open"}
};


const std::map<DoorState,std::string> StateStrings = {
    { DoorState::Undefined, "Undefined"},
    { DoorState::Closed, "Closed"},
    { DoorState::Opening, "Opening"},
    { DoorState::Stopped, "Stopped"},
    { DoorState::Open, "Open"},
    { DoorState::Closing, "Closing"}
};


#endif