// wt-wifi.ino
// refer to ../wifi-proedure.txt


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


#include "wt-wifi.h"
#include "helper.h"
#include "timer.h"
#include "settings.h"
#include <aes256.h>
#include <sha1.h>
#include "temperature.h"
#include "voltage.h"

#define WifiConfig_UDP_Port  (8784U)  // port for outgoing UDP broadcast messages. 
	// is acii for "WT" expressed as decimal numbers 87 and 84 :)
#define WifiConfig_TCP_Port  (WifiConfig_UDP_Port+1U) // Port for incomint TCP connection 


static uint32_t WifiConfig_seed; // is set to microseconds uptime
static uint32_t uploadSeed;




#define WIFI_mml 200 // Wifi max message length excluding zero. 
static char Wifi_msg[WIFI_mml+1]; 
	// the input packet was calculated to be of the size of max. 170 bytes. 
	// refer to wifi-procedure.txt

const char TIMEOUT_FMT_STR[] PROGMEM = "Timed out in %s:%d\n";
const char ERROR_FMT_STR[] PROGMEM = "got 'ERROR' in %s:%d\n";


// PASS PROGMEM-STINGS ONLY!!
// any string can be null, if not used
// returns found string-id (1-3) on success
// 0 on time-out
// caller must set Timer_timeOutCounter!!
// Attention: the string must not contain a substring with length of 3 or more 
// characters, with which it starts!
// invalid string: "dogEatsManyHotodogs"
// the algorithm starts from the beginning, if a match fails for one character. 
// e.g. "\r\nOK\r\n" will be fine, since the identical substring has length 2. 
// it would fail on the searchstring "abcXabcXY", if the stream is
// "abcXabcXabcXY"
uint8_t waitForString_P(const char* str1, const char* str2=0, const char* str3=0);
	// cpp style for variable number of arguments. 














