#define DebugFunk
#define DebugKalibrieren

// include the library code:
#include <LiquidCrystal.h>
#include <LCDKeypad.h>
#include <SPI.h>
#include <RH_NRF24.h>
#include <estEEprom.h>
#include "Joy.h"
// initialize the library with the numbers of the interface pins
//LCDKeypad lcd;
LCDKeypad lcd(A0, 8, 9, 4, 5, 6, 7);
// Singleton instance of the radio driver
RH_NRF24 nrf24(A7,10);
const uint32_t timRadioMinIntervall =    10000;
const uint32_t timRadioMaxIntervall = 10000000;
static uint32_t timRadio = 0-timRadioMaxIntervall;

Joy joyLV (A2);
Joy joyMV (A1);
Joy joyLH (A6);
Joy joyMH (A3);
Joy *joyArray[] = {&joyLV,&joyMV,&joyLH,&joyMH};
const uint8_t joyArrayCount = 4;
void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Spiel mit mir!");
   Serial.begin(115200);
  while (!Serial)
    ; // wait for serial port to connect. Needed for Leonardo only
 if (!nrf24.init())
    Serial.println("init failed");
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");
  pinMode(joyLV.pin(), INPUT);
  pinMode(joyMV.pin(), INPUT);
  pinMode(joyLH.pin(), INPUT);
  pinMode(joyMH.pin(), INPUT);
  analogWrite(3,200);
  ReadFromEEprom();
  while (millis()<1000) {    
  }
  Ebene0Init();
}

void radioSend() 
{
  #ifdef DebugFunk
    Serial.print(timRadio);
    Serial.print(" - Sending: LV="); Serial.print(joyLV.pos()); Serial.print("="); Serial.print(joyLV.micros());
    Serial.print(           " MV="); Serial.print(joyMV.pos()); Serial.print("="); Serial.print(joyMV.micros());
    Serial.print(           " LH="); Serial.print(joyLH.pos()); Serial.print("="); Serial.print(joyLH.micros());
    Serial.print(           " MH="); Serial.print(joyMH.pos()); Serial.print("="); Serial.print(joyMH.micros());
  #endif // DebugFunk
  timRadio = micros();
  uint8_t data[] = {1, joyLV.micros() & 0xFF, joyLV.micros()>>8 & 0xFF,  
                       joyMV.micros() & 0xFF, joyMV.micros()>>8 & 0xFF, 
                       joyLH.micros() & 0xFF, joyLH.micros()>>8 & 0xFF, 
                       joyMH.micros() & 0xFF, joyMH.micros()>>8 & 0xFF};
  joyLV.changed=false;
  joyMV.changed=false;
  joyLH.changed=false;
  joyMH.changed=false;
  //Serial.print("; ");Serial.print(micros()-timRadio);
  // Send a message to nrf24_server
  nrf24.send(data, sizeof(data));
  //Serial.print("; ");Serial.print(micros()-timRadio);
  nrf24.waitPacketSent();
  //Serial.print("; ");Serial.print(micros()-timRadio);
  nrf24.setModeRx();
  #ifdef DebugFunk
    Serial.print("; ");Serial.print(micros()-timRadio);
    Serial.println("ns");
  #endif
}

void joyCheck(Joy &joy) 
{ 
  joy.setPos(analogRead(joy.pin()));
}

void lcdPrintByte(int joyVar) {
  if (joyVar<10)
    lcd.print(F("   "));
  else if (joyVar<100)
    lcd.print(F("  "));
  else if (joyVar<1000)
    lcd.print(F(" "));
  lcd.print(joyVar);
}


//uint8_t Display = 0;
//const uint8_t DisplayJoyPos = 0;
//const uint8_t DisplayJoyKalibrieren = 1;
boolean bBlock = false;
void (*DisplayLoop)() = 0;
#define LV 1
#define MV 2
#define LH 3
#define MH 4
uint8_t Ebene0 = 0;

