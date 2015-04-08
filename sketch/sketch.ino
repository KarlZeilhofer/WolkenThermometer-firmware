// sketch.ino


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
#include <SoftwareSerial.h>
#include <OneWire.h>

#include "button.h"
#include "helper.h"
#include "idle.h"
#include "pins.h"
#include "serialmenu.h"
#include "settings.h"
#include "temperature.h"
#include "timer.h"
#include "voltage.h"
#include "wt-wifi.h"


#include <string.h>
#include <sha1.h>
#include <aes256.h>
#include <twi.h>


#include "defs.h"


// all configuration starts with a reset!
void setup()
{
	Serial.begin(9600);
#ifdef SS_DEBUG
	mySerial.begin(9600);
#endif

	myDebug(("\nProject: WolkenThermometer V1.0\n"));

	
	Pins_init();
	myDebug("A\n");
	Timer_init_T1(); // sets up a 1Hz interrupt
	myDebug("B\n");

	if(Settings_areKeysConfigured()){
		myDebug("C\n");
		while(Button_isPressed()); // wait for release
		myDebug("D\n");

		
		// TODO: change back the time limits to 5-10 and 20-30s
		if(Button_PressedDuration >= 1 && Button_PressedDuration <=3){
			myDebug("E\n");
			Timer_blinkingLed = true; // blinking LED while configuring Wifi. 
			WifiConfig_run(); 
			Timer_blinkingLed = false;
			Pins_enableLed(0); // disable LED
		}else if(Button_PressedDuration >= 4 && Button_PressedDuration <=8){
		myDebug("F\n");
			Pins_enableLed(1);
			SerialMenu_setup();
			Pins_enableLed(0);
		}
	}
	myDebug("G\n");
	
	if(Settings_areKeysConfigured() == false){
		myDebug("H\n");
		Pins_enableLed(1);
		SerialMenu_setup();
		Pins_enableLed(0);
	}
	if(Settings_isWifiConfigured() == false){
		myDebug("H2\n");
		//myDebug(str123);
		myDebugP(PSTR("Wifi is not configured! Press button for 1-3 seconds,\nto enter Wifi-Config-Mode - Halt!")); // TODO: communicate this state with LED!
		while(1); // TODO: show this state with LED!
		// we must enter the wifi-config via the button, so we get a 
		// randomized seed!
	}
	myDebug("I\n");

	Pins_enableLed(true);
	Temp_init();
	Pins_enableLed(false);
	myDebug("J\n");

}

// when all configurations are fine, go to main loop!
void loop()
{
	myDebugP(PSTR("looping luie"));

	
	// turn sensor on
	Pins_enableLed(true);
	delay(10);

	// measure temperature
	Temp_read(); // result is in Temp_raw
	Volt_read();

	// measure voltage:
	// TODO

	// Turn sensor off:
	Pins_enableLed(false);
	Temp_toStr(); // convert result to string, result is in Temp_str
	Volt_toStr();

	// turn Wifi on. 
	Pins_enableWifi(true);

	// connect to wifi-access point
	// transmit data:
	if(Wifi_connectToAccessPoint()){
		Wifi_sendData();
	}
	
	// recieve server-reply (next seed (uin16_t) or error=0)
	// TODO

	// turn wifi off
	Pins_enableWifi(false);

	Timer_timeOutCounter = 60*10-30; // 9:30 minutes

	// wait 10 minutes = 60 times 10s
	for(int i=0; i<60; i++){
		Idle_for(10); // idle for 10s

		// check voltage
		if(Volt_permill>980){
			Pins_enableLoadResistor(true);
			delay(1000L*28);
			Pins_enableLoadResistor(false);

			Pins_enableLed(true);
			delay(1000L*2);
			Volt_read();
			Pins_enableLed(false);
		}
	}
}