bool WifiConfig_run()
{
	myDebug(__func__); myDebug("()\n");

	bool success = false;

	Pins_enableWifi(true);

	// set up TCP server and periodical UDP-broadcast:
	delay(1000); flushSerialRx();
	sprintf_P(Wifi_msg, PSTR("\r\nAT+RST\r\n"));
		Serial.write(Wifi_msg);
		myDebug(Wifi_msg);
	Timer_timeOutCounter = 10;
	switch(waitForString_P(PSTR("\r\nready\r\n"))){
		case 1: 
			sprintf_P(Wifi_msg, PSTR("got 'ready' after RST in %s:%d\n"), __FILE__, __LINE__);
			myDebug(Wifi_msg);
		break;
		default:
			sprintf_P(Wifi_msg, TIMEOUT_FMT_STR, __FILE__, __LINE__);
			myDebug(Wifi_msg);
	}



	delay(1000); flushSerialRx();
	sprintf_P(Wifi_msg, PSTR("AT+CWMODE=3\r\n")); // station + access point. 
		Serial.write(Wifi_msg);
		myDebug(Wifi_msg);
		// station is neded here, so that we can open a tcp server. 
	// wait for "\r\nno change\r\n" or "\r\nOK\r\n"
	Timer_timeOutCounter = 5;
	switch(waitForString_P(PSTR("\r\nno change\r\n"), PSTR("\r\nOK\r\n"))){
		case 1: 
			sprintf_P(Wifi_msg, PSTR("got 'no change' for CWMODE in %s:%d\n"), __FILE__, __LINE__);
			myDebug(Wifi_msg);
		break;
		case 2:
			sprintf_P(Wifi_msg, PSTR("got 'OK' in %s:%d\n"), __FILE__, __LINE__);
			myDebug(Wifi_msg);
		break;
		default:
			sprintf_P(Wifi_msg, TIMEOUT_FMT_STR, __FILE__, __LINE__);
			myDebug(Wifi_msg);
	}



	delay(1000); flushSerialRx();
	sprintf_P(Wifi_msg, PSTR("AT+RST\r\n"));
		Serial.write(Wifi_msg);
		myDebug(Wifi_msg);
	// wait for "\r\nready\r\n"
	Timer_timeOutCounter = 10;
	switch(waitForString_P(PSTR("\r\nready\r\n"))){
		case 1: 
			sprintf_P(Wifi_msg, PSTR("got 'ready' after RST in %s:%d\n"), __FILE__, __LINE__);
			myDebug(Wifi_msg);
		break;
		default:
			sprintf_P(Wifi_msg, TIMEOUT_FMT_STR, __FILE__, __LINE__);
			myDebug(Wifi_msg);
	}



	delay(1000); flushSerialRx();
	sprintf_P(Wifi_msg, PSTR("AT+CWSAP=\"WT_ID_%lu\",\"\",1,0\r\n"), Settings_getId()); // no '-' in ssid allowed!
		Serial.write(Wifi_msg);
		myDebug(Wifi_msg);
	// wait for "\r\nOK\r\n" or "\r\nERROR\r\n"
	Timer_timeOutCounter = 3;
	switch(waitForString_P(PSTR("\r\nOK\r\n"), PSTR("\r\nERROR\r\n"))){
		case 1: 
			sprintf_P(Wifi_msg, PSTR("got 'OK' for CWSAP in %s:%d\n"), __FILE__, __LINE__);
			myDebug(Wifi_msg);
		break;
		case 2:
			sprintf_P(Wifi_msg, ERROR_FMT_STR, __FILE__, __LINE__);
			myDebug(Wifi_msg);
		break;
		default:
			sprintf_P(Wifi_msg, TIMEOUT_FMT_STR, __FILE__, __LINE__);
			myDebug(Wifi_msg);
	}




	delay(1000); flushSerialRx();
	sprintf_P(Wifi_msg, PSTR("AT+CIPMUX=1\r\n")); // no '-' in ssid allowed!
		Serial.write(Wifi_msg);
		myDebug(Wifi_msg);
	// wait for "\r\nOK\r\n" or "\r\nERROR\r\n"
	Timer_timeOutCounter = 3;
	switch(waitForString_P(PSTR("\r\nOK\r\n"), PSTR("\r\nERROR\r\n"))){
		case 1: 
			sprintf_P(Wifi_msg, PSTR("got 'OK' for CIPMUX in %s:%d\n"), __FILE__, __LINE__);
			myDebug(Wifi_msg);
		break;
		case 2:
			sprintf_P(Wifi_msg, ERROR_FMT_STR, __FILE__, __LINE__);
			myDebug(Wifi_msg);
		break;
		default:
			sprintf_P(Wifi_msg, TIMEOUT_FMT_STR, __FILE__, __LINE__);
			myDebug(Wifi_msg);
	}




	delay(1000); flushSerialRx();
	sprintf_P(Wifi_msg, PSTR("AT+CIPSERVER=1,%u\r\n"),WifiConfig_TCP_Port);
		Serial.write(Wifi_msg);
		myDebug(Wifi_msg); // tcp server socket
	// wait for "\r\nOK\r\n" or "\r\nERROR\r\n"
	Timer_timeOutCounter = 3;
	switch(waitForString_P(PSTR("\r\nOK\r\n"), PSTR("\r\nERROR\r\n"))){
		case 1: 
			sprintf_P(Wifi_msg, PSTR("got 'OK' CIPSERVER in %s:%d\n"), __FILE__, __LINE__);
			myDebug(Wifi_msg);
		break;
		case 2:
			sprintf_P(Wifi_msg, ERROR_FMT_STR, __FILE__, __LINE__);
			myDebug(Wifi_msg);
		break;
		default:
			sprintf_P(Wifi_msg, TIMEOUT_FMT_STR, __FILE__, __LINE__);
			myDebug(Wifi_msg);
	}


	// UDP Broadcast socket
	delay(1000); flushSerialRx();
	sprintf_P(Wifi_msg, PSTR("AT+CIPSTART=1,\"UDP\",\"192.168.4.255\",%u\r\n"), WifiConfig_UDP_Port);
		Serial.write(Wifi_msg);
		myDebug(Wifi_msg);

	// wait for "\r\nOK\r\n" or "\r\nERROR\r\n"
	Timer_timeOutCounter = 3;
	switch(waitForString_P(PSTR("\r\nOK\r\n"), PSTR("\r\nERROR\r\n"))){
		case 1: 
			sprintf_P(Wifi_msg, PSTR("got 'OK' for CIPSTART in %s:%d\n"), __FILE__, __LINE__);
			myDebug(Wifi_msg);
		break;
		case 2:
			sprintf_P(Wifi_msg, ERROR_FMT_STR, __FILE__, __LINE__);
			myDebug(Wifi_msg);
		break;
		default:
			sprintf_P(Wifi_msg, TIMEOUT_FMT_STR, __FILE__, __LINE__);
			myDebug(Wifi_msg);
	}

	
	
	delay(1000); flushSerialRx();
	myDebugP(PSTR("Waiting for incoming TCP connection"));


	// wait for connected keyword (on timeout (300s) power everything down, and go to sleep)
	
	success = false;
	WifiConfig_seed = micros(); // micros is a random number here, since this config is 
		// triggered by a button release!	
	Timer_timeOutCounterTCP = 300;
	do{ // double-do-while, so we can use break, to start a new iteration. 
	do{
		myDebugP(PSTR("Starting do-while-loop\n"));
		// Send a UDP packet every 5 seconds, max. 60 times
		bool gotPacketStart=false;
		
		// prepare UDP-packet:
		// do this here, just to get the length: (do it again later. this sves RAM)
		sprintf_P(Wifi_msg, PSTR("WTID=%lu&TCP-Port=%u&seed=%lu"), Settings_getId(), WifiConfig_TCP_Port, WifiConfig_seed);
		uint8_t udpLen = strlen(Wifi_msg);

		for(int udpTxCount=0; udpTxCount<60; udpTxCount++){
			// UDP-Packet transmit

			sprintf_P(Wifi_msg, PSTR("AT+CIPSEND=1,%u\r\n"), (uint16_t)udpLen);
				Serial.write(Wifi_msg); 
			// DO NOT WAIT for THE START SIGN '>'!!
			// HERE WE POTENTIALLY LOOSE A DATA PACKET

			delay(2000);
			
			Timer_timeOutCounter = 5; 
			sprintf_P(Wifi_msg, PSTR("WTID=%lu\nTCP-Port=%u\nseed=%lu\r\n"), Settings_getId(), WifiConfig_TCP_Port, WifiConfig_seed);
				// note: the trailing \r\n is not part of the UDP-packet. 
				// it belongs to the AT-command
				Serial.write(Wifi_msg);
			if(waitForString_P(PSTR("\r\n+IPD,0,"))){	 
				sprintf_P(Wifi_msg, PSTR("got '+IPD,0,' TCP packet in %s:%d\n"), __FILE__, __LINE__);
				myDebug(Wifi_msg);
				gotPacketStart = true;
			}else{
				myDebugP(PSTR("Still no packet received...\n"));
			}
		}
		if(gotPacketStart == false){
			break; // restart do-while
		}
		// then directly after this comes the data-length in bytes as a decimal number
		// e.g. "+IPD,0,5:HelloWorld!"
		// after this we get an "\r\nOK\r\n", for finished receiption. 

		// read packet length
		int i=0; 
		char ch = 0;
		Timer_timeOutCounter = 5;
		while(ch != ':' && i<10 && Timer_timeOutCounter){
			if(Serial.available()){
				Wifi_msg[i]=ch=Serial.read();
				i++;
			}
		}
		Wifi_msg[i]=0; // terminate string
		uint16_t packetLen = Helper_str2uint32(Wifi_msg);
		sprintf_P(Wifi_msg, PSTR("Packetlength: %u\n"), packetLen);
		myDebug(Wifi_msg);
		if(packetLen > WIFI_mml){
			myDebugP(PSTR("Received packet too long. Skip it!\n"));
			delay(1000);
			flushSerialRx();
			break; // skip this data packet
		}

		i=0; // read packet data
		ch = 0;
		Timer_timeOutCounter = 5;
		while(i<WIFI_mml && i<packetLen && Timer_timeOutCounter){ // TODO use a constant
			if(Serial.available()){
				Wifi_msg[i]=ch=Serial.read();
				i++;
			}
		}
		if(i==WIFI_mml){ // TODO use a constant
			myDebugP(PSTR("Buffer exceeded!\n"));
			break; // restart do-while
		}
		Wifi_msg[i]=0; // terminate buffer!

		bool gotData = false;
		if(packetLen && Timer_timeOutCounter){
			gotData = true;
		}

		char* Wifi_msg = Wifi_msg; // same buffer for saving RAM !!!
		// only the data comes into in the buffer now!
		// format: "encoded-Wifi_msg-as-hex-string&hash-as-hex-string&"
		// example: "a94a8fe5ccb19ba61c4c0873d391e987&46237b3d702aad617eae653792a6269e836aa44d&"

		if(gotData){
			if(1){ // TODO !!! remove infinite loop
				myDebugP(PSTR("packet data: "));
				myDebug(Wifi_msg);
				delay(2000);
			}
						// wait for datastart (on timeout (300s) power everything down, and go to sleep)
			// write data into Wifi_msg



			// parse data
			char* cipherText= Wifi_msg; // same memory! saves RAM!!
			int cipherLen=0;
			char* hash=0; 
			int i=0;
			while(Wifi_msg[i] && Wifi_msg[i]!='&'){
				i++;
			}
			cipherLen = i;
			cipherText[cipherLen]=0; // terminate cipherText

			if(cipherLen%32 != 0){
				myDebugP(PSTR("cipherText-length as hex "));
				myDebugP(PSTR("is not multiple of 32\n"));
				myDebugP(PSTR("Discard this packet\n"));

				delay(1000);
				flushSerialRx();
				break; // restart do-while
			}

			i++; // skip &
			hash = cipherText+i; // set the hash-pointer to somewhere in the middle of the string
			
			// finde tereminating '&'
			int j=0;
			while(Wifi_msg[i] && Wifi_msg[i]!='&'){
				i++;j++;
			}
			hash[j]=0; // terminate string

			myDebugP(PSTR("received hash as hex: '"));
			myDebug(hash);
			myDebugP(PSTR("'\n"));

			if(j!=40){
				char str[10]; // temporary string for printing. 
				myDebugP(PSTR("wrong length of received hash: "));
				sprintf_P(str, PSTR("%u\n"), (uint16_t)j);
				myDebug(str);
				myDebugP(PSTR("Skip this packet\n"));

				delay(1000);
				flushSerialRx();
				break;
			}


			// convert hash to binary format (into the same memory!!!):
			Helper_contractFromHexToBin(hash, 40, (uint8_t*)hash);
			// hash has a length of 20 bytes now

			// AES256-key = publickey . secretkey . seed-as-decimal-string . binary-zero-pad-to-32-chars
			// (256 bit = 32 byte)
			char aesKey[32]={0}; // auto-zero-padding (important!)
			{ // block for only temporary stack-usage. 
				char sk[11];
				char pk[11];

				Settings_getSecKey(sk);
				Settings_getPubKey(pk);

				sprintf_P(aesKey, PSTR("%s%s%lu"), sk, pk, WifiConfig_seed);
			}
			myDebugP(PSTR("aesKey="));
			myDebug(aesKey);  // TODO: !!! remove this from production code!


			// replace the hex CT with the binary CT 
			// again here into the same memory space to save RAM. 
			Helper_contractFromHexToBin(cipherText, cipherLen, (uint8_t*)cipherText);
			cipherLen/=2; // update cipher-length

			// decrypt cipher text
			aes256_context ctx; // aes256-object
			aes256_init(&ctx, (uint8_t*) aesKey);
			for(int i=0; i<cipherLen; i+=16){ // decrypt in blocks of 16 bytes
				aes256_decrypt_ecb(&ctx, (uint8_t*)(cipherText+i));
			}

			cipherText[cipherLen] = 0; // terminate result-string. 
			// format: "<secret-ssid>\n<secret-pwd>\n" . binary-zero-pad-to-multiple-of-16-chars
			// example: "MKZ-WLAN\nKarlZei123\n [+12*\0]"
			char* plainText = cipherText; // rename string!
			int plainLen = cipherLen;
			
			myDebugP(PSTR("Decrypted plain text:\n"));
			myDebug(plainText);  
			myDebugP(PSTR("\n"));

			// extract ssid and password
			// use same memory space here again!!!
			char* ssid = plainText;
			i=0;
			while(plainText[i]!='\n' && i<30){
				i++;
			}
			ssid[i]=0; // terminate ssid
			uint8_t ssidLen = i;
			i++; // skip '\n'
			int a=i;
			char* pwd = plainText+i;
			i=0;
			while(plainText[a+i]!='\n' && i<30){
				i++;
			}
			pwd[i]=0; // terminate pwd
			uint8_t pwdLen = i;

			myDebugP(PSTR("SSID = "));
			myDebug(ssid);
			myDebugP(PSTR("\nPassword = "));
			myDebug(pwd);
			myDebugP(PSTR("\n"));

			// check hash value
			// but first move received hash to the bottom of the buffer:
			memcpy(Wifi_msg, hash, 20);
			hash = Wifi_msg+0; 
			char* hashInput = Wifi_msg+20; // now we can savely use this memory again
				// theortically we can use now 180 bytes, but we need 
				// only max. 30+30+10+1 = 71 bytes
			// hash:= sha1(secret-ssid . secret-pwd . seed-as-decimal-string)
			sprintf_P(hashInput, PSTR("%s%s%lu"), ssid, pwd, WifiConfig_seed);
			SHA1_Digest ourHash = SHA1_get(hashInput, strlen(hashInput));  
		

			if(memcmp(hash, &ourHash.digest, 20)==0){
				myDebug("Hash is valid, saving SSID and Password in settings!\n");

				// save them in the settings
				// terminate password on it's new place in memory:
				hashInput[ssidLen + pwdLen]=0;
				Settings_setPwd(hashInput+ssidLen);
				// terminate ssid:
				hashInput[ssidLen]=0; // we overwrite here the first char of pwd!!!
				Settings_setSsid(hashInput);
				Settings_setWifiConfigured(true);
				success = true;

				// TODO: send reply to smart phone!
			}else{
				myDebug("Hash Error. Try it again.\n");
			}
		}
	}while(Timer_timeOutCounterTCP && !success);
	}while(Timer_timeOutCounterTCP && !success); // double-loop!


	Pins_enableWifi(false);

	if(Timer_timeOutCounterTCP == 0){
		myDebugP(PSTR("'Waiting for Data' timed out\n")); 
		return false;
	}else{
		return true;
	}
}















