//#define Debug

// nrf24_server.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing server
// with the RH_NRF24 class. RH_NRF24 class does not provide for addressing or
// reliability, so you should only use RH_NRF24  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example nrf24_client
// Tested on Uno with Sparkfun NRF25L01 module
// Tested on Anarduino Mini (http://www.anarduino.com/mini/) with RFM73 module
// Tested on Arduino Mega with Sparkfun WRL-00691 NRF25L01 module

#include <SPI.h>
#include <RH_NRF24.h>

// Singleton instance of the radio driver
RH_NRF24 nrf24(9,10);
// RH_NRF24 nrf24(8, 7); // use this to be electrically compatible with Mirf
// RH_NRF24 nrf24(8, 10);// For Leonardo, need explicit SS pin
// RH_NRF24 nrf24(8, 7); // For RFM73 on Anarduino Mini

#include <ServoTimer2.h>  // the servo libraMH
// create servo object to control a servo 
// twelve servo objects can be created on most boards
ServoTimer2 servoLV;  
ServoTimer2 servoMV;  
ServoTimer2 servoLH; 
ServoTimer2 servoMH;

int joyLV = 1400;
int joyMV = 1400;
int joyLH = 1400;
int joyMH = 1400;
                
void setup() 
{
  #ifdef Debug
    Serial.begin(115200);
    while (!Serial) 
      ; // wait for serial port to connect. Needed for Leonardo onMV
  #endif
  servoLV.attach(4);  // attaches the servo on pin 9 to the servo object
  servoLV.write(joyLV);
  servoMV.attach(5);
  servoMV.write(joyMV);
  servoLH.attach(6);  
  servoLH.write(joyLH);
  servoMH.attach(7);  
  servoMH.write(joyMH);
  
  if (!nrf24.init()) {
  #ifdef Debug
    Serial.println("init failed");
  #endif
  }
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  if (!nrf24.setChannel(1)) {
    #ifdef Debug
      Serial.println("setChannel failed");
    #endif
  }
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm)) {
    #ifdef Debug
      Serial.println("setRF failed");    
    #endif
  }
}

void p() {
  Serial.print(joyLV);Serial.print(":");
  Serial.print(joyMV);Serial.print("  ");
  Serial.print(joyLH);Serial.print(":");
  Serial.print(joyMH);
}
void loop()
{
  if (nrf24.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (nrf24.recv(buf, &len)) {
//      NRF24::printBuffer("request: ", buf, len);
      #ifdef Debug
        Serial.print("got request: ");
      #endif
      if (len==9 && buf[0] == 1) {
        joyLV = buf[2] << 8 | buf[1];
        joyMV = buf[4] << 8 | buf[3];
        joyLH = buf[6] << 8 | buf[5];
        joyMH = buf[8] << 8 | buf[7];
        #ifdef Debug
 //         Serial.print(buf[2]);Serial.print(","),Serial.print(buf[1]);Serial.print(";");
          p();
          Serial.println();
        #endif
      } else {
        #ifdef Debug
          Serial.println((char*)buf);
        #endif
      }
      
 //     if (servoLV.read() != joyLV) {
        servoLV.write(joyLV);              // tell servo to go to position in variable 'pos' 
 //       Serial.print  ("* ");
 //     }// else 
 //       Serial.print  ("  ");
 //     if (servoMV.read() != joyMV) {
        servoMV.write(joyMV);              // tell servo to go to position in variable 'pos' 
//        Serial.print  ("* ");
//     }// else 
//        Serial.print  ("  ");
//      if (servoLH.read() != joyLH) {
        servoLH.write(joyLH);              // tell servo to go to position in variable 'pos' 
 //       Serial.print  ("* ");
 //     }// else 
 //       Serial.print  ("  ");
//      if (servoMH.read() != joyMH) {
        servoMH.write(joyMH);              // tell servo to go to position in variable 'pos' 
 //       Serial.print  ("* ");
//      }// else 
 //       Serial.print  ("  ");
        
      // Send a reply
      uint8_t data[] = "And hello back to you";
      nrf24.send(data, sizeof(data));
      nrf24.waitPacketSent();
      Serial.println("Sent a reply");
    }
    } else {
      #ifdef Debug
        Serial.println("recv failed");
      #endif
    }
  }
}

