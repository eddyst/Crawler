// include the library code:
#include <LiquidCrystal.h>
#include <LCDKeypad.h>
#include <SPI.h>
#include <RH_NRF24.h>
#include "Joy.h"
// initialize the library with the numbers of the interface pins
//LCDKeypad lcd;
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

const uint32_t timRadioMinIntervall =   20000;
const uint32_t timRadioMaxIntervall = 1000000;
static uint32_t timRadio = 0-timRadioMaxIntervall;

Joy joyLX (A1);
Joy joyLY (A2);
Joy joyRX (A3);
Joy joyRY (A6);

void setup() {
  Serial.begin(57600);
  while (!Serial)
    ; // wait for serial port to connect. Needed for Leonardo only

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Spiel mit mir!");
  pinMode(joyLX.pin(), INPUT);
  pinMode(joyLY.pin(), INPUT);
  pinMode(joyRX.pin(), INPUT);
  pinMode(joyRY.pin(), INPUT);
  analogWrite(3,200);
}

void radioSend() 
{
	  timRadio = micros();
    Serial.print(timRadio);
    Serial.print(" - Sending: ");
    uint8_t data[] = {1, joyLX.pos(), joyLY.pos(), joyRX.pos(), joyRY.pos()};
    Serial.print("; ");Serial.print(micros()-timRadio);
    // Send a message 
    
    Serial.print("; ");Serial.print(micros()-timRadio);
    Serial.println("ns");
}

void joyCheck(Joy &joy) 
{ 
  joy.setPos(map(analogRead(joy.pin()),0,1023,0,255));
}

void lcdPrintByte(byte joyVar) {
  if (joyVar<10)
    lcd.print(F("  "));
  else if (joyVar<100)
    lcd.print(F(" "));
  lcd.print(joyVar);
}




void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  joyCheck(joyLX);
  joyCheck(joyLY);
  joyCheck(joyRX);
  joyCheck(joyRY);
  
  lcd.setCursor(0, 1);
  lcdPrintByte(joyLX.pos());
  lcd.print(F(":"));
  lcdPrintByte(joyLY.pos());
  lcd.print(F("  "));
  lcdPrintByte(joyRX.pos());
  lcd.print(F(":"));
  lcdPrintByte(joyRY.pos());
  
  if (   (   micros() - timRadio >= timRadioMinIntervall 
          && (   joyLX.changed
              || joyLY.changed
              || joyRX.changed
              || joyRY.changed )                      )
      || (micros() - timRadio >= timRadioMaxIntervall )
     ) {
    radioSend();
  }
}

