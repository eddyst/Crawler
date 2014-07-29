// include the library code:
#include <LiquidCrystal.h>
#include <LCDKeypad.h>
#include <SPI.h>
#include <RH_NRF24.h>

// initialize the library with the numbers of the interface pins
//LCDKeypad lcd;
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

const uint32_t timRadioIntervall = 1000000;
static uint32_t timRadio = 0-timRadioIntervall;

int pinLX = A1;
int pinLY = A2;
byte joyLX = 258;
byte joyLY = 254;
int pinRX = A3;
int pinRY = A6;
byte joyRX = 258;
byte joyRY = 254;

void setup() {
  Serial.begin(57600);
  while (!Serial)
    ; // wait for serial port to connect. Needed for Leonardo only

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Spiel mit mir!");
  pinMode(pinLX, INPUT);
  pinMode(pinLY, INPUT);
  pinMode(pinRX, INPUT);
  pinMode(pinRY, INPUT);
  analogWrite(3,200);
}

void radioSend(byte Message, byte Value ) 
{
	  timRadio = micros();
    Serial.print(timRadio);
    Serial.print(" - Sending: (");
    Serial.print(Message);
    Serial.print(", ");
    Serial.print(Value);
    Serial.print(") ");Serial.print(micros()-timRadio);
    uint8_t data[] = {Message, Value};
    Serial.print("; ");Serial.print(micros()-timRadio);
    // Send a message 
    
    Serial.print("; ");Serial.print(micros()-timRadio);
    Serial.println("ns");
}

void joyCheck(int Pin, byte &joyVar, byte Msg) 
{ 
  byte joyNew = map(analogRead(Pin),0,1023,0,255);
	if (joyNew != joyVar) {
    radioSend(Msg,joyNew);
    joyVar = joyNew;
  }  
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
  joyCheck(pinLX, joyLX, 10);
  joyCheck(pinLY, joyLY, 11);
  joyCheck(pinRX, joyRX, 21);
  joyCheck(pinRY, joyRY, 22);

  lcd.setCursor(0, 1);
  lcdPrintByte(joyLX);
  lcd.print(F(":"));
  lcdPrintByte(joyLY);
  lcd.print(F("  "));
  lcdPrintByte(joyRX);
  lcd.print(F(":"));
  lcdPrintByte(joyRY);

  if ( micros() - timRadio >= timRadioIntervall) {
    radioSend(0, 0); //Send a Keep alive Msg
  }

}

