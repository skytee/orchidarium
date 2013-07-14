orcsensors
==========

Code to read i2c sensor data via [USB-i2c](http://www.robot-electronics.co.uk/htm/usb_i2c_tech.htm). Seen from the host, this implies a serial connection over USB (FT232RL) to an i2c master.

This code is an implementation of methods
* to set up a serial connection
* to control the i2c master
* to configure and read from the sensors (i2c slaves)
* to compute values from raw sensor data

