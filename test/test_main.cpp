#include <Arduino.h>
#include <unity.h>
#include "filteredbutton.h"
#include "statemachine.h"

void setUp(void) {
    // set stuff up here

}

void tearDown(void) {
    // clean stuff up here
}

void testFilteredButton(){
    FilteredButton b;

    TEST_ASSERT_FALSE(b.isOn(true));
    TEST_ASSERT_FALSE(b.isOn(true));
    TEST_ASSERT_TRUE(b.isOn(true));
    TEST_ASSERT_FALSE(b.isOn(true));
    TEST_ASSERT_FALSE(b.isOn(true));
    TEST_ASSERT_FALSE(b.isOn(false));
    TEST_ASSERT_FALSE(b.isOn(true));
    TEST_ASSERT_FALSE(b.isOn(true));
    TEST_ASSERT_TRUE(b.isOn(true));
    TEST_ASSERT_FALSE(b.isOn(true));
}

void testCallback(DoorState state, bool activateOutput){
  Serial.println(StateStrings.at(state).c_str());
}

void testStateMachineCloseOpen(){
    StateMachine s(testCallback, 3);

    TEST_ASSERT_EQUAL(DoorState::Open, s.getState());
    s.operate(true, DoorState::Undefined, false);//trigger closing
    TEST_ASSERT_EQUAL(DoorState::Closing, s.getState());
    s.operate(false, DoorState::Undefined, false);//do 3 ticks
    TEST_ASSERT_EQUAL(DoorState::Closing, s.getState());
    s.operate(false, DoorState::Undefined, false);
    TEST_ASSERT_EQUAL(DoorState::Closing, s.getState());
    s.operate(false, DoorState::Undefined, false);
    TEST_ASSERT_EQUAL(DoorState::Closing, s.getState());
    s.operate(false, DoorState::Undefined, false);//now should be closed
    TEST_ASSERT_EQUAL(DoorState::Closed, s.getState());    

    //now open again
    TEST_ASSERT_EQUAL(DoorState::Closed, s.getState());
    s.operate(true, DoorState::Undefined, false);//trigger closing
    TEST_ASSERT_EQUAL(DoorState::Opening, s.getState());
    s.operate(false, DoorState::Undefined, false);//do 3 ticks
    TEST_ASSERT_EQUAL(DoorState::Opening, s.getState());
    s.operate(false, DoorState::Undefined, false);
    TEST_ASSERT_EQUAL(DoorState::Opening, s.getState());
    s.operate(false, DoorState::Undefined, false);
    TEST_ASSERT_EQUAL(DoorState::Opening, s.getState());
    s.operate(false, DoorState::Undefined, false);//now should be closed
    TEST_ASSERT_EQUAL(DoorState::Open, s.getState());    

}

void testStateMachineCloseReset(){
    StateMachine s(testCallback, 3, 3);

    //after 3 ticks in open state with endswitch asserted, must go into closed state 
    s.operate(false, DoorState::Undefined, true);//do 3 ticks
    TEST_ASSERT_EQUAL(DoorState::Open, s.getState());
    s.operate(false, DoorState::Undefined, true);
    TEST_ASSERT_EQUAL(DoorState::Open, s.getState());
    s.operate(false, DoorState::Undefined, true);
    TEST_ASSERT_EQUAL(DoorState::Open, s.getState());
    s.operate(false, DoorState::Undefined, true);//now should be closed
    TEST_ASSERT_EQUAL(DoorState::Closed, s.getState());    
}

void testStateMachineCloseResetInterupted(){
    StateMachine s(testCallback, 3, 3);

    //after 3 ticks in open state with endswitch asserted, must go into closed state 
    //deasserting limit switch restarts the timeout
    s.operate(false, DoorState::Undefined, true);//do 3 ticks
    TEST_ASSERT_EQUAL(DoorState::Open, s.getState());
    s.operate(false, DoorState::Undefined, true);
    TEST_ASSERT_EQUAL(DoorState::Open, s.getState());
    s.operate(false, DoorState::Undefined, false);
    TEST_ASSERT_EQUAL(DoorState::Open, s.getState());
    s.operate(false, DoorState::Undefined, true);
    TEST_ASSERT_EQUAL(DoorState::Open, s.getState());
    s.operate(false, DoorState::Undefined, true);
    TEST_ASSERT_EQUAL(DoorState::Open, s.getState());
    s.operate(false, DoorState::Undefined, true);
    TEST_ASSERT_EQUAL(DoorState::Open, s.getState());
    s.operate(false, DoorState::Undefined, true);//now should be closed
    TEST_ASSERT_EQUAL(DoorState::Closed, s.getState());    
}