// use PROGMEM strings!!!
// C++ style of function!
uint8_t waitForString_P(const char* str1, const char* str2, const char* str3)
{
	bool found=false;
	const char* str[3]={str1, str2, str3};
	int l[3]={0}; // string lengths
	int mc[3]={0}; // character matching counter

	
	for(int s=0; s<3; s++){
		if(str[s]){
			l[s] = strlen_P(str[s]);
		}else{
			l[s] = -1; // set negative, so the condition mc2!=l2 is true (see while-loop)!
		}
	}


	while(Timer_timeOutCounter){
		if(Serial.available()){
			char ch = Serial.read();
			char dstr[2]; // debug-string
			dstr[0]=ch;
			dstr[1]=0;
			//myDebug(dstr); // TODO remove
	
			for(int s=0; s<3; s++){
				if(str[s]){
					if(ch == pgm_read_byte_near(str[s] + mc[s])){ // translates to e.g. str1[mc1]
						mc[s]++;
						if(mc[s]==l[s]){
							return s+1;
						}
					}else{
						if(ch == pgm_read_byte_near(str[s] + 0)){
							mc[s]=1;
							if(mc[s]==l[s]){
								return s+1;
							}
						}else{
							mc[s]=0;
						}
					}
				} // end if(str)
			} // end for
		}// end if Serial.available()
	}// end while


	return 0;
}


















