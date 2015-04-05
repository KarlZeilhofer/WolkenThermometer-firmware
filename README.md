WolkenThermometer-firmware

this is the first release of the firmware for the WolkenThermometer project. 
Please refer to the project website http://www.zeilhofer.co.at/wolke

Features already implemented:
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

Features TODO:
- implement hashing of the transmitted data-upload package (in combination with the server script)
- Finish and test configuration via WiFi (AES256 encrypted and SHA1 hashed)
- battery state dependent transmit intervals (if running on solar cell)
- LED blinking as described on the project page
- reduce all the delays to optimize for power consumption!
