// -------------------------------------------------------------------------- //
//! @brief   KIM1 library example
//! @note  		Board compatibility :
//!					- Arduino UNO
//!					- Arduino UNO Wifi Rev2
//!					- Wemos D1
// -------------------------------------------------------------------------- //

#include "KIM.h"

/*-- CONFIGURATION --*/
#define INTERVAL_MS     60000

const char BAND[] = "B1";
const char FRQ[] = "300";
const char PWR[] = "500";
const char TCXOWU[] = "5000";

const char data[] = "BB7572A3C6D5D417D61E148D29C3110860B2C050E672A3C6D5D417D61E148D";
/*-- CONFIGURATION --*/

#if defined(__AVR_ATmega4809__)	// Arduino UNO Wifi Rev2

HardwareSerial &kserial = Serial1;

#else	// Arduino UNO and Wemos D1

SoftwareSerial kserial(RX_KIM, TX_KIM);

#endif

KIM kim(&kserial);

void setup() {
        Serial.begin(9600);
        Serial.println();

        Serial.println("Example KIM1 Arduino shield");

        if(kim.check()) {
                Serial.println("KIM -- Check success");
        } else {
                Serial.println("KIM -- Check fail. Please check wiring and jumpers. Freezing.");
                while(1);
        }
        Serial.println();

        kim.set_BAND(BAND, sizeof(BAND)-1);
        kim.set_FRQ(FRQ, sizeof(FRQ)-1);
        kim.set_PWR(PWR, sizeof(PWR)-1);
        kim.set_TCXOWU(TCXOWU, sizeof(TCXOWU)-1);

        Serial.print("KIM -- Get ID : ");
        Serial.println(kim.get_ID());
		
        Serial.print("KIM -- Get SN : ");
        Serial.println(kim.get_SN());
		
        Serial.print("KIM -- Get FW : ");
        Serial.println(kim.get_FW());
		
        Serial.print("KIM -- Get BAND : ");
        Serial.println(kim.get_BAND());
		
        Serial.print("KIM -- Get PWR : ");
        Serial.println(kim.get_PWR());
		
        Serial.print("KIM -- Get FRQ : ");
        Serial.println(kim.get_FRQ());
		
        Serial.print("KIM -- Get TCXOWU : ");
        Serial.println(kim.get_TCXOWU());
}

void loop() {
        Serial.print("KIM -- Send data ... ");
        if(kim.send_data(data, sizeof(data)-1) == OK_KIM){
                Serial.println("Message sent");
        }else{
                Serial.println("Error");    
        }
        Serial.println("KIM -- Turn OFF");

        kim.set_sleepMode(true);
        delay(INTERVAL_MS);
        kim.set_sleepMode(false);
}