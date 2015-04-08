// timer.ino


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



#include "timer.h"
#include "button.h"
#include "pins.h"


boolean Timer_blinkingLed = false;
boolean Timer_udpBroadCast = false; // every 5 seconds a broadcast is transmitted in wifi-configure-mode. 
volatile uint16_t Timer_timeOutCounter = 0; // is decremented every second until zero. 
volatile uint16_t Timer_timeOutCounterTCP = 0; // is decremented every second until zero. 


/* Arduino 101: timer and interrupts
   1: Timer1 compare match interrupt example 
   more infos: http://www.letmakerobots.com/node/28278
   created by RobotFreak 
*/

void Timer_init_T1()
{
  // initialize timer1 
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  OCR1A = 8000000L/256/1;            // compare match register 8MHz/256 divider/1Hz
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  interrupts();             // enable all interrupts
}

// timer compare interrupt service routine
ISR(TIMER1_COMPA_vect)          
{
	if(Button_isPressed()){
		Button_PressedDuration++;
		Serial.print(Button_PressedDuration);
	}
	
	// Blinking the LED:
	if(Timer_blinkingLed){ // write to this pin only if blinking is active, otherwise
		// the main program controls the LED. 
		digitalWrite(VccEnableN_Analog_Pin, digitalRead(VccEnableN_Analog_Pin)^1); // toggle
	}
	


	// decrement it until 0
	if(Timer_timeOutCounter){
		Timer_timeOutCounter--;
	}
	if(Timer_timeOutCounterTCP){
		Timer_timeOutCounterTCP--;
	}
}
