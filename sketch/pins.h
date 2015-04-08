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



#ifndef __PINS_H
#define __PINS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "defs.h"


#define Rx_AVR_Pin 	0
#define Tx_AVR_Pin 	1
#define VccEnable_Wifi_Pin 	2 // output, acitve high
#define Button_Pin	3 // input, no pullup
#define IO0_Wifi_Pin	4 // input, pullup (no idea what it is good for)
#define IO2_Wifi_Pin	5 // input, pullup (no idea what it is good for)
#define PDn_Wifi_Pin	6 // output, must be high, for running Wifi
#define RSTn_Wifi_Pin	7 // output, reset pin for Wifi, active Low. 
#define Sensor_Pin	8 // in-/output, OneWire communication pin for temperature sensor
#define VccEnableN_Analog_Pin	9 // output, LOW = power on.
     // activates the temperature sensor, the LED and the analog reference Voltage. 
#define Discharge_Pin	10 // output, active High, laods the battery with about 120mA (38 Ohm)
#define nc11_Pin	11 // output, not connected

#ifdef SS_DEBUG
#define ssRx_Pin	12 // Input, software serial pin (MISO)
#define ssTx_Pin	13 // output, software serial pin (SCK)
#else
#define nc12_Pin	12 // output, not connected
#define nc13_Pin	13 // output, not connected
#endif

#define VoltageInput_Pin 6 // has a 1k:1k voltage devider and an 
//	external voltage reference with 2.5V

// TODO: there are more n.c. pins (Analog Inputs)

extern bool wifiIsOnFlag;


void Pins_init();

void Pins_enableLed(bool on);
void Pins_enableWifi(bool on);
void Pins_enableLoadResistor(bool on);


#ifdef __cplusplus
}
#endif

#endif

