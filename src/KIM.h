// -------------------------------------------------------------------------- //
//! @file   	KIM.h
//! @brief  	Handles the interface between Kim1 interface shield v2 and Arduino
//! @note  		Board compatibility :
//!					- Arduino UNO
//!					- Arduino UNO Wifi Rev2
//!					- Wemos D1
//! @author 	Kineis
//! @date   	2020-03-15
//! @version 	3.0.2
// -------------------------------------------------------------------------- //

#ifndef KIM_H
#define KIM_H

// -------------------------------------------------------------------------- //
// Includes
// -------------------------------------------------------------------------- //
#include <stdbool.h>
#include "Arduino.h"


// -------------------------------------------------------------------------- //
// Defines values
// -------------------------------------------------------------------------- //
#define BAUDRATE 4800

#if defined(ESP8266)				//< WeMos D1

#define ON_OFF_KIM_PIN 	D4
#define RX_KIM 			D7
#define TX_KIM 			D8
#define softSerial 		true

#elif defined(__AVR_ATmega4809__)	//< Uno Wifi Rev2

#define ON_OFF_KIM_PIN 	4
#define RX_KIM 			0
#define TX_KIM 			1
#define softSerial 		false

// Edit the following lines to add other boards :
//#elif defined(xxx)

//#define ON_OFF_KIM_PIN 	x
//#define RX_KIM 			x
//#define TX_KIM 			x
//#define softSerial 		x

#else								//< Uno

#define ON_OFF_KIM_PIN 	4
#define RX_KIM 			7
#define TX_KIM 			8
#define softSerial 		true

#endif

#if  softSerial
	#include <SoftwareSerial.h>
	#include <new>
	#define SerialUART	SoftwareSerial
#else
	#define SerialUART	HardwareSerial
#endif

typedef enum {
	ERROR_KIM           	= 0x00,    	//!< Error returned by KIM
	OK_KIM              	= 0x01,   	//!< OK return by KIM
	UNKNOWN_ERROR_KIM   	= 0x02,   	//!< Something unknown returned by KIM
	TIMEOUT_KIM   			= 0x04   	//!< Timeout, nothing returned by KIM
} RetStatusKIMTypeDef;

class KIM{
	private:
	
	
// -------------------------------------------------------------------------- //
//! @brief 	This function sends AT command to the KIM module.
//!
//!		The AT command is stored in 'command'
// -------------------------------------------------------------------------- //

        RetStatusKIMTypeDef send_ATCommand();
		
		SerialUART* kimSerial;

		char response[20];
		char command[70];
		bool pinON_OFF_used;
		
		//This syntaxe is used to be compatible with old versions of gcc
		const char AT_ID[6] 		= {'A','T','+','I','D','='};					//!< "AT+ID="
		const char AT_SN[6] 		= {'A','T','+','S','N','='};					//!< "AT+SN="
		const char AT_SW[6] 		= {'A','T','+','S','W','='};					//!< "AT+SW="
		const char AT_PWR[7] 		= {'A','T','+','P','W','R','='};				//!< "AT+PWR="
		const char AT_BAND[8] 		= {'A','T','+','B','A','N','D','='};			//!< "AT+BAND="
		const char AT_FRQ[7] 		= {'A','T','+','F','R','Q','='};				//!< "AT+FRQ="
		const char AT_FW[6] 		= {'A','T','+','F','W','='};					//!< "AT+FW="
		const char AT_TCXOWU[10] 	= {'A','T','+','T','C','X','O','W','U','='};	//!< "AT+TCXOWU="		
		const char AT_TX[6] 		= {'A','T','+','T','X','='};					//!< "AT+TX="
		const char AT_REQUEST[3] 	= {'?','\r','\0'};								//!< "?\r\0"
		