void loop() {
  DisplayLoop();
  //Wenn Tasten blockiert aber keine mehr gedückt, Blockierung aufheben
  if (bBlock && lcd.button()==KEYPAD_NONE) 
    bBlock = false;

  if (   (   micros() - timRadio >= timRadioMinIntervall 
          && (   joyLV.changed
              || joyMV.changed
              || joyLH.changed
              || joyMH.changed 
             )
         )
      || (micros() - timRadio >= timRadioMaxIntervall 
         )
     ) {
    radioSend();
  }
  uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  if (nrf24.available()) {
    // If there is a reply message for us now
    if (nrf24.recv(buf, &len))
    {
      Serial.print("got reply: ");
      Serial.println((char*)buf);
    }
  }
}

void Ebene0Init(){
  lcd.clear();
  if (Ebene0 == 5) {
    lcd.print( F( "Speichern"));
  } else if (Ebene0 > 0) {
    lcd.print( F( "Kalibriere "));
    switch (Ebene0) {
      case LV:
        Serial.println(F("Ebene0=LV"));
        lcd.print(F("LV"));
        break;
      case LH:
        Serial.println(F("Ebene0=LH"));
        lcd.print(F("LH"));
        break;
      case MV:
        Serial.println(F("Ebene=MV"));
        lcd.print(F("MV"));
        break;
      case MH:
        Serial.println(F("Ebene0=MH"));
        lcd.print(F("MH"));
        break;
    }
  }
  DisplayLoop = &Ebene0Loop;
}
#define Ebene0Max 5
void Ebene0Loop(){
  if        (bBlock == false && (lcd.button()==KEYPAD_UP || lcd.button()==KEYPAD_DOWN)) {
    bBlock = true;
    if (lcd.button()==KEYPAD_UP) {
      if (Ebene0 == 0)
        Ebene0 = Ebene0Max;
      else
        Ebene0--;
    } else {
      if (Ebene0 == Ebene0Max)
        Ebene0=0;
      else
        Ebene0++;
    }
    Ebene0Init();
  } else if (   bBlock == false //Wenn Taste frisch gedrückt
             && Ebene0 > 0 && Ebene0 < 5 //nicht in Normalanzeige
             && lcd.button()==KEYPAD_RIGHT) {
    bBlock = true;
    #ifdef DebugKalibrieren
      Serial.println(F("Ebene1KalibrierenInit"));
    #endif
    Ebene1KalibrierenInit();
  } else if (   bBlock == false //Wenn Taste frisch gedrückt      
             && Ebene0 == 5     //nicht in Normalanzeige
             && lcd.button()==KEYPAD_RIGHT) {
    bBlock = true;
    Serial.println(F("saveToEprom"));
    saveToEprom();
    lcd.setCursor(10, 1);
    lcd.print(F("OK"));
  } else { //Normalanzeige
    if (Ebene0 == 0) {
      joyCheck(joyLV);
      joyCheck(joyMV);
      joyCheck(joyLH);
      joyCheck(joyMH);
      lcd.setCursor(0, 0);
      lcdPrintByte(joyLV.pos());
      lcd.print(F(":"));
      lcdPrintByte(joyMV.pos());
      //  lcd.print(F("  "));
      lcd.setCursor(0, 1);
      lcdPrintByte(joyLH.pos());
      lcd.print(F(":"));
      lcdPrintByte(joyMH.pos());
    } //else muß sich nix bewegen    
  }
}

