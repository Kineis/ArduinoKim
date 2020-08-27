Kim1 Library :
# HW setup

* Arduino Uno : ATmega328 MCU
* KIM1 shield v2
* 401MHz antenna plugged to the KIM1 shield for Kinéis transmission


\note 
This library is compatible with **Arduino Uno**, **Arduino Uno WiFi Rev2**, **Wemos D1 Board**.
Check compatibilty if you want to use this library with other boards.

# SW

## Getting started

You can create the documentation for this software package, generated with Doxygen : 

* install Doxygen:
  ````
    sudo apt-get install doxygen
  ````
* run the Makefile :
  ````
    make doc
  ````
  
A good method is to start by compiling the example : 

* Open "Example_KIM" with the Arduino IDE :
* Select the target board : Tools -> Boards -> Your Board
* Select the right port : Tools -> Port -> Your Board Port
* To upload it to your board, press the "Upload"
* Open the serial monitor (9600 baud)

If the HW configuration is right you should see the configuration of the Kim1 module.

## SW package

This software package contains:
* Arduino SW Library to control KIM1 Module
* Examples

The examples show how to use KIM.h library.
Below is the description of the core of "Example_KIM" application : 
* Initialization : library, UART & GPIOs;
* Power On the KIM1 module;
* Get factory data and transmission parameters(**KIM ID**, **KIM SN**, **KIM FW version**, ...) 
and print data on UART configured to show logs;
* Set transmission parameters and show logs;
* Start periodic transmission :
  * Send Payload (default value, **BB7572A3C6D5D417D61E148D29C3110860B2C050E672A3C6D5D417D61E148D**)
  * Power Off the KIM1 module;
  * Wait **60 seconds**;
  * Power On the KIM1 module;
* Repeat transmission;

\note 
To Communicate with KIM1 module, UART communication is configured as below 
* *BaudRate* : **4800**
* *Word length* : **8 bits**
* *Parity* : **None**
* *Hardware flow control* : **None**
* *Stop bits* : **1 bit**


## Source tree

The code is detailed as below:

````
.
├── doxyfile
├── README.md
├── Makefile
├── keywords.txt
├── library.properties
├── logo_kineis.png
````
 Folder containing this documentation once generated with Doxygen
````
├── doc
│   └── html
│       ├── index.html
│       └── ...
````
 Source code of Kim library
````
├── src
│   ├── KIM.h
│   └── KIM.cpp
````
 Source code of example application
````
└── examples
    ├── Example_KIM
    └── Example_KIM_Wifi
````