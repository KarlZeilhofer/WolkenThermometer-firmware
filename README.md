# WolkenThermometer-firmware

this is the release of the firmware for the WolkenThermometer project. 
Please refer to the project website http://www.zeilhofer.co.at/wolke

## Download, Setup and Compile
- When you have installed git on your computer, just go into your Arduinio-projects-folder (e.g. ~/Arduino/projects) and run
```
git clone https://github.com/KarlZeilhofer/WolkenThermometer-firmware.git
```
Otherwise you can downlaod the zip-pacakage too:
https://github.com/KarlZeilhofer/WolkenThermometer-firmware/archive/master.zip

- Then you have to copy or symlink the user libraries (aes256, sha1 and OneWire) from ```/WolkenThermometer-firmware/lib``` to your Arduino user libs folder (this is system dependent, but on linux it is ```~/Arduino/libraries```. 

- Then open the file ```/WolkenThermometer-firmware/src/sketch/sketch.ino``` in your Arduino IDE (tested with version 1.6.1)
- Set the correct board: **Arduino Pro or Pro Mini**
- Set the correct processor: **ATmega328 (3.3V, 8MHz)**
- Now you are ready for compile (verify) and upload

## Features already implemented:
- settings dialog for setting ID, secret key, public key and if you want to SSID and password
- measurement of temperature and battery voltage
- different reset button handlers
  - press without hold: reset the software
  - press with 1-3s hold: reset and activate WiFi access point for configuration with android app
  - press with 5-10s hold: reset and activate serial settings dialog
- software serial for debug outputs
- store settings in EEPROM
- system timer with 1s period for flashing LED and time out counters
- power down with watchdog timer for saving power

## Features TODO:
- implement hashing of the transmitted data-upload package (in combination with the server script)
- Finish and test configuration via WiFi (AES256 encrypted and SHA1 hashed)
- battery state dependent transmit intervals (if running on solar cell)
- LED blinking as described on the project page
- reduce all the delays to optimize for power consumption!