void saveToEprom() {
  for (uint8_t i = 0; i< joyArrayCount; i++) {
    EEPROM.updateWord(i * 12 +  0, (*joyArray[i]).posMin   );
    EEPROM.updateWord(i * 12 +  2, (*joyArray[i]).posMid   );
    EEPROM.updateWord(i * 12 +  4, (*joyArray[i]).posMax   );
    EEPROM.updateWord(i * 12 +  6, (*joyArray[i]).microsMin);
    EEPROM.updateWord(i * 12 +  8, (*joyArray[i]).microsMid);
    EEPROM.updateWord(i * 12 + 10, (*joyArray[i]).microsMax);
  }  
}
void ReadFromEEprom(){
  Serial.println(F("ReadFromEEprom"));
  for (uint8_t i = 0; i< joyArrayCount; i++) {
    (*joyArray[i]).posMin   =EEPROM.readWord(i * 12 +  0);
    (*joyArray[i]).posMid   =EEPROM.readWord(i * 12 +  2);
    (*joyArray[i]).posMax   =EEPROM.readWord(i * 12 +  4);
    (*joyArray[i]).microsMin=EEPROM.readWord(i * 12 +  6);
    (*joyArray[i]).microsMid=EEPROM.readWord(i * 12 +  8);
    (*joyArray[i]).microsMax=EEPROM.readWord(i * 12 + 10);
    
    #ifdef DebugKalibrieren
      Serial.print(F("i="));Serial.print(i);Serial.print(F(": "));
      Serial.print((*joyArray[i]).posMin   );Serial.print(F("-"));
      Serial.print((*joyArray[i]).posMid   );Serial.print(F("-"));
      Serial.print((*joyArray[i]).posMax   );Serial.print(F(" ; "));
      Serial.print((*joyArray[i]).microsMin);Serial.print(F("-"));
      Serial.print((*joyArray[i]).microsMid);Serial.print(F("-"));
      Serial.print((*joyArray[i]).microsMax);Serial.println();
    #endif
  } 
}

uint8_t Ebene1 = 0;
#define Ebene1Max 1
#define Ebene1Kalibrieren_Joy 0
#define Ebene1Kalibrieren_Servo 1
void Ebene1KalibrierenInit(){
  lcd.setCursor(0,1);
  switch (Ebene1) {
    case Ebene1Kalibrieren_Joy:
      lcd.print(F("Joyst           "));
      break;
    case Ebene1Kalibrieren_Servo:
      lcd.print(F("Servo           "));
      break;
  }
  DisplayLoop = &Ebene1KalibrierenLoop;
}
void Ebene1KalibrierenLoop(){
  if        (bBlock == false && (lcd.button()==KEYPAD_UP || lcd.button()==KEYPAD_DOWN)) {
    bBlock = true;
    if (lcd.button()==KEYPAD_UP) {
      if (Ebene1 == 0)
        Ebene1 = Ebene1Max;
      else
        Ebene1--;
      } else {
      if (Ebene1 == Ebene1Max)
        Ebene1=0;
      else
        Ebene1++;
    }
    Ebene1KalibrierenInit();
  } else if (   bBlock == false //Wenn Taste frisch gedrückt
             && lcd.button()==KEYPAD_LEFT) {
    bBlock = true;
    Ebene0Init();
  } else if (   bBlock == false //Wenn Taste frisch gedrückt
             && lcd.button()==KEYPAD_RIGHT) {
    bBlock = true;
    Ebene2KalibrierenInit();
  }
}

uint8_t Ebene2 = 0;
#define Ebene2Max 2
#define Ebene2Kalibrieren_Min 0
#define Ebene2Kalibrieren_Mid 1
#define Ebene2Kalibrieren_Max 2
void Ebene2KalibrierenInit(){
  lcd.setCursor(7,1);
  switch (Ebene2) {
    case Ebene2Kalibrieren_Min:
      lcd.print(F("Min"));
      break;
    case Ebene2Kalibrieren_Mid:
      lcd.print(F("Mid"));
      break;
    case Ebene2Kalibrieren_Max:
      lcd.print(F("Max"));
      break;
  }
  lcd.print(F("      "));
  DisplayLoop = &Ebene2KalibrierenLoop;
}
void Ebene2KalibrierenLoop(){
  if        (bBlock == false && (lcd.button()==KEYPAD_UP || lcd.button()==KEYPAD_DOWN)) {
    bBlock = true;
    if (lcd.button()==KEYPAD_UP) {
      if (Ebene2 == 0)
        Ebene2 = Ebene2Max;
      else
        Ebene2--;
      } else {
      if (Ebene2 == Ebene2Max)
        Ebene2=0;
      else
        Ebene2++;
    }
    Ebene2KalibrierenInit();
  } else if (   bBlock == false //Wenn Taste frisch gedrückt
  && lcd.button()==KEYPAD_LEFT) {
    bBlock = true;
    Ebene1KalibrierenInit();
  } else if (   bBlock == false //Wenn Taste frisch gedrückt
  && lcd.button()==KEYPAD_RIGHT) {
    bBlock = true;
    Ebene3KalibrierenInit();
  } 
}

