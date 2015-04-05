// serialmenu.ino


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


#include "serialmenu.h"
#include "helper.h"
#include "settings.h"


char SerialMenu_str[SerialMenu_MAX_LEN+1];

// function which handles the configuration after burning the firmware
// ID, Public-Key and Secret-Key are needed.
// it also gives the possibility for setting the Wifi SSID and password. 
// when settings are already configured, we can skip input requests with enter-key. 
void SerialMenu_setup()
{
	myDebugP(PSTR("\nWelcome to the configuration of the WolkenThermometer\n"));
	myDebugP(PSTR("  For Help, please visit www.zeilhofer.co.at/wolke\n"));
	myDebugP(PSTR("  Info: Line terminations are with LF (\\n).\n\n"));
	
	// if keys are already configured, ask, for reconfiguration. 
	if(Settings_areKeysConfigured()){
		myDebugP(PSTR("This WolkenSensor has been configured already.\n"));
		myDebugP(PSTR("  ID = "));
		Serial.println(Settings_getId(), DEC); // TODO
		myDebugP(PSTR("Reconfigure it? (yes/no): "));
		Helper_serialReadLine(SerialMenu_str, SerialMenu_MAX_LEN);

		int i=0;
		while(SerialMenu_str[i]){
			SerialMenu_str[i] = tolower((char)SerialMenu_str[i]);
			i++;
		}
		Serial.println(SerialMenu_str);
		
		if(strcmp(SerialMenu_str, "yes") != 0){ // if not yes
			myDebugP(PSTR("Good bye\n"));
			return;
		}
		// if yes, continue
	}

	// if we do a reconfiguration, we can skip entries by 
	// just hitting enter. 

	uint32_t id;
	do{
		myDebugP(PSTR("Please Enter ID: "));
		Helper_serialReadLine(SerialMenu_str, SerialMenu_MAX_LEN);
		id = Helper_str2uint32(SerialMenu_str);
		Serial.println(id);

		if(id < 1 && !Settings_areKeysConfigured()){
			myDebugP(PSTR("Error: ID must be >=1\n"));
		}
	}while(id < 1 && !Settings_areKeysConfigured());
	if(id >= 1){
		Settings_setId(id);
	}

	int len = -1;
	while(!(
				len==10 || 
		    	(len==0 && Settings_areKeysConfigured())
		   )
		 ){
		myDebugP(PSTR("Please Enter Secret-Key (10 characters): "));
		Helper_serialReadLine(SerialMenu_str, SerialMenu_MAX_LEN);
		len = strlen(SerialMenu_str);
		if(len!=10){ // TODO: don't show error on skip!
			myDebugP(PSTR("Error: key length != 10\n"));
		}
	}
	if(len==10){
		Serial.print(SerialMenu_str);
		Settings_setSecKey(SerialMenu_str);
	}
	Serial.println("");
	

	len = -1;
	while(!(
				len==10 || 
		    	(len==0 && Settings_areKeysConfigured())
		   )
		 ){
		myDebugP(PSTR("Please Enter Public-Key (10 characters): "));
		Helper_serialReadLine(SerialMenu_str, SerialMenu_MAX_LEN);
		len = strlen(SerialMenu_str);
		if(len!=10){ // TODO: don't show error on skip!
			myDebugP(PSTR("Error: key length != 10\n")); // TODO dublicated PSTR
		}
	}
	if(len==10){
		Serial.print(SerialMenu_str);
		Settings_setPubKey(SerialMenu_str);
	}
	Serial.println("");	
	
	Settings_setKeysConfigured(true);

	myDebugP(PSTR("Do you want to set SSID and WiFi-Password now (yes/no)?\n"));
	Helper_serialReadLine(SerialMenu_str, SerialMenu_MAX_LEN);

	int i=0;
	while(SerialMenu_str[i]){
		SerialMenu_str[i] = tolower((char)SerialMenu_str[i]);
		i++;
	}
	Serial.println(SerialMenu_str);
	
	if(strcmp(SerialMenu_str, "yes") != 0){ // if not yes
		myDebugP(PSTR("Good bye\n"));
		return;
	}
	// if yes, continue


	// SSID
	len = 31;
	while(	!( 
				(len >= 1 && len <=30) || // length = 1...30
				(len==0 && Settings_isWifiConfigured())  // or skip
			)
		){
		myDebugP(PSTR("Please Enter WiFi-SSID (max. 30 characters): "));
		Helper_serialReadLine(SerialMenu_str, SerialMenu_MAX_LEN);
		len = strlen(SerialMenu_str);
		if(len>30){
			myDebugP(PSTR("Error: SSID length != 1...30\n")); // TODO dublicated PSTR
		}
	}
	if(len >= 1 && len <= 30){
		myDebug(SerialMenu_str);
		Settings_setSsid(SerialMenu_str);
	}
	myDebugP(PSTR("\n"));



	// WiFi Password
	// since no password is possible for open WLAN, we can not skip this!
	len = 31;
	while(	!( 
				(len >= 0 && len <=30)  // length = 1...30
			)
		)
	{
		myDebugP(PSTR("Please Enter WiFi password (0...30 characters): "));
		Helper_serialReadLine(SerialMenu_str, SerialMenu_MAX_LEN);
		len = strlen(SerialMenu_str);
		if(len>30){
			myDebugP(PSTR("Error: password length != ...30\n")); // TODO dublicated PSTR
		}
	}
	if(len >= 1 && len <=30){
		myDebug(SerialMenu_str);
		Settings_setPwd(SerialMenu_str);
	}else{
		myDebugP(PSTR("No Password (open WiFi)"));
	}
	myDebugP(PSTR("\n"));

	Settings_setWifiConfigured(true);

	
	myDebugP(PSTR("The configuration is finished. Please go on with the smart phone app\n"));
}

