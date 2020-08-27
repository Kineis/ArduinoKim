// -------------------------------------------------------------------------- //
//! @brief   KIM1 library wifi example
//! @note  		Board compatibility :
//!					- Arduino UNO Wifi Rev2
// -------------------------------------------------------------------------- //
#include <SPI.h>
#include <WiFiNINA.h>
#include <String.h>
#include "index.h"

#include "KIM.h"

HardwareSerial &kserial = Serial1;
RetStatusKIMTypeDef kim_response = 0x08;
KIM kim(&kserial);

char ssid[] = "Arduino_Wifi";
char pass[] = "123456";

int status = WL_IDLE_STATUS;
WiFiServer server(80);

bool refreshPB = false;
bool sendPB = false;
bool setPB = false;

String ID;
String SN;
String FW;
String PWR;
String BAND;
String TCXOWU;
String FRQ;

String queryString;
String set_value;
String payload_value;

/*Periodic transmission*/
uint32_t actTime_ms = 0;
uint32_t lastTime_ms = 0;
uint32_t transmitPeriod_s = 60;
bool periodicTransmission = false;

void setup() {
        //Initialize serial and wait for port to open:
        Serial.begin(9600);
        while (!Serial) {
                ; // wait for serial port to connect. Needed for native USB port only
        }

        if( kim.check()) {
                Serial.println("KIM -- Check success");
        } else {
                Serial.println("KIM -- Check fail. Please check wiring and jumpers.");
                Serial.println("KIM -- Warning: with UNO Wifi Rev2, RX/TX jumpers should be placed on STm32 position");
                Serial.println("KIM -- Freezing.");
                while(1);
        }
        refreshKIMparameters();
        kim.set_sleepMode(true);

        lastTime_ms = millis();
        actTime_ms = millis();

        Serial.println("Access Point Web Server");

        // check for the WiFi module:
        if (WiFi.status() == WL_NO_MODULE) {
                Serial.println("Communication with WiFi module failed!");
                // don't continue
                while (true);
        }

        // print the network name (SSID);
        Serial.print("Creating access point named: ");
        Serial.println(ssid);

        // Create open network. Change this line if you want to create an WEP network:
        status = WiFi.beginAP(ssid, pass);
        if (status != WL_AP_LISTENING) {
                Serial.println("Creating access point failed, freezing.");
                while (true);
        }

        // wait 10 seconds for connection:
        delay(5000);

        // start the web server on port 80
        server.begin();

        // you're connected now, so print out the status
        printWiFiStatus();
}


