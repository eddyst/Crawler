/*
  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include <LiquidCrystal.h>
#include <LCDKeypad.h>
#include <SPI.h>
#include <RH_NRF24.h>

// initialize the library with the numbers of the interface pins
//LCDKeypad lcd;
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
// Singleton instance of the radio driver
RH_NRF24 nrf24(A7,10);
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
    if (!nrf24.init())
    Serial.println("init failed");
    // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
    if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
    if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");

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
    // Send a message to nrf24_server
    nrf24.send(data, sizeof(data));
    Serial.print("; ");Serial.print(micros()-timRadio);
    nrf24.waitPacketSent();
    Serial.print("; ");Serial.print(micros()-timRadio);
    nrf24.setModeRx();
    Serial.print("; ");Serial.print(micros()-timRadio);
    Serial.println("ns");
}

void joyCheck(int Pin, byte &joyVar, byte Msg) 
{ 
  byte joyNew = map(analogRead(Pin),0,1023,0,255);
	if (joyNew != joyVar) {
/*    Serial.print  ("joyCheck ");
    Serial.print  (joyVar);
    Serial.print  (";"); 
    Serial.print  (joyNew);
    Serial.println(" !=");
*/    radioSend(Msg,joyNew);
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
  // Now wait for a reply
  uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  if (nrf24.available()) {
    // If there is a reply message for us now
    if (nrf24.recv(buf, &len))
    {
      Serial.print("got reply: ");
      Serial.println((char*)buf);
    }    }
}

