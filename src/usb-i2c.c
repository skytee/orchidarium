/* vim: set ts=4:
 *
 *  File:		usb-i2c.c
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
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <stdlib.h>
#include <termios.h>
#include <usb-i2c.h>

// read/write single byte for non-registered devices
// returns 0 on success, -1 on failure
int usb_i2c_write_byte(int fd, unsigned char addr, unsigned char value)
{
	ssize_t sz;
	unsigned char data[3];

	fd_set set;
	struct timeval timeout;

	data[0] = 0x53;
	data[1] = (addr << 1);
	data[2] = value;

	sz = (write(fd, data, 3 * sizeof(unsigned char)));
	if (sz < 3)
		return -1;

	tcdrain(fd);

	// read status byte from i2c-usb
	timeout.tv_usec=500000;
	timeout.tv_sec=0;
	FD_ZERO(&set);
	FD_SET(fd, &set);
	if ( select((FD_SETSIZE), &set, NULL, NULL, &timeout) == 0 )
		return -1;

	sz = read(fd, data, 1);
	if (sz < 1)
		return -1;

	// i2c-usb: 0x0 = fail, else succes
	if (data[0] == 0)
		return -1;

	return 0;
}


// read multiple bytes from slave device
// returns 0 on success, -1 on failure
// values: pointer to data structure that's filled with data from slave
// size: how many bytes to read
// usecs: time to sleep between issuing write and subsequent read command to usb-i2c
int usb_i2c_read_byte_data(int fd, unsigned char addr, unsigned char* values, ssize_t size, int usecs)
{
	ssize_t sz;
	unsigned char data[3];

	fd_set set;
	struct timeval timeout;

	data[0] = 0x54;
	data[1] = (addr << 1) | 1;
	data[2] = size;

	// send read command to i2c-usb
	sz = (write(fd, data, 3 * sizeof(unsigned char)));
	if (sz < 3)
		return -1;

	tcdrain(fd);

	// wait for slave device to prepare data
	usleep(usecs);

	timeout.tv_usec=500000;
	timeout.tv_sec=0;
	FD_SET(fd, &set);
	if ( select((FD_SETSIZE), &set, NULL, NULL, &timeout) == 0 )
		return -1;

	sz = read(fd, data, size);
	if (sz < size)
		return -1;

	memcpy(values, data, sz);	

	return sz;
}

// read data from i2c devices, i.e. from a 1 byte internal address register
// returns 0 on success, -1 on failure
int usb_i2c_read_reg_data(int fd, unsigned char addr, struct usb_i2c_data *data)
{
	ssize_t sz;
	size_t msg_size = 4;
	unsigned char msg[msg_size];

	fd_set set;
	struct timeval timeout;

	bzero(msg, msg_size);

	msg[0] = 0x55;				// usb-i2c command
	msg[1] = (addr << 1) | 1;	// device address on i2c bus
	msg[2] = data->reg;			// device internal register
	msg[3] = data->len;			// length of data to read

	sz = write(fd, msg, msg_size);
	if (sz < msg_size)
		return -1;

	tcdrain(fd);

	// read reply from i2c-usb, data->len bytes are expected
	bzero(data->buf, data->len);
	timeout.tv_usec=500000;
	timeout.tv_sec=0;
	FD_ZERO(&set);
	FD_SET(fd, &set);
	if ( select((FD_SETSIZE), &set, NULL, NULL, &timeout) == 0 )
		return -1;

	sz = read(fd, data->buf, data->len);
	if (sz < data->len)
		return -1;

	return 0;
}


// write a payload to i2c devices with a 1 byte internal address register
// returns 0 on success, -1 on failure
int usb_i2c_write_reg_data(int fd, unsigned char addr, struct usb_i2c_data *data)
{
	ssize_t sz;
	unsigned char *msg;
	size_t msg_size = 4 + data->len; // plus 4 bytes usb-i2c command

	fd_set set;
	struct timeval timeout;

	msg = malloc(msg_size);
	if (msg == NULL)
		return -1;

	bzero(msg, msg_size);

	msg[0] = 0x55;			// usb-i2c command
	msg[1] = (addr << 1);	// device address on i2c bus
	msg[2] = data->reg;		// device internal register
	msg[3] = data->len;		// length of data to send 

	memcpy((msg+4), data->buf, data->len);

	sz = write(fd, msg, msg_size);
	if (sz < msg_size)
		return -1;

	tcdrain(fd);

	// read status byte from i2c-usb
	timeout.tv_usec=500000;
	timeout.tv_sec=0;
	FD_ZERO(&set);
	FD_SET(fd, &set);
	if ( select((FD_SETSIZE), &set, NULL, NULL, &timeout) == 0 )
		return -1;

	sz = read(fd, msg, 1);
	if (sz < 1)
		return -1;

	// i2c-usb: 0x0 = fail, else succes
	if (msg[0] == 0)
		return -1;

	return 0;
}