void loop() {
        // compare the previous status to the current status
        if (status != WiFi.status()) {
                // it has changed update the variable
                status = WiFi.status();

                if (status == WL_AP_CONNECTED) {
                        Serial.println("Device connected to AP");
                } else {
                        Serial.println("Device disconnected from AP");
                }
        }

        WiFiClient client = server.available();

        if (client) {
                Serial.println("new client");
                String currentLine = "";          // Incoming data from the client
                while (client.connected()) {
                        if (client.available()) {
                                char c = client.read();
                                // Serial.write(c);
                                if (c == '\n') {
                                        // if the current line is blank, you got two newline characters in a row.
                                        // that's the end of the client HTTP request, so send a response:
                                        if (currentLine.length() == 0) {
                                                // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                                                // and a content-type so the client knows what's coming, then a blank line:
                                                sendResponse(client);
                                                break;
                                        }
                                        else {      // if you got a newline, then clear currentLine:
                                                if(refreshPB || sendPB || setPB) {
                                                        uint8_t i, countSep;
                                                        queryString = currentLine.substring(6, currentLine.indexOf(' ', 6));

                                                        for (i=0, countSep=0; queryString[i]; i++)
                                                                countSep += (queryString[i] == '&');

                                                        if(setPB && countSep == 1) {
                                                                set_value = queryString.substring(
                                                                                    queryString.indexOf('=', queryString.indexOf('&')) + 1,
                                                                                    queryString.length());

                                                                kim.set_sleepMode(false);
                                                                if(queryString.startsWith("ID")) {
                                                                        kim_response = kim.set_ID(&set_value[0], set_value.length());
                                                                } else if (queryString.startsWith("SN")) {
                                                                        kim_response = kim.set_SN(&set_value[0], set_value.length());
                                                                } else if (queryString.startsWith("FW")) {
                                                                        // Can not set FW
                                                                } else if (queryString.startsWith("PWR")) {
                                                                        kim_response = kim.set_PWR(&set_value[0], set_value.length());
                                                                } else if (queryString.startsWith("BAND")) {
                                                                        kim_response = kim.set_BAND(&set_value[0], set_value.length());
                                                                } else if (queryString.startsWith("TCXOWU")) {
                                                                        kim_response = kim.set_TCXOWU(&set_value[0], set_value.length());
                                                                } else if (queryString.startsWith("FRQ")) {
                                                                        kim_response = kim.set_FRQ(&set_value[0], set_value.length());
                                                                } else if (queryString.startsWith("T_TR")) {
                                                                        transmitPeriod_s = atoi(&set_value[0]);
                                                                        kim_response = 0x08;
                                                                }
                                                                refreshKIMparameters();
                                                                kim.set_sleepMode(true);
                                                        } else if(sendPB && (countSep == 0 || countSep == 1)) {
                                                                if(countSep == 0) {
                                                                        payload_value = queryString.substring(5, queryString.length());
                                                                        periodicTransmission = false;
                                                                } else {
                                                                        payload_value = queryString.substring(5, queryString.indexOf('&'));
                                                                        periodicTransmission = true;
                                                                }
                                                                payload_value.toUpperCase();
                                                                if(checkPayloadContent(payload_value, payload_value.length())) {
                                                                        lastTime_ms = millis();

                                                                        kim.set_sleepMode(false);
                                                                        kim_response = kim.send_data(&payload_value[0], payload_value.length());
                                                                        kim.set_sleepMode(true);
                                                                } else {
                                                                        payload_value = "";
                                                                        periodicTransmission = false;
                                                                }
                                                        } else {
                                                                refreshKIMparameters();
                                                                kim_response = 0x08;
                                                        }
                                                }
                                                currentLine = "";
                                                refreshPB = false;
                                                sendPB = false;
                                                setPB = false;
                                        }
                                }
                                else if (c != '\r') {    // if you got anything else but a carriage return character,
                                        currentLine += c;
                                }
                                if(currentLine.startsWith("GET /?")) {
                                        if (currentLine.endsWith("REFRESH=")) {
                                                refreshPB = true;
                                                sendPB = false;
                                                setPB = false;
                                        }
                                        if (currentLine.endsWith("SET=")) {
                                                refreshPB = false;
                                                sendPB = false;
                                                setPB = true;
                                        }
                                        if (currentLine.endsWith("SEND=")) {
                                                refreshPB = false;
                                                sendPB = true;
                                                setPB = false;
                                        }
                                }
                        }
                }
                client.stop();
                Serial.println("client disconnected");
        }
        /*Kim periodic*/
        actTime_ms = millis();
        if(periodicTransmission && (actTime_ms - lastTime_ms) > (transmitPeriod_s * 1000)) {
                lastTime_ms = millis();
                actTime_ms = millis();

                kim.set_sleepMode(false);

                if(kim.send_data(&payload_value[0], payload_value.length()) == OK_KIM)
                        Serial.println("KIM -- periodic : Sent");
                else
                        Serial.println("KIM -- periodic : Error");

                kim.set_sleepMode(true);
        }
}

void printWiFiStatus() {
        Serial.print("SSID: ");
        Serial.println(WiFi.SSID());

        IPAddress ip = WiFi.localIP();
        Serial.print("IP Address: ");
        Serial.println(ip);

        Serial.print("To see this page in action, open a browser to http://");
        Serial.println(ip);

}

void sendResponse(WiFiClient client) {
        client.print(response1);
        client.print(ID);
        client.print(response2);
        client.print(SN);
        client.print(response3);
        client.print(FW);
        client.print(response4);
        client.print(PWR);
        client.print(response5);
        client.print(BAND);
        client.print(response6);
        client.print(TCXOWU);
        client.print(response7);
        client.print(FRQ);
        client.print(response8);
        client.print(transmitPeriod_s);
        if(periodicTransmission) {
                client.print(" (on : ");
                client.print(payload_value);
                client.print(")");
        } else {
                client.print(" (off)");
        }
        client.print(response9);

        switch(kim_response) {
        case ERROR_KIM :
                client.print("Error (check input values)");
                break;
        case OK_KIM :
                client.print("Ok");
                break;
        case UNKNOWN_ERROR_KIM :
                client.print("Unknown error");
                break;
        case TIMEOUT_KIM :
                client.print("Timeout");
        default:
                ;
        }
        client.print(response10);
}

void refreshKIMparameters() {
        kim.set_sleepMode(false);

        FW      = kim.get_FW();
        BAND    = kim.get_BAND();
        ID      = kim.get_ID();
        SN      = kim.get_SN();
        PWR     = kim.get_PWR();
        FRQ     = kim.get_FRQ();
        TCXOWU  = kim.get_TCXOWU();

        kim.set_sleepMode(true);
}
/*
*!      @brief  Check if the payload is not empty and if characters are only hex.
*/
bool checkPayloadContent(String str, uint8_t len) {
        bool ret = true;
        if (len == 0)
                return false;

        for(uint8_t i = 0; i < len; i++) {
                if(str[i] < '0' || str[i] > 'F' || (str[i] > '9' && str[i] < 'A')) {
                        ret = false;
                        break;
                }
        }
        return ret;
}
