// -------------------------------------------------------------------------- //
//! @file   	KIM.cpp
//! @brief  	Handles the interface between Kim1 interface shield v2 and Arduino
//! @note  		Board compatibility :
//!					- Arduino UNO
//!					- Arduino UNO Wifi Rev2
//!					- Wemos D1
//! @author 	Kineis
//! @date   	2020-03-15
//! @version 	3.0.2
// -------------------------------------------------------------------------- //


// -------------------------------------------------------------------------- //
// Includes
// -------------------------------------------------------------------------- //
#include "KIM.h"


// -------------------------------------------------------------------------- //
//! Constructors
// -------------------------------------------------------------------------- //
KIM::KIM(SerialUART* device, bool pinON_OFF_used)
{
	kimSerial = device;
	this->pinON_OFF_used = pinON_OFF_used;

	if(pinON_OFF_used == true){
		pinMode(ON_OFF_KIM_PIN, OUTPUT);
		digitalWrite(ON_OFF_KIM_PIN, LOW);
	}
}

KIM::KIM(SerialUART* device)
{
	kimSerial = device;
	this->pinON_OFF_used = true;

	pinMode(ON_OFF_KIM_PIN, OUTPUT);
	digitalWrite(ON_OFF_KIM_PIN, LOW);
}


// -------------------------------------------------------------------------- //
//! Initialize the KIM1 module : Softserial and ON/OFF pin
// -------------------------------------------------------------------------- //

bool KIM::check()
{
	set_sleepMode(true);
	set_sleepMode(false);

	char* tmp = get_ID();	//if success, the expected response is : +ID:xx

    if (*(tmp) == '+')
		return true;
	else
		return false;
}


// -------------------------------------------------------------------------- //
//! Set the status of the KIM1 module : ON or OFF
// -------------------------------------------------------------------------- //

bool KIM::set_sleepMode(bool mode)
{
	uint8_t bufferdata = 0;
	if(pinON_OFF_used == true){
		if(get_sleepMode() == mode){
				return true;
		}
		else{
			if(mode == false){


#if  softSerial
				new (kimSerial) SerialUART(RX_KIM, TX_KIM);	//Initialize UART
#endif
				kimSerial->begin(BAUDRATE);

				kimSerial->flush();					//Clean TX buffer
				while (kimSerial->available() > 0)	//Clean RX buffer
					kimSerial->read();

				digitalWrite(ON_OFF_KIM_PIN, true);	//When State_PIN is put HIGH,
													//KIM sends SN packet
				bufferdata = 0;
				for(int i=0; i<2; i++){
					bufferdata = kimSerial->readBytesUntil('\n', response, 20);

					if(bufferdata != 0)
						return true;
				}
				return false;
			}
			else{
				kimSerial->flush();					//Clean TX buffer
				while (kimSerial->available() > 0)	//Clean RX buffer
					kimSerial->read();

				digitalWrite(ON_OFF_KIM_PIN, false);
				
#if  softSerial
				kimSerial->~SerialUART();	//De-initialize UART
#else
				//< Disables serial communication, allowing the RX and TX pins to
				//< be used for general input and output. To re-enable serial
				//< communication, call Serial.begin().
				kimSerial->end();
#endif
				pinMode(TX_KIM, INPUT);
				pinMode(RX_KIM, INPUT);
				return true;
			}
		}
	}else{
		if(mode == true)
			return false;
		else
			return true;
	}
}


// -------------------------------------------------------------------------- //
//! Get the status of the KIM1 module : ON or OFF
// -------------------------------------------------------------------------- //

bool KIM::get_sleepMode()
{
	if(pinON_OFF_used == true){
		if(digitalRead(ON_OFF_KIM_PIN) == 1)
			return false;
		else
			return true;
	}else{
		return false;
	}
}


// -------------------------------------------------------------------------- //
//! This function sends AT command to the KIM module.
// -------------------------------------------------------------------------- //