// return true on success. 
bool Wifi_connectToAccessPoint() // and send. TODO: split function up
{
	// AT+RST
	// AT+WCMODE=1
	// AT+RST
	// AT+CWJAP="ssid","password"


	if(Settings_isWifiConfigured() == false){
		myDebugP(PSTR("Missing Wifi Config\n"));
		return false;
	}

		// set up TCP server and periodical UDP-broadcast:
	delay(1000); flushSerialRx();
	sprintf_P(Wifi_msg, PSTR("\r\nAT+RST\r\n"));
		Serial.write(Wifi_msg);
		myDebug(Wifi_msg);
	Timer_timeOutCounter = 10;
	switch(waitForString_P(PSTR("\r\nready\r\n"))){
		case 1: 
			sprintf_P(Wifi_msg, PSTR("got 'ready' after RST in %s:%d\n"), __FILE__, __LINE__);
			myDebug(Wifi_msg);
		break;
		default:
			sprintf_P(Wifi_msg, TIMEOUT_FMT_STR, __FILE__, __LINE__);
			myDebug(Wifi_msg);
			return false;
	}



	delay(1000); flushSerialRx();
	sprintf_P(Wifi_msg, PSTR("AT+CWMODE=1\r\n")); // station only! 
		Serial.write(Wifi_msg);
		myDebug(Wifi_msg);
		// station is neded here, so that we can open a tcp server. 
	// wait for "\r\nno change\r\n" or "\r\nOK\r\n"
	bool skip2ndRst = false;
	Timer_timeOutCounter = 5;
	switch(waitForString_P(PSTR("\r\nno change\r\n"), PSTR("\r\nOK\r\n"))){
		case 1: 
			sprintf_P(Wifi_msg, PSTR("got 'no change' for CWMODE in %s:%d\n"), __FILE__, __LINE__);
			myDebug(Wifi_msg);
			skip2ndRst=true;
		break;
		case 2:
			sprintf_P(Wifi_msg, PSTR("got 'OK' in %s:%d\n"), __FILE__, __LINE__);
			myDebug(Wifi_msg);
		break;
		default:
			sprintf_P(Wifi_msg, TIMEOUT_FMT_STR, __FILE__, __LINE__);
			myDebug(Wifi_msg);
			return false;
	}


	if(!skip2ndRst){
		delay(1000); flushSerialRx();
		sprintf_P(Wifi_msg, PSTR("AT+RST\r\n"));
			Serial.write(Wifi_msg);
			myDebug(Wifi_msg);
		// wait for "\r\nready\r\n"
		Timer_timeOutCounter = 10;
		switch(waitForString_P(PSTR("\r\nready\r\n"))){
			case 1: 
				sprintf_P(Wifi_msg, PSTR("got 'ready' after RST in %s:%d\n"), __FILE__, __LINE__);
				myDebug(Wifi_msg);
			break;
			default:
				sprintf_P(Wifi_msg, TIMEOUT_FMT_STR, __FILE__, __LINE__);
				myDebug(Wifi_msg);
				return false;
		}
	}



	delay(1000); flushSerialRx();
	char ssid[31];
	char pwd[31];
	Settings_getSsid(ssid);
	Settings_getPwd(pwd);
	sprintf_P(Wifi_msg, PSTR("AT+CWJAP=\"%s\",\"%s\"\r\n"), ssid, pwd); // no '-' in ssid allowed!
	//sprintf_P(Wifi_msg, PSTR("AT+CWJAP=\"repeater\",\"fvo-rp27\"\r\n")); 
		Serial.write(Wifi_msg);
		myDebug(Wifi_msg);
	// wait for "\r\nOK\r\n" or "\r\nERROR\r\n"
	Timer_timeOutCounter = 10;
	switch(waitForString_P(PSTR("\r\nOK\r\n"), PSTR("\r\nERROR\r\n"))){
		case 1: 
			sprintf_P(Wifi_msg, PSTR("got 'OK' for CWJAP in %s:%d\n"), __FILE__, __LINE__);
			myDebug(Wifi_msg);
		break;
		case 2:
			sprintf_P(Wifi_msg, ERROR_FMT_STR, __FILE__, __LINE__);
			myDebug(Wifi_msg);
			return false;
		break;
		default:
			sprintf_P(Wifi_msg, TIMEOUT_FMT_STR, __FILE__, __LINE__);
			myDebug(Wifi_msg);
			return false;
	}




}


