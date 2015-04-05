// idle.ino



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



#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#include "idle.h"
#include "helper.h"

volatile int16_t tic;


// NOTE: Fuse WDTON must not be programmed (which is the case for the fuses
// used with this bootloader [-U lfuse:w:0xd6:m -U hfuse:w:0xda:m -U efuse:w:0x07:m ])
void Idle_for(int16_t seconds)
{
	//disable interrupts
	cli();
	//reset watchdog
	wdt_reset();

	/* Clear Watchdog Reset Flag in MCUStatusReg */
	MCUSR &= ~(1<<WDRF);

	//set up WDT interrupt
	WDTCSR |= (1<<WDCE)|(1<<WDE);
	//Start watchdog timer with 1s prescaler
	WDTCSR = (1<<WDIE)|(1<<WDP2)|(1<<WDP1); // interrupt only, 1s timeout period
		// TODO: remove WDP0!!! is just for debugging!
	//Enable global interrupts
	sei();


	sleep_disable();
	//set_sleep_mode(SLEEP_MODE_PWR_SAVE);
	set_sleep_mode(SLEEP_MODE_PWR_DOWN); 
		// measured: 175-185uA from battery (3.4.2015)
		// (remaining current sinks have to be identified...)
	//Set sleep enable (SE) bit:
	sleep_enable();

	char str[30];
	// sprintf(str, "WDTCSR=0x%x\n", (uint16_t) WDTCSR);
	// 	myDebug(str);
	// sprintf(str, "MCUSR=0x%x\n", (uint16_t) MCUSR);
	// 	myDebug(str);

	tic = 10;
 	for(int i=0; i<seconds; i++){
		wdt_reset();
   //  	// Put the device to sleep:
		sleep_mode();

		// wake up here..

	}	
 
    // sleep_disable();
    wdt_disable();
}

// this ISR is just used for waking up. 
// we resume where we went to power-save-mode
ISR(WDT_vect)
{
	// nothing to do, just reset the Interrupt flag (done by hardware)
}
