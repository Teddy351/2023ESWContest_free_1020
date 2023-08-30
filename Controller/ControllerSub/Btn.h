#ifndef __BTN_H__
#define __BTN_H__
#include "Arduino.h"

#define FALL 1
#define RISE 4
#define PRESSING 2
#define UNPRESSING 0

class Btn {
  private:
    byte _pin;
    bool _cur,_pre;
  public:
    Btn(byte pin);
    Btn(byte pin,bool state);
    byte State();
    byte Joy_X();
    byte Joy_Y();
};
#endif
