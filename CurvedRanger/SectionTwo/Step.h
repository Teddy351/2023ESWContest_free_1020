#ifndef __STEP_H__
#define __STEP_H__
#include "Arduino.h"
class Step_M{
    private:
        byte dir,step;
        long STEP;
        bool DIR;
        long pre_step = 0;
        unsigned long cur = 0;
        unsigned long pre = 0;
        bool state = 0;
        long duration = 100;
    
    public:
        Step_M(byte dir,byte step);
        void setStep(long steps);
        void setDuration(long duration);
        bool Whether();
        void Move();
        void Just_Move();
        long preStep(long steps);
};
#endif
