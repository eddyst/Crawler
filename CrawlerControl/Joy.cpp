/* 
* Joy.cpp
*
* Created: 30.07.2014 15:48:18
* Author: Steier
*/


#include "Joy.h"

// default constructor
Joy::Joy(uint8_t pin)
{ _pin=pin;
  _pos = 128;
  changed = false;
} //Joy

// default destructor
Joy::~Joy()
{
} //~Joy
uint8_t Joy::pin() {
  return _pin;
}
void Joy::setPos(byte value) {
  if( _pos != value) {
   _pos = value;
    changed = true;
  }  
}