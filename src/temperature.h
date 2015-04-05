// temperature.h


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



#ifndef __TEMPERATURE_H
#define __TEMPERATURE_H

#ifdef __cplusplus
extern "C" {
#endif


#include "pins.h"
#include <OneWire.h>

// variables and objects:
extern int16_t Temp_raw; // most recent raw value. 
extern char Temp_str[10]; // e.g. +24.56 or -17.38 or +0.00


// functions:

void Temp_init();

// updates the Temp_raw value
void Temp_read();
char* Temp_toStr(); // returns a pointer to Temp_str



#ifdef __cplusplus
}
#endif

#endif