// helper.h


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



#ifndef __HELPER_H
#define __HELPER_H

#include <EEPROM.h>
#include <SoftwareSerial.h>
#include "pins.h"
#include "defs.h"

#ifdef SS_DEBUG
extern SoftwareSerial mySerial;
#endif


void Helper_serialReadLine(char* buffer, uint16_t maxLen);
uint32_t Helper_str2uint32(char* str);

void Helper_eepromWriteData(char* buffer, uint16_t len, int addr);
void Helper_eepromReadData(char* buffer, uint16_t len, int addr);

void Helper_expandFromBinToHex(uint8_t* bin, int binLen, char* hex);
void Helper_contractFromHexToBin(char* hex, int hexLen, uint8_t* bin); 
	// only for contraction the same buffer-pointer can be used for hex* and bin*

void myDebug(const char* str); // write RAM string
void myDebugP(const char* str); // write PROGMEM strings

void flushSerialRx();

#endif