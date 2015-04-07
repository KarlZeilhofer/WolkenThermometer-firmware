// temperature.src


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



#include "temperature.h"
#include "helper.h"


int16_t Temp_raw; // most recent raw value. 
char Temp_str[10]; 


// static means here "private" // TODO: check, if C and C++ is mixed up here!
static OneWire  ds(Sensor_Pin);  // on pin 8 ist the DS18B20
static uint8_t Temp_address[8];
static uint8_t Temp_sensorType; // sensor type

// read out address of sensor
void Temp_init()
{
	uint8_t i;

	byte data[12];
	uint8_t* addr = Temp_address; // just rename it to the example code
	float celsius, fahrenheit;

	// since we have only one sensor, we will reset the search every time. 
	ds.reset_search();
	ds.search(addr);

	myDebugP(PSTR("ROM ="));
	for( i = 0; i < 8; i++) {
		Serial.write(' ');
		Serial.print(addr[i], HEX);
	}

	if (OneWire::crc8(addr, 7) != addr[7]) {
		myDebugP(PSTR("CRC is not valid!"));
		return;
	}
	Serial.println();

	// the first ROM byte indicates which chip
	switch (addr[0]) {
	case 0x10:
		myDebugP(PSTR("  Chip = DS18S20"));  // or old DS1820
		Temp_sensorType = 1;
		break;
	case 0x28:
		myDebugP(PSTR("  Chip = DS18B20"));
		Temp_sensorType = 0;
		break;
	case 0x22:
		myDebugP(PSTR("  Chip = DS1822"));
		Temp_sensorType = 0;
		break;
	default:
		myDebugP(PSTR("Device is not a DS18x20 family device."));
		return;
	} 

}

void Temp_read()
{
	Temp_init(); // TODO: can we remove this?

	uint8_t present = 0;
	uint8_t* addr = Temp_address; // just rename it to the example code
	bool crcOK = false;
	int8_t retries = 5;
	uint8_t data[9];

	ds.reset();
	ds.select(addr);
	ds.write(0x44);        // start conversion, no parasite power
	delay(800);     // maybe 750ms is enough, maybe not
	// we might do a ds.depower() here, but the reset will take care of it.

	while(retries>0 && !crcOK){
		retries--;
		present = ds.reset();
		ds.select(addr);    
		ds.write(0xBE);         // Read Scratchpad

		for (uint8_t i = 0; i < 9; i++) {           // we need 9 bytes
			data[i] = ds.read();
		}
		if( OneWire::crc8(data, 8) == data[8]){
			crcOK = true;
		}else{
			myDebugP(PSTR("CRC Error in Sensor Read out\n"));
		}

		/// no CRC check:
//		crcOK = true; 
	}
	if(!crcOK){
		return; // caller has to check, if we have a valid string. 
	}

	// Convert the data to actual temperature
	// because the result is a 16 bit signed integer, it should
	// be stored to an "int16_t" type, which is always 16 bits
	// even when compiled on a 32 bit processor.
	int16_t raw = (data[1] << 8) | data[0];
	if (Temp_sensorType) {
		raw = raw << 3; // 9 bit resolution default
	if (data[7] == 0x10) {
		// "count remain" gives full 12 bit resolution
		raw = (raw & 0xFFF0) + 12 - data[6];
		}
	} else {
		byte cfg = (data[4] & 0x60);
		// at lower res, the low bits are undefined, so let's zero them
		if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
		else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
		else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
		//// default is 12 bit resolution, 750 ms conversion time
	}

	Temp_raw = raw;
}


char* Temp_toStr() // returns a pointer to a static string. 
{
	Temp_str[0]=0;

	
	int16_t sign = 0;
	int16_t absRaw = 0;
	int16_t absGanzzahl = 0;
	if(Temp_raw>=0){
		sign = 1;
		absRaw = Temp_raw;
	}else{
		sign = -1;
		absRaw = -Temp_raw;
	}

	absGanzzahl = absRaw/16;
	int16_t nachkomma100stel = absRaw%16;
	nachkomma100stel = ((nachkomma100stel*1000)/16 + 5)/10; 

	if(sign>0){
		Temp_str[0]='+';
	}else{
		Temp_str[0]='-';		
	}
	snprintf(Temp_str+1, 8, "%d.%02u", absGanzzahl, 
		(uint16_t)(nachkomma100stel));

	return Temp_str;
}