    public:
	
	
// -------------------------------------------------------------------------- //
//! @brief 	Constructors 
//!
//! @param [in] SerialUART* device
//!    	
//! @param [in] pinON_OFF_used
//!    	If you want to use the ON/OFF pin of the KIM module put 'true' or 
//!		nothing. Else 'false'.
// -------------------------------------------------------------------------- //

        KIM(SerialUART* device, bool pinON_OFF_used);
        KIM(SerialUART* device);
		
		
// -------------------------------------------------------------------------- //
//! @brief 	Check the communication with the KIM1 module : serial and ON/OFF pin
//!
//!
//! @return bool
//!    	false 	: check fail
//!    	true 	: check success
// -------------------------------------------------------------------------- //

		bool check();
		
	
/** @defgroup powerGroup ON/OFF functions
 *	These functions are used to manage KIM1 module supply thanks to ON_OFF_KIM_PIN.
 *  @{
 */	
 
// -------------------------------------------------------------------------- //
//! @brief 	Set the status of the KIM1 module : ON or OFF  
//!
//! @param [in] bool 
//!    	false 	: turn ON the KIM module ; 
//!    	true 	: turn OFF the KIM module
//!
//! @return bool
//!    	false 	: error to set the wanted status ; 
//!    	true 	: success to set the wanted status
// -------------------------------------------------------------------------- //
		
		bool set_sleepMode(bool mode);	


// -------------------------------------------------------------------------- //
//! @brief 	Get the status of the KIM1 module : ON or OFF  
//!
//! @return bool
//!    	false 	: KIM is ON ; 
//!    	true 	: KIM is OFF
// -------------------------------------------------------------------------- //	
	
        bool get_sleepMode();  
 /** @} */		

/** @defgroup getGroup Get functions
 *	"Get functions" are used to read the KIM1 module configuration with AT 
 *	Commands.
 *	All these functions have the same construction.  
 *	An AT command is sent through the serial port to ask the chosen parameter.
 *	The answer is returned.
 *
 *
 *		get_ID();		To get the ID value
 *		return "+ID:28,7AC8998"
 *		get_PWR();		To get the transmition power value
 *		return "+PWR:750"
 *  @{
 */
		char* get_ID();
		char* get_SN();
		char* get_FW();
		char* get_PWR();
		char* get_BAND();
		char* get_FRQ();
		char* get_TCXOWU();
 /** @} */

/** @defgroup setGroup Set functions
 *	"Set functions" are used to configure the KIM1 module with AT Commands.
 *	All these functions have the same construction.  
 *	An AT command is sent through the serial port to set a value to the
 *	chosen parameter. RetStatusKIMTypeDef is returned, see RetStatusKIMTypeDef
 * 	details.
 *
 *
 *		set_BAND("B1", 2);		To set the frequency band on B1
 *		set_PWR("1000", 4);		To transmit at 1000 mW
 *		set_PWR("750", 3);		To transmit at 750 mW
 *  @{
 */
		RetStatusKIMTypeDef set_ID(char ID[], uint8_t len);
		RetStatusKIMTypeDef set_SN(char SN[], uint8_t len);
		RetStatusKIMTypeDef set_PWR(char PWR[], uint8_t len);
		RetStatusKIMTypeDef set_BAND(char BAND[], uint8_t len = 2);
		RetStatusKIMTypeDef set_FRQ(char FRQ[], uint8_t len);
		RetStatusKIMTypeDef set_TCXOWU(char TCXOWU[], uint8_t len);
 /** @} */
		
		RetStatusKIMTypeDef send_data(char data[], uint8_t len);
		

// -------------------------------------------------------------------------- //
//! @brief  Binary uint8_t[] to hex-string (char[]) convert function.

//! @param [in] *input 
//!    	pointer to the input uint8_t array
//! @param [in] len
//!    	length of the input array
//! @param [out] *output 
//!    	pointer to the output char array
// -------------------------------------------------------------------------- //

		static void uint2hexString(uint8_t *input, uint8_t len, char *output);
};

#endif
