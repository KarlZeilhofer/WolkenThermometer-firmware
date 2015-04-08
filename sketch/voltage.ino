// voltage.ino


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




#include "voltage.h"
#include "pins.h"


uint32_t Volt_raw_times10; // 0...1023 = 0...5V
char Volt_str[10]; // e.g. 97.0 
int16_t Volt_permill;


// caller has to enable power to the reference!
void Volt_read()
{
	analogReference(EXTERNAL); // just to be sure not to 
		// shorten the internal ref with the external!

	const int N=1000; // do 1000 samples

	Volt_raw_times10 = 0;
	for(int i=0; i<N; i++){
		Volt_raw_times10 += analogRead(VoltageInput_Pin);
	}
	Volt_raw_times10 /= N/10; // 0...10230

	int32_t zero_point_1_mV = ((int32_t)Volt_raw_times10*1000*5)/1024;

	// x1 = 3.2V
	// x2 = 4.1V
	// y1 = 0%
	// y2 = 100%
	// percent =((1000pm)/900mV * (x-3200mV)) + 0
	Volt_permill =  (1000*(zero_point_1_mV-32000))/9000 + 0;
}

void Volt_toStr()
{
	int i = Volt_permill/10;
	int absI = abs(i);
	int d = abs(Volt_permill)%10;
	
	if(Volt_permill>=0){
		Volt_str[0]='+';
	}else{
		Volt_str[0]='-';
	}
	snprintf(Volt_str+1, 8, "%d.%u", absI, d); 
	// 4.1V = 100%
	// 3.2V = 0%
}