int Ebene3KalibrierenOld   = 0;
int Ebene3KalibrierenValue = 0;
void Ebene3KalibrierenInit(){
  if (Ebene1 == Ebene1Kalibrieren_Servo) {
    switch (Ebene2) {
      case Ebene2Kalibrieren_Min:
        Ebene3KalibrierenValue=Ebene3KalibrierenOld=(*joyArray[Ebene0-LV]).microsMin;
        (*joyArray[Ebene0-LV]).setPos((*joyArray[Ebene0-LV]).posMin);
        break;
      case Ebene2Kalibrieren_Mid:
        Ebene3KalibrierenValue=Ebene3KalibrierenOld=(*joyArray[Ebene0-LV]).microsMid;
        (*joyArray[Ebene0-LV]).setPos((*joyArray[Ebene0-LV]).posMid);
        break;
      case Ebene2Kalibrieren_Max:
        Ebene3KalibrierenValue=Ebene3KalibrierenOld=(*joyArray[Ebene0-LV]).microsMax;
        (*joyArray[Ebene0-LV]).setPos((*joyArray[Ebene0-LV]).posMax);
        break;
    }
  }
  DisplayLoop = &Ebene3KalibrierenLoop;
}

void Ebene3KalibrierenSetMicros(uint16_t Value) 
{
	 switch (Ebene2){
     case Ebene2Kalibrieren_Min:
       (*joyArray[Ebene0-LV]).microsMin = Value;
       break;
     case Ebene2Kalibrieren_Mid:
       (*joyArray[Ebene0-LV]).microsMid = Value;
       break;
     case Ebene2Kalibrieren_Max:
       (*joyArray[Ebene0-LV]).microsMax = Value;
       break;
   }
   (*joyArray[Ebene0-LV]).refresh();
}

void Ebene3KalibrierenLoop(){
  if (   bBlock == false //Wenn Taste frisch gedrückt
      && lcd.button()==KEYPAD_LEFT) {
    bBlock = true;
    if (Ebene1==Ebene1Kalibrieren_Servo)
      Ebene3KalibrierenSetMicros(Ebene3KalibrierenOld);
    Ebene2KalibrierenInit();
  }
  else 
  if (   bBlock == false //Wenn Taste frisch gedrückt
      && lcd.button()==KEYPAD_RIGHT) {
    bBlock = true;
    if (Ebene1 == Ebene1Kalibrieren_Joy) {
      switch (Ebene2) {
        case Ebene2Kalibrieren_Min:
        (*joyArray[Ebene0-LV]).posMin = Ebene3KalibrierenValue;
        break;
        case Ebene2Kalibrieren_Mid:
        (*joyArray[Ebene0-LV]).posMid = Ebene3KalibrierenValue;
        break;
        case Ebene2Kalibrieren_Max:
        (*joyArray[Ebene0-LV]).posMax = Ebene3KalibrierenValue;
        break;
      }
    }
    Ebene2KalibrierenInit();
  } 
  else
  if (   bBlock == false //Wenn Taste frisch gedrückt
      && Ebene1 == Ebene1Kalibrieren_Servo
      && lcd.button()==KEYPAD_UP ){
    #ifdef DebugKalibrieren
      Serial.println(F("KEYPAD_UP"));
    #endif
    bBlock = true;
    Ebene3KalibrierenValue++;
    Ebene3KalibrierenSetMicros(Ebene3KalibrierenValue);
  }
  else
  if (   bBlock == false //Wenn Taste frisch gedrückt
      && Ebene1 == Ebene1Kalibrieren_Servo
      && lcd.button()==KEYPAD_DOWN) {
    #ifdef DebugKalibrieren
    Serial.println(F("KEYPAD_DOWN"));
    #endif
    bBlock = true;
    Ebene3KalibrierenValue--;
    Ebene3KalibrierenSetMicros(Ebene3KalibrierenValue);
  } else {
    if (Ebene1 == Ebene1Kalibrieren_Joy) {
      Ebene3KalibrierenValue=analogRead((*joyArray[Ebene0-LV]).pin());
    } 
    lcd.setCursor(12,1);
    lcdPrintByte(Ebene3KalibrierenValue);
  }
}

