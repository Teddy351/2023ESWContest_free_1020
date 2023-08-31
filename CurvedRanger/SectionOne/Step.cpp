#include "Step.h"
#include "Arduino.h"

Step_M::Step_M(byte dir,byte step){
    pinMode(dir,OUTPUT);
    pinMode(step,OUTPUT);

    this->dir = dir;
    this->step = step;

    this->pre_step = 0;
    digitalWrite(this->dir,LOW);
    digitalWrite(this->step,LOW);
}

void Step_M::setStep(long steps){
    if(steps >= 0){
        this->STEP = steps;
        digitalWrite(this->dir,HIGH);
    }
    else{
        this->STEP = -steps;
        digitalWrite(this->dir,LOW);
    }
}

void Step_M::setDuration(long duration){
    this->duration = duration;
}

bool Step_M::Whether(){
    if(this->STEP){
        return 1;
    }
    else{
        return 0;
    }
}

void Step_M::Move(){
    if(this->STEP){
        this->cur = micros();
        if(this->cur - this->pre >= this->duration){
            this->pre = this->cur;
            if(this->state){
                this->state = 0;
            }
            else{
                this->state = 1;
                this->STEP--;
            }
            digitalWrite(this->step,this->state);
        }
    }
}

void Step_M::Just_Move(){
    digitalWrite(this->step,1);
    delayMicroseconds(this->duration);
    digitalWrite(this->step,0);
    delayMicroseconds(this->duration);
}

long Step_M::preStep(long steps){
    long last = steps - this->pre_step;
    this->pre_step = steps;
    Serial.println(last);
    return last;
}
