#include "Btn.h"
#include "Arduino.h"


Btn::Btn(byte pin){
  _pin = pin;
}
Btn::Btn(byte pin,bool state){
  if(state){
    pinMode(pin,INPUT_PULLUP);
    _cur = 1;
    _pre = 1;
  }
  else{
    pinMode(pin,INPUT);
    _cur = 0;
    _pre = 0;
  }
  _pin = pin;
}

byte Btn::State(){
  byte signal;
  _cur = digitalRead(_pin);
  if(_cur == HIGH && _pre == HIGH){
    //HIGH
    signal = 0;
  }else if(_cur == LOW && _pre == HIGH){
    //FALLING
    signal = 1;
  }else if(_cur == LOW && _pre == LOW){
    //PRESSING
    signal = 2;
  }else{
    //RISING
    signal = 4;
  }
  _pre = _cur;
  return signal;
}

byte Btn::Joy_X(){
  word val = analogRead(_pin);
  if(val <= 10){
    return 0;
  }else if(val >= 1000){
    return 2;
  }else{
    return 1;
  }
}

byte Btn::Joy_Y(){
  word val = analogRead(_pin);
  if(val <= 10){
    return 0;
  }else if(val >= 1000){
    return 2;
  }else{
    return 1;
  }
}
