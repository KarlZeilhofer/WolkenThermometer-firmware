// settings.h



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


#ifndef __SETTINGS_H
#define __SETTINGS_H


#include <EEPROM.h>

// on the WolkenThermometer we use the ATmega328p with 1kByte EEPROM


enum Addresses_e
{
	AddrId=0, // WolkenThermometer ID, set by serialmenu
	AddrPubKey=4, // public key, set by serialmenu
	AddrSecKey=14, // secret key, set by serialmenu
	AddrSsid=24, // host wifi ssid, set by wificonfig
	AddrPwd=55, // wifi password, set by wificonfig
	AddrKeysConfigured=86,
	AddrWifiConfigured=87
};


extern enum Addresses_e Addresses;


boolean Settings_areKeysConfigured(); 
boolean Settings_isWifiConfigured();
uint32_t Settings_getId();
void Settings_getSecKey(char* str);
void Settings_getPubKey(char* str);
void Settings_getSsid(char* str);
void Settings_getPwd(char* str);

boolean Settings_setKeysConfigured(boolean flag); 
boolean Settings_setWifiConfigured(boolean flag);
void Settings_setId(uint32_t id);
void Settings_setSecKey(char* str);
void Settings_setPubKey(char* str);
void Settings_setSsid(char* str);
void Settings_setPwd(char* str);


#endif
