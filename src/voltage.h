// voltage.h


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



#ifndef __VOLTAGE_H
#define __VOLTAGE_H

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t Volt_raw_times10; // 0...10230 = 0...5V
extern char Volt_str[10]; // e.g. 97.0 
// for simplicity, the number is rounded to integers, 
// but the string is a floating-point, since the 
// server-protocol is specified that way. 

extern int16_t Volt_permill;


void Volt_read();
void Volt_toStr();

#ifdef __cplusplus
}
#endif

#endif