RetStatusKIMTypeDef KIM::send_ATCommand()
{
	kimSerial->flush();					//Clean TX buffer
	while (kimSerial->available() > 0)	//Clean RX buffer
		kimSerial->read();

    response[0] = '\0';

    kimSerial->print(command);

	for(int i=0; i<10; i++){
		uint8_t k = kimSerial->readBytesUntil('\n', response, 20);
		response[k] = '\0';
        if((response[0] != '\0') && ( \
                    response[1] == 'K'	/*OK*/	|| \
                    response[1] == 'I' /*+ID*/	|| \
                    response[1] == 'S' /*+SN*/	|| \
                    response[1] == 'F' /*+FW*/	|| \
                    response[1] == 'B' /*+BAND*/|| \
                    response[1] == 'F' /*+FRQ*/	|| \
                    response[1] == 'P' /*+PWR*/)
          ){
            return OK_KIM;
        }
        else if (response[0] != '\0' && response[1] == 'R'){   /*ERROR*/
			return ERROR_KIM;
        }
		else if (response[0] != '\0'){
			return UNKNOWN_ERROR_KIM;
        }
    }
    return TIMEOUT_KIM;
}


// -------------------------------------------------------------------------- //
//! Get the ID of the KIM1 module.
// -------------------------------------------------------------------------- //

char* KIM::get_ID()
{
	for(uint8_t i=0; i<6; i++)
		command[i] = AT_ID[i];

	for(uint8_t i=0; i<3; i++)
		command[6+i] = AT_REQUEST[i];

    send_ATCommand();
	return &response[0];
}


// -------------------------------------------------------------------------- //
//! Get the Serial Number of the KIM1 module.
// -------------------------------------------------------------------------- //

char* KIM::get_SN()
{
	for(uint8_t i=0; i<6; i++)
		command[i] = AT_SN[i];

	for(uint8_t i=0; i<3; i++)
		command[6+i] = AT_REQUEST[i];

    send_ATCommand();
	return &response[0];
}


// -------------------------------------------------------------------------- //
//! Get the Firmware Version of the KIM1 module.
// -------------------------------------------------------------------------- //

char* KIM::get_FW()
{
	for(uint8_t i=0; i<6; i++)
		command[i] = AT_FW[i];

	for(uint8_t i=0; i<3; i++)
		command[6+i] = AT_REQUEST[i];

    send_ATCommand();
	return &response[0];
}


// -------------------------------------------------------------------------- //
//! Get the transmit power of the KIM1 module.
// -------------------------------------------------------------------------- //

char* KIM::get_PWR()
{
	for(uint8_t i=0; i<7; i++)
		command[i] = AT_PWR[i];

	for(uint8_t i=0; i<3; i++)
		command[7+i] = AT_REQUEST[i];

    send_ATCommand();
	return &response[0];
}


// -------------------------------------------------------------------------- //
//! Get the transmit band frequency of the KIM1 module.
// -------------------------------------------------------------------------- //

char* KIM::get_BAND()
{
	for(uint8_t i=0; i<8; i++)
		command[i] = AT_BAND[i];

	for(uint8_t i=0; i<3; i++)
		command[8+i] = AT_REQUEST[i];

    send_ATCommand();
	return &response[0];
}


// -------------------------------------------------------------------------- //
//! Get the frequency offset of the KIM1 module.
// -------------------------------------------------------------------------- //

char* KIM::get_FRQ()
{
	for(uint8_t i=0; i<7; i++)
		command[i] = AT_FRQ[i];

	for(uint8_t i=0; i<3; i++)
		command[7+i] = AT_REQUEST[i];

    send_ATCommand();
	return &response[0];
}


// -------------------------------------------------------------------------- //
//! Get the TCXO Warm Up time of the KIM1 module (seconds).
// -------------------------------------------------------------------------- //

char* KIM::get_TCXOWU()
{
	for(uint8_t i=0; i<10; i++)
		command[i] = AT_TCXOWU[i];

	for(uint8_t i=0; i<3; i++)
		command[10+i] = AT_REQUEST[i];

    send_ATCommand();
	return &response[0];
}


