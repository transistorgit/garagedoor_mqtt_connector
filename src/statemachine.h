#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "common.h"
#include <functional>

class StateMachine{

    enum State{
        Closed,
        Opening,
        StoppedOpening,
        Open,
        Closing,
        StoppedClosing
    };

    const std::map<State, DoorState> intToExtState = {
        {Closed, DoorState::Closed},
        {Opening, DoorState::Opening},
        {StoppedOpening, DoorState::Stopped},
        {Open, DoorState::Open},
        {Closing, DoorState::Closing},
        {StoppedClosing, DoorState::Stopped}
    };

    const std::map<State,std::string> InternalStateStrings = {
        { State::Closed, "Closed"},
        { State::Opening, "Opening"},
        { State::StoppedOpening, "StoppedOpening"},
        { State::Open, "Open"},
        { State::Closing, "Closing"},
        { State::StoppedClosing, "StoppedClosing"}
    };

    State state {Open};
    std::function<void(DoorState, bool)> callback;

    const int resetThreshold = 60;//ticks to reset state if Close sensor is activated 
    const int openCloseTime = 250;//ticks for a full door swing
    int currentMovementDuration = 0;
public:

    StateMachine(std::function<void(DoorState, bool)> stateCallback);//set callback to get info about current state

    //call with constant time tick
    //trigger is remote sender or switch, endSwitch is end position magnet contact (reed) in closed state
    void operate(bool trigger, DoorState target, bool endSwitch);
    void changeState(State target, bool activateOutput = false);
    DoorState getState();
    DoorState getTarget();
};

#endif