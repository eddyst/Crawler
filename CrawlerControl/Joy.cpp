/* 
* Joy.cpp
*
* Created: 30.07.2014 15:48:18
* Author: Steier
*/


#include "Joy.h"

// default constructor
Joy::Joy(uint8_t pin)
{ posMin = 0;
  posMid = 508;
  posMax = 1023;
  microsMin = 1000;
  microsMid = 1420;
  microsMax = 1800;
  _pin=pin;
  _pos = posMid;
  _micros = microsMid;
  changed = false;
} //Joy

// default destructor
Joy::~Joy()
{
} //~Joy
uint8_t Joy::pin() {
  return _pin;
}
void Joy::setPos(int value) {
  if( abs(_pos - value) > 2 ) {
    _pos = value;
    refresh();
  }  
}

void Joy::refresh() {
  int _new;
  if (abs(_pos - posMid) < 3)
  _new = microsMid;
  else if (_pos < posMid)
  _new = map(_pos,posMin,posMid,microsMin,microsMid);
  else
  _new = map(_pos,posMax,posMid,microsMax,microsMid);
  if (_new != _micros) {
    _micros = _new;
    changed = true;
  }  
}