#include "statemachine.h"
#include "Arduino.h"

StateMachine::StateMachine(std::function<void(DoorState, bool)> stateCallback)
    :callback(stateCallback)
{
}

void StateMachine::operate(bool trigger, DoorState target, bool endSwitch){
    //Serial.print(currentMovementDuration);
    //Serial.print(trigger?" SM:Triggered ": " SM:not triggered ");
    //Serial.print(StateStrings.at(target).c_str());
    //Serial.print(" -- Current: ");
    //Serial.print(InternalStateStrings.at(state).c_str());
    //Serial.println(endSwitch?" SM:endsw ": " SM:no endsw ");

    //first handle reset condition, valid in all states
    if(endSwitch && currentMovementDuration>resetThreshold && state!=Closed ){
        changeState(Closed);
        return;
    }

    switch(state){
        case Open:
            currentMovementDuration = 0;
            if(trigger || target == DoorState::Closed){
                changeState(Closing, target == DoorState::Closed);
            }
        break;
        case Closing:
            currentMovementDuration++;
            if(endSwitch){
                //handle endSwitch immediately for faster feedback
                changeState(Closed);
            }else if(trigger){
                changeState(StoppedClosing);
            }else if(currentMovementDuration > openCloseTime){
                //remove this elseif, if state change should be by endSwitch only
                changeState(Closed);
            }
        break;
        case StoppedClosing:
            if (trigger || target == DoorState::Open)
            {
                currentMovementDuration = openCloseTime - currentMovementDuration; //change counting direction, as next movement will be opposite dir
                changeState(Opening);
            }
        break;
        case Closed:
            currentMovementDuration = 0;
            if(trigger || target == DoorState::Open)
            {
                changeState(Opening, target == DoorState::Open);
            }
        break;
        case Opening:
            currentMovementDuration++;
            if(trigger)
            {
                changeState(StoppedOpening);
            }
            if(currentMovementDuration > openCloseTime)
            {
                changeState(Open);
            }
        break;
        case StoppedOpening:
            if(trigger || target == DoorState::Closed){
                currentMovementDuration = openCloseTime - currentMovementDuration; //change counting direction, as next movement will be opposite dir
                changeState(Closing);
            }
        break;
    }
}

void StateMachine::changeState(State target, bool activateOutput){
    //Serial.print(StateStrings.at(intToExtState.at(target)).c_str());
    //Serial.println(activateOutput?" SM:trigger ": " SM:no trigger ");

    state = target;
    if(callback){
        callback(intToExtState.at(target), activateOutput);
    }
}

DoorState StateMachine::getState(){
    return intToExtState.at(state);
}

DoorState StateMachine::getTarget(){
    switch(state){
        case Opening:
        case StoppedOpening:
        case Open:
            return intToExtState.at(Open);
        case Closing:
        case StoppedClosing:
        case Closed:
            return intToExtState.at(Closed);
    }
    return intToExtState.at(Open);
}