// send temperature and battery state to server
uint16_t Wifi_sendData()
{
	// AT+CIPSTART="TCP","me.zeilhofer.co.at",80
	// AT+CIPSEND=n
	// "HTTP GET /wt-test/dataupload.php?
	// 		sensorid=<SENSORID>&
	// 		temperature=<TEMPERATURE>&
	// 		battery=<BATTERY>&
	// 		hash=<HASH> HTTP/1.1\r\n"
	// "Host: me.zeilhofer.co.at:80\r\n"
	// "\r\n"

/* not implemented for EXPO show case:	
	// wait for "+IPD,"
	// (read packet-chunk-length, optional)
	// skip until "\r\n\r\n" (end of HTTP-header)
	// read server-response until "\r\n"
	// if response == 0, there was an error
	// else, it is the seed for the next hash (this will be redefined for future server-protocoll)




*/


	int retries = 2;
	int ret; // return value of wait-function

	while(retries){ // double loop, so we can use break
	while(retries){
		// connect to server
		delay(2000); flushSerialRx();
		sprintf_P(Wifi_msg, PSTR("AT+CIPSTART=\"TCP\",\"%S\",%u\r\n"), UPLOAD_HOST, UPLOAD_PORT);
			Serial.write(Wifi_msg);
			myDebug(Wifi_msg); // tcp server socket
		// wait for "\r\nOK\r\n" or "\r\nERROR\r\n"
		Timer_timeOutCounter = 20; // this high timeout is needed for Lackerserver
		ret = waitForString_P(PSTR("\r\nLinked\r\n"), PSTR("\r\nERROR\r\n"));
		if(ret == 1){
			sprintf_P(Wifi_msg, PSTR("got 'Linked' CIPSTART in %s:%d\n"), __FILE__, __LINE__);
			myDebug(Wifi_msg);
		}else if(ret == 2){
			sprintf_P(Wifi_msg, ERROR_FMT_STR, __FILE__, __LINE__);
			myDebug(Wifi_msg);
			retries--;
			break;
		}else{
			sprintf_P(Wifi_msg, TIMEOUT_FMT_STR, __FILE__, __LINE__);
			myDebug(Wifi_msg);
			retries--;
			break;
		}



		// HTTP GET Packet:
		delay(1000); flushSerialRx();
		// "GET /wt-test/dataupload.php?
		// 		sensorid=<SENSORID>&
		// 		temperature=<TEMPERATURE>&
		// 		battery=<BATTERY>&
		// 		hash=<HASH> HTTP/1.1\r\n"
		// "Host: me.zeilhofer.co.at:80\r\n
		// "\r\n"


		// TODO: use snprintf_P, since the server and path are of variable length
		// with the changeable defines

		// sum up packet length:
		uint16_t pLen=0;
		sprintf_P(Wifi_msg, PSTR("GET %S?sensorid=%lu"), UPLOAD_PATH, Settings_getId());
		pLen += strlen(Wifi_msg);
		sprintf_P(Wifi_msg, PSTR("&temperature=%s"), Temp_str);
		pLen += strlen(Wifi_msg);
		sprintf_P(Wifi_msg, PSTR("&battery=%s&hash=0 HTTP/1.1\r\n"
			"Host: %S:%u\r\n\r\n"), Volt_str, UPLOAD_HOST, UPLOAD_PORT);
		pLen += strlen(Wifi_msg);
		// now we know, how long the packet is. now we can sent it to the wifi-module:


		sprintf_P(Wifi_msg, PSTR("AT+CIPSEND=%u\r\n"), pLen);
			Serial.write(Wifi_msg); myDebug(Wifi_msg);


		Timer_timeOutCounter = 3; 
		ret=waitForString_P(PSTR("\r\n> "));
		if(ret == 1){
			sprintf_P(Wifi_msg, PSTR("got '> ' in %s:%d\n"), __FILE__, __LINE__);
			myDebug(Wifi_msg);
		}else{
			sprintf_P(Wifi_msg, TIMEOUT_FMT_STR, __FILE__, __LINE__);
			myDebug(Wifi_msg);
			retries--;
			if(retries==0){
				return false;
			}
			break; // retry
		}

		delay(500);

		sprintf_P(Wifi_msg, PSTR("GET %S?sensorid=%lu"), UPLOAD_PATH, Settings_getId());
			Serial.write(Wifi_msg); myDebug(Wifi_msg);
		delay(100);
		sprintf_P(Wifi_msg, PSTR("&temperature=%s"), Temp_str);
			Serial.write(Wifi_msg); myDebug(Wifi_msg);
		delay(100);
			sprintf_P(Wifi_msg, PSTR("&battery=%s&hash=0 HTTP/1.1\r\n"
			"Host: %S:%u\r\n\r\n\r\n\r\n"), Volt_str, UPLOAD_HOST, UPLOAD_PORT); // NOTE: we have added a 3rd \r\n. This terminates the AT-command. 
			Serial.write(Wifi_msg); myDebug(Wifi_msg);


		// for now, wait here for nothing, just return true. 
		// TODO: receive next-seed

		// wait for "\r\nSEND OK\r\n"
		Timer_timeOutCounter = 5; 
		ret = waitForString_P(PSTR("\r\nSEND OK\r\n"));
		if(ret == 1){
			sprintf_P(Wifi_msg, PSTR("got 'SEND OK' in %s:%d\n"), __FILE__, __LINE__);
			myDebug(Wifi_msg);
			retries = 0; // success
			break;
		}else{
			sprintf_P(Wifi_msg, TIMEOUT_FMT_STR, __FILE__, __LINE__);
			myDebug(Wifi_msg);
			retries--;
			if(retries==0){
				return false; // give up
			}
		}
	}
	}// double loop, so we can use break


	// wait for "\r\n+IPD,"
	Timer_timeOutCounter = 7; 
	switch(waitForString_P(PSTR("\r\n+IPD,"))){
		case 1: 
			sprintf_P(Wifi_msg, PSTR("got '+IPD' in %s:%d\n"), __FILE__, __LINE__);
			myDebug(Wifi_msg);
		break;
		default:
			sprintf_P(Wifi_msg, TIMEOUT_FMT_STR, __FILE__, __LINE__);
			myDebug(Wifi_msg);
			return false;
	}

	// wait for "\r\n\r\n" (end of http header)
	Timer_timeOutCounter = 3; 
	switch(waitForString_P(PSTR("\r\n\r\n"))){
		case 1: 
			sprintf_P(Wifi_msg, PSTR("got 'End of HTTP Header' in %s:%d\n"), __FILE__, __LINE__);
			myDebug(Wifi_msg);
		break;
		default:
			sprintf_P(Wifi_msg, TIMEOUT_FMT_STR, __FILE__, __LINE__);
			myDebug(Wifi_msg);
			return false;
	}

	char reply[11];
	Helper_serialReadLine(reply, 10);

	// read E or S for error or success
	if(reply[0] == 'E'){
			sprintf_P(Wifi_msg, PSTR("Got Error as Server reply\n"));
			myDebug(Wifi_msg);
	}
	if(reply[0] == 'S'){
			sprintf_P(Wifi_msg, PSTR("Got success as Server reply\n"));
			myDebug(Wifi_msg);
	}

	// skip '&'
	// read integer (nextSeed)
	uploadSeed = Helper_str2uint32(reply+2);
	sprintf_P(Wifi_msg, PSTR("nextSeed = %u\n"), uploadSeed);
	myDebug(Wifi_msg);

	return true;
}