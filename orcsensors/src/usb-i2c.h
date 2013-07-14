/* vim: set ts=4:
 *
 *  File:		usb-i2c.h
 *
 *	Desc:		Functions to access i2c slave devices
 *				attached to the host via a USB-i2c interface.
 *
 *  Doc:		USB-i2c: http://www.robot-electronics.co.uk/htm/usb_i2c_tech.htm
 *
 *  Author:		Thorsten Haas, dev@thorstenhaas.de
 *
 *  History:	n/a
 *
 *  License:	This program is free software; you can redistribute it and/or modify
 *				it under the terms of the GNU General Public License as published by
 *				the Free Software Foundation; either version 2 of the License, or
 *				(at your option) any later version.
*/
#ifndef _USB_I2C_H
#define _USB_I2C_H

struct usb_i2c_data {
    unsigned char reg;
	unsigned char *buf;
	short len;
};

// write single byte for non-registered devices
// returns 0 on success, -1 on failure
int usb_i2c_write_byte(int fd, unsigned char addr, unsigned char value);

// read multiple bytes from slave device
// returns 0 on success, -1 on failure
// values: pointer to data structure that's filled with data from slave
// size: how many bytes to read
// usecs: time to sleep between issuing write and subsequent read command to usb-i2c
int usb_i2c_read_byte_data(int fd, unsigned char addr, unsigned char* values, ssize_t size, int usecs);

// read data from i2c devices, i.e. from a 1 byte internal address register
// returns 0 on success, -1 on failure
int usb_i2c_read_reg_data(int fd, unsigned char addr, struct usb_i2c_data *data);

// write a payload to i2c devices with a 1 byte internal address register
int usb_i2c_write_reg_data(int fd, unsigned char addr, struct usb_i2c_data *data);

#endif
