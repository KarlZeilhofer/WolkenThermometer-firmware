// wt-wifi.h


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



#ifndef __WT_WIFI_H
#define __WT_WIFI_H


#define UPLOAD_HOST (PSTR("me.zeilhofer.co.at"))
#define UPLOAD_PORT (80U)
#define UPLOAD_PATH (PSTR("/wt-test/dataupload.php"))

// return true on success
// false on time-out
bool WifiConfig_run();


// connect to the configured WiFi network, and transmit the measurement data
bool Wifi_connectToAccessPoint();
uint16_t Wifi_sendData(); // TODO

#endif