// -------------------------------------------------------------------------- //
//! Set the ID of the KIM1 module, if it is possible (check returned value).
// -------------------------------------------------------------------------- //

RetStatusKIMTypeDef KIM::set_ID(char ID[], uint8_t len)
{
	for(uint8_t i=0; i<6; i++)
		command[i] = AT_ID[i];

	for(uint8_t i=0; i<len; i++)
		command[6+i] = ID[i];

	command[6+len] = '\r';
	command[7+len] = '\0';

	return send_ATCommand();
}


// -------------------------------------------------------------------------- //
//!	Set the Serial Number of the KIM1 module, if it is possible.
// -------------------------------------------------------------------------- //

RetStatusKIMTypeDef KIM::set_SN(char SN[], uint8_t len)
{
	for(uint8_t i=0; i<6; i++)
		command[i] = AT_SN[i];

	for(uint8_t i=0; i<len; i++)
		command[6+i] = SN[i];

	command[6+len] = '\r';
	command[7+len] = '\0';

	return send_ATCommand();
}


// -------------------------------------------------------------------------- //
//! Set the transmit power of the KIM1 module.
// -------------------------------------------------------------------------- //

RetStatusKIMTypeDef KIM::set_PWR(char PWR[], uint8_t len)
{
	for(uint8_t i=0; i<7; i++)
		command[i] = AT_PWR[i];

	for(uint8_t i=0; i<len; i++)
		command[7+i] = PWR[i];

	command[7+len] = '\r';
	command[8+len] = '\0';

	return send_ATCommand();
}


// -------------------------------------------------------------------------- //
//! Set the transmit band frequency of the KIM1 module.
// -------------------------------------------------------------------------- //

RetStatusKIMTypeDef KIM::set_BAND(char BAND[], uint8_t len)
{
	for(uint8_t i=0; i<8; i++)
		command[i] = AT_BAND[i];

	for(uint8_t i=0; i<len; i++)
		command[8+i] = BAND[i];

	command[8+len] = '\r';
	command[9+len] = '\0';

	return send_ATCommand();
}


// -------------------------------------------------------------------------- //
//! Set the frequency offset of the KIM1 module.
// -------------------------------------------------------------------------- //

RetStatusKIMTypeDef KIM::set_FRQ(char FRQ[], uint8_t len)
{
	for(uint8_t i=0; i<7; i++)
		command[i] = AT_FRQ[i];

	for(uint8_t i=0; i<len; i++)
		command[7+i] = FRQ[i];

	command[7+len] = '\r';
	command[8+len] = '\0';

	return send_ATCommand();
}


// -------------------------------------------------------------------------- //
//! Set the TCXO Warm Up time of the KIM1 module.
// -------------------------------------------------------------------------- //

RetStatusKIMTypeDef KIM::set_TCXOWU(char TCXOWU[], uint8_t len)
{
	for(uint8_t i=0; i<10; i++)
		command[i] = AT_TCXOWU[i];

	for(uint8_t i=0; i<len; i++)
		command[10+i] = TCXOWU[i];

	command[10+len] = '\r';
	command[11+len] = '\0';

	return send_ATCommand();
}


// -------------------------------------------------------------------------- //
//! Send data to satellites
// -------------------------------------------------------------------------- //

RetStatusKIMTypeDef KIM::send_data(char data[], uint8_t len)
{
	for(uint8_t i=0; i<6; i++)
		command[i] = AT_TX[i];

	for(uint8_t i=0; i<len; i++)
		command[6+i] = data[i];

	command[6+len] = '\r';
	command[7+len] = '\0';

	return send_ATCommand();
}


// -------------------------------------------------------------------------- //
//! Binary uint8_t[] to hex-string (char[]) convert function.
// -------------------------------------------------------------------------- //

void KIM::uint2hexString(uint8_t *input, uint8_t len, char *output)
{
        uint8_t idx;
        idx = 0;

        for (idx = 0; idx < len; idx++)
                sprintf((char *)(output + idx * 2), "%02X", input[idx]);
}
