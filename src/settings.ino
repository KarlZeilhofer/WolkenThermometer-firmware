// settings.ino



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


#include <EEPROM.h>
#include "helper.h"
#include "settings.h"

enum Addresses_e Addresses;

// Getters:
boolean Settings_areKeysConfigured()
{
	if(EEPROM.read(AddrKeysConfigured) == 'K'){
		return true;
	}else{
		return false;
	}
}

boolean Settings_isWifiConfigured()
{
	if(EEPROM.read(AddrWifiConfigured) == 'W'){
		return true;
	}else{
		return false;
	}
}

uint32_t Settings_getId()
{
	uint32_t n=0;
	
	n += EEPROM.read(AddrId);
	n = n<<8;
	n += EEPROM.read(AddrId+1);
	n = n<<8;
	n += EEPROM.read(AddrId+2);
	n = n<<8;
	n += EEPROM.read(AddrId+3);
	
	return n;
}

void Settings_getSecKey(char* str)
{
	Helper_eepromReadData(str, 10, AddrSecKey);	
	str[10]=0; // terminate string
}

void Settings_getPubKey(char* str)
{
	Helper_eepromReadData(str, 10, AddrPubKey);
	str[10]=0; // terminate string
}

void Settings_getSsid(char* str)
{
	Helper_eepromReadData(str, 30, AddrSsid);
	str[30]=0; // terminate string (could be already terminated)
}

void Settings_getPwd(char* str)
{
	Helper_eepromReadData(str, 30, AddrPwd);
	str[30]=0; // terminate string (could be already terminated)
}


// Setters:
boolean Settings_setKeysConfigured(boolean flag)
{
	if(flag){
		EEPROM.write(AddrKeysConfigured, 'K');
	}else{
		EEPROM.write(AddrKeysConfigured, '#');
	}
}

boolean Settings_setWifiConfigured(boolean flag)
{
	if(flag){
		EEPROM.write(AddrWifiConfigured, 'W');
	}else{
		EEPROM.write(AddrWifiConfigured, '#');
	}
}

// big endian format
void Settings_setId(uint32_t id)
{
	EEPROM.write(AddrId, (id>>24)&0xff);
	EEPROM.write(AddrId+1, (id>>16)&0xff);
	EEPROM.write(AddrId+2, (id>>8)&0xff);
	EEPROM.write(AddrId+3, (id>>0)&0xff);
}

void Settings_setSecKey(char* str)
{
	Helper_eepromWriteData(str, 10, AddrSecKey);
}

void Settings_setPubKey(char* str)
{
	Helper_eepromWriteData(str, 10, AddrPubKey);
}

void Settings_setSsid(char* str)
{
	Helper_eepromWriteData(str, 31, AddrSsid); 
}

void Settings_setPwd(char* str)
{
	Helper_eepromWriteData(str, 31, AddrPwd);
}
