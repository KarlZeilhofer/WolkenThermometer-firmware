// timer.h


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



#ifndef __TIMER_H
#define __TIMER_H


extern boolean Timer_blinkingLed;
extern boolean Timer_udpBroadCast; // every 5 seconds a broadcast is transmitted in wifi-configure-mode. 
extern volatile uint16_t Timer_timeOutCounter; // is decremented every second until zero. 
extern volatile uint16_t Timer_timeOutCounterTCP; // is decremented every second until zero. 

void Timer_init_T1();


#endif