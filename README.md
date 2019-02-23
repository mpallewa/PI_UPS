# PI_UPS
A UPS for the Raspberry PI or similar using the Adafruit Powerboost 1000c and the Trinket M0. Could control any other UPS device with an enable pin as well.

Please see the schematic for the connection details. The M0 needs to be connecyed to the PI via I2C and uses thw address 0x20. If this is changed it should be changed in the python scripts that run on the PI as well. The UPS monitors that status of the PI by monitoring the read requests from the PI. Only the I2C pins of the PI are used. 


src/powerboost_1000c_trinket_m0.ino - Arduino sketch

doc/UPS_schem.png 	                - Schematic

util/ups.py 	                      - UPS monitoring python code to run on the PI (will run as a daemon)

util/rv_ar1v0-cal.py                - Python utility to fetch the battery voltage and the status. 

doc/VBAT_VEN.png                    - Battery voltage and the voltage at the enable pin
