/* 
* Joy.h
*
* Created: 30.07.2014 15:48:19
* Author: Steier
*/

#include <arduino.h>

#ifndef __JOY_H__
#define __JOY_H__


class Joy
{
//variables
public:
  boolean changed;
  uint8_t _pin;
  int posMin;
  int posMid;
  int posMax;
  int microsMin;
  int microsMid;
  int microsMax;
protected:
private:
  int _pos;
  int _micros;
//functions
public:
	Joy(const uint8_t pin);
	~Joy();
  uint8_t pin();
  int pos()    {return _pos;   }
  int micros() {return _micros;}
  void setPos(int);
  void refresh();
protected:
private:
	Joy( const Joy &c );
	Joy& operator=( const Joy &c );
}; //Joy

#endif //__JOY_H__
