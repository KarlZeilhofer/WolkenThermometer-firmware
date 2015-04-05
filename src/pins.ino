// pins.h


// Copyright 2015 Karl Zeilhofer
// www.zeilhofer.co.at

// This file is part of WolkenThermometer-firmware.

//     WolkenThermometer-firmware is free software: you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.

//     WolkenThermometer-firmware is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.

//     You should have received a copy of the GNU General Public License
//     along with WolkenThermometer-firmware.  If not, see <http://www.gnu.org/licenses/>.


#include "pins.h"
#include "helper.h"


bool wifiIsOnFlag=false;

// init all pins to input/output
// and set outputs to consume the lowest power. 
void Pins_init()
{
    // define output pins:
    pinMode(VccEnable_Wifi_Pin, OUTPUT);  	digitalWrite(VccEnable_Wifi_Pin, LOW);   
//    pinMode(IO0_Wifi_Pin, OUTPUT); 		digitalWrite(IO0_Wifi_Pin, LOW);  // set to high on start (low = flash-mode = firmware update)
//    pinMode(IO2_Wifi_Pin, OUTPUT); 		digitalWrite(IO2_Wifi_Pin, LOW);  // should be high on start
    pinMode(PDn_Wifi_Pin, OUTPUT);		digitalWrite(PDn_Wifi_Pin, LOW);   
    pinMode(RSTn_Wifi_Pin, OUTPUT);		digitalWrite(RSTn_Wifi_Pin, LOW);   
    pinMode(VccEnableN_Analog_Pin, OUTPUT);	digitalWrite(VccEnableN_Analog_Pin, HIGH);   
    pinMode(Discharge_Pin, OUTPUT);		digitalWrite(Discharge_Pin, LOW);   
    pinMode(nc11_Pin, OUTPUT);			digitalWrite(nc11_Pin, LOW);   
#ifndef SS_DEBUG
    pinMode(nc12_Pin, OUTPUT);          digitalWrite(nc12_Pin, LOW);   
    pinMode(nc13_Pin, OUTPUT);			digitalWrite(nc13_Pin, LOW);   
#endif

    // define input pins:
    pinMode(IO0_Wifi_Pin, INPUT); 
    pinMode(IO2_Wifi_Pin, INPUT); 
    pinMode(Rx_AVR_Pin, INPUT); 
    pinMode(Tx_AVR_Pin, INPUT); 
    pinMode(Button_Pin, INPUT); // no pullup here!
}


void Pins_enableLed(bool on)
{
	if(on){
		digitalWrite(VccEnableN_Analog_Pin, LOW); // activate PMOS-Gate
	}else{
		digitalWrite(VccEnableN_Analog_Pin, HIGH);
	}
}

void Pins_enableWifi(bool on)
{
    if(on){
        myDebugP(PSTR("Turn WiFi on! - "));
        myDebugP(PSTR("No debug over Serial any more!\n"));

        wifiIsOnFlag=true;
        // set inputs to inputs, and enable then the power
        pinMode(IO0_Wifi_Pin, INPUT); 
        pinMode(IO2_Wifi_Pin, INPUT); 
        pinMode(Rx_AVR_Pin, INPUT); 
        digitalWrite(VccEnable_Wifi_Pin, HIGH); 
        digitalWrite(PDn_Wifi_Pin, HIGH);   
        digitalWrite(RSTn_Wifi_Pin, HIGH);   

    }else{
        wifiIsOnFlag=false;
        // disable power and set inputs to output-low
        digitalWrite(PDn_Wifi_Pin, LOW);   
        digitalWrite(RSTn_Wifi_Pin, LOW);  
        digitalWrite(VccEnable_Wifi_Pin, LOW); 
        pinMode(IO0_Wifi_Pin, OUTPUT); 
        pinMode(IO2_Wifi_Pin, OUTPUT); 
        pinMode(Rx_AVR_Pin, OUTPUT); 
        digitalWrite(IO0_Wifi_Pin, LOW); 
        digitalWrite(IO2_Wifi_Pin, LOW); 
        digitalWrite(Rx_AVR_Pin, LOW);

        myDebugP(PSTR("Turned WiFi off! - "));
        myDebugP(PSTR("Debug over Serial works again!\n"));
    }
}

void Pins_enableLoadResistor(bool on)
{
    digitalWrite(Discharge_Pin, on);
}