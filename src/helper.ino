// helper.ino


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



#include "defs.h"
#include <avr/pgmspace.h>
#include "helper.h"

#ifdef SS_DEBUG
SoftwareSerial mySerial(ssRx_Pin, ssTx_Pin); // TODO: can we ommit Tx-Pin?
#endif

// maxLen including termination zero.
// CR or LF are not contained.
// CR are skipped
// LF is the end-of-line merker
// serial input is read until LF, independent of the number of received characters!
void Helper_serialReadLine(char* buffer, uint16_t maxLen)
{
	char ch=0;
	uint16_t i=0;
	do{
		while(Serial.available()==0); // wait for input
		ch = Serial.read();
		if(ch!='\r' && ch!='\n' && i<(maxLen-1)){
			buffer[i++]=ch;
		}
	}while(ch!='\n');
	buffer[i]=0; // terminate string
}

// very, very simple converter funktion. Without any error handling, or range checking!
uint32_t Helper_str2uint32(char* str)
{
	uint16_t i=0;
	uint32_t n=0; //number

	while(str[i] && isdigit(str[i])){ 
		n*=10;
		n+=str[i]-'0';
		i++;
	}
	return n;
}


void Helper_eepromWriteData(char* buffer, uint16_t len, int addr)
{
	for(int i=0; i<len; i++){
		EEPROM.write(addr+i, buffer[i]);
	}
}

void Helper_eepromReadData(char* buffer, uint16_t len, int addr)
{
	for(int i=0; i<len; i++){
		buffer[i] = EEPROM.read(addr+i);
	}
}

// hex buffer must have binLen*2+1 bytes reserved!
// the hex buffer is then zero-terminated!
// TODO: test this function
void Helper_expandFromBinToHex(uint8_t* bin, int binLen, char* hex)
{
	for(int i=0; i<binLen; i++){
		sprintf(hex+2*i, "%x%x", bin[i]>>4, bin[i]&0xf);
	}
}


void Helper_contractFromHexToBin(char* hex, int hexLen, uint8_t* bin)
{
	for(int i=0; i<hexLen; i+=2){
		uint8_t nibbleH = hex[i] > '9' ? (hex[i]|32) - 'a' + 10 : hex[i] - '0';
		uint8_t nibbleL = hex[i+1] > '9' ? (hex[i+1]|32) - 'a' + 10 : hex[i+1] - '0';
		bin[i/2]=nibbleH<<4 | nibbleL;
	}
}

void myDebug(const char* str) // write string to I2C
{
	uint8_t *heapptr, *stackptr;
	uint16_t diff=0;
	
// 	if(strlen(str)>1){
// 		stackptr = (uint8_t *)malloc(4);          // use stackptr temporarily
// 		heapptr = stackptr;                     // save value of heap pointer
// 		free(stackptr);      // free up the memory again (sets stackptr to 0)
// 		stackptr =  (uint8_t *)(SP);           // save value of stack pointer

// 		// print stackpointer:
// 		static char str2[20];
// 		sprintf(str2, "SP=%u HP=%u\n", (uint16_t)stackptr, (uint16_t)heapptr);
// #ifdef SS_DEBUG
// 		mySerial.write(str2);
// #endif
// 	}


#ifdef SS_DEBUG
	mySerial.write(str);
#endif

	// write also to the serial, if it is not used by the WiFi
	if(wifiIsOnFlag == false){
		Serial.write(str);
	}
	// for writing to the WiFi, use Serial.write() directly!
}

// PROGMEM aware wrapper:
void myDebugP(const char* str) // write string to I2C
{
	char ch_str[2]; // make as many strings as we have characters
	ch_str[1]=0; // terminate helper string. 
	uint16_t i=0; 
	while(ch_str[0] =  pgm_read_byte_near(str+i)){
		myDebug(ch_str);
		i++;
	}
}


// discard all data in buffer. 
void flushSerialRx(){
	while(Serial.available()){
		Serial.read(); 
	}
}