void testStateMachineManualOpen(){
    StateMachine s(testCallback, 3, 5);

    //after 3 ticks in open state with endswitch asserted, must go into closed state 
    s.operate(false, DoorState::Undefined, true);//do 5 ticks, to get s into closed state
    TEST_ASSERT_EQUAL(DoorState::Open, s.getState());
    s.operate(false, DoorState::Undefined, true);
    TEST_ASSERT_EQUAL(DoorState::Open, s.getState());
    s.operate(false, DoorState::Undefined, true);
    TEST_ASSERT_EQUAL(DoorState::Open, s.getState());
    s.operate(false, DoorState::Undefined, true);
    TEST_ASSERT_EQUAL(DoorState::Open, s.getState());
    s.operate(false, DoorState::Undefined, true);
    TEST_ASSERT_EQUAL(DoorState::Open, s.getState());
    s.operate(false, DoorState::Undefined, true);//now should be closed
    TEST_ASSERT_EQUAL(DoorState::Closed, s.getState());    
    //this is the default state in real life

    //now from here try to open the gate. use filteredButton to suppress too long manual presses
    //due to inertia, the limit switch is still active some time
    FilteredButton button;
    s.operate(button.isOn(true), DoorState::Undefined, true);
    TEST_ASSERT_EQUAL(DoorState::Closed, s.getState());
    s.operate(button.isOn(true), DoorState::Undefined, true);
    TEST_ASSERT_EQUAL(DoorState::Closed, s.getState());
    s.operate(button.isOn(true), DoorState::Undefined, true);//button triggers after 3 ticks
    TEST_ASSERT_EQUAL(DoorState::Opening, s.getState());    
    s.operate(button.isOn(true), DoorState::Undefined, true);
    TEST_ASSERT_EQUAL(DoorState::Opening, s.getState());    
    s.operate(button.isOn(true), DoorState::Undefined, false);//limit switch releases after 4 ticks
    TEST_ASSERT_EQUAL(DoorState::Opening, s.getState());    
    s.operate(button.isOn(true), DoorState::Undefined, false);
    TEST_ASSERT_EQUAL(DoorState::Opening, s.getState());    
    s.operate(button.isOn(true), DoorState::Undefined, false);
    TEST_ASSERT_EQUAL(DoorState::Open, s.getState());    
    //now gate is open
}

void testStateMachineMqttOpen(){
    StateMachine s(testCallback, 3, 5);

    //after 3 ticks in open state with endswitch asserted, must go into closed state 
    s.operate(false, DoorState::Undefined, true);//do 5 ticks, to get s into closed state
    TEST_ASSERT_EQUAL(DoorState::Open, s.getState());
    s.operate(false, DoorState::Undefined, true);
    TEST_ASSERT_EQUAL(DoorState::Open, s.getState());
    s.operate(false, DoorState::Undefined, true);
    TEST_ASSERT_EQUAL(DoorState::Open, s.getState());
    s.operate(false, DoorState::Undefined, true);
    TEST_ASSERT_EQUAL(DoorState::Open, s.getState());
    s.operate(false, DoorState::Undefined, true);
    TEST_ASSERT_EQUAL(DoorState::Open, s.getState());
    s.operate(false, DoorState::Undefined, true);//now should be closed
    TEST_ASSERT_EQUAL(DoorState::Closed, s.getState());    
    //this is the default state in real life

    //now from here try to open the gate. 
    //due to inertia, the limit switch is still active some time
    s.operate(false, DoorState::Open, true);//Mqtt cmd is sent only once
    TEST_ASSERT_EQUAL(DoorState::Opening, s.getState());
    TEST_ASSERT_EQUAL(DoorState::Open, s.getTarget());
    s.operate(false, DoorState::Undefined, true);
    TEST_ASSERT_EQUAL(DoorState::Opening, s.getState());
    TEST_ASSERT_EQUAL(DoorState::Open, s.getTarget());
    s.operate(false, DoorState::Undefined, true);
    TEST_ASSERT_EQUAL(DoorState::Opening, s.getState());    
    TEST_ASSERT_EQUAL(DoorState::Open, s.getTarget());
    s.operate(false, DoorState::Undefined, true);
    TEST_ASSERT_EQUAL(DoorState::Opening, s.getState());    
    TEST_ASSERT_EQUAL(DoorState::Open, s.getTarget());
    s.operate(false, DoorState::Undefined, false);//limit switch releases after 4 ticks
    TEST_ASSERT_EQUAL(DoorState::Open, s.getState());    
    TEST_ASSERT_EQUAL(DoorState::Open, s.getTarget());
    //now gate is open
}

void testStateMachineMqttClose(){
    StateMachine s(testCallback, 3, 5);

    s.operate(false, DoorState::Closed, true);//Mqtt cmd is sent only once
    TEST_ASSERT_EQUAL(DoorState::Closing, s.getState());
    TEST_ASSERT_EQUAL(DoorState::Closed, s.getTarget());
    s.operate(false, DoorState::Undefined, true);
    TEST_ASSERT_EQUAL(DoorState::Closing, s.getState());
    TEST_ASSERT_EQUAL(DoorState::Closed, s.getTarget());
    s.operate(false, DoorState::Undefined, true);
    TEST_ASSERT_EQUAL(DoorState::Closing, s.getState());    
    TEST_ASSERT_EQUAL(DoorState::Closed, s.getTarget());
    s.operate(false, DoorState::Undefined, true);
    TEST_ASSERT_EQUAL(DoorState::Closing, s.getState());    
    TEST_ASSERT_EQUAL(DoorState::Closed, s.getTarget());
    s.operate(false, DoorState::Undefined, false);//limit switch releases after 4 ticks
    TEST_ASSERT_EQUAL(DoorState::Closed, s.getState());    
    TEST_ASSERT_EQUAL(DoorState::Closed, s.getTarget());
    //now gate is open
}

void setup()
{
    delay(2000); // service delay
    UNITY_BEGIN();

    RUN_TEST(testFilteredButton);
    RUN_TEST(testStateMachineCloseOpen);
    RUN_TEST(testStateMachineCloseReset);
    RUN_TEST(testStateMachineCloseResetInterupted);
    RUN_TEST(testStateMachineManualOpen);
    RUN_TEST(testStateMachineMqttOpen);
    RUN_TEST(testStateMachineMqttClose);

    UNITY_END(); // stop unit testing
}

void loop()
{
}