#ifndef FILTEREDBUTTON_H
#define FILTEREDBUTTON_H

//helper to suppress button ring. triggers only once on a longer button press.
class FilteredButton{
    const int filterTicks;
    int state = 0;
public:
    FilteredButton(int filterLenInTicks = 3):filterTicks(filterLenInTicks){}
    bool isOn(bool currentInput){
        if(currentInput){
            state++;
        }
        else{
            state = 0;
        }
        if(state==filterTicks){
            return true;
        }
        return false;
    }
};

#endif