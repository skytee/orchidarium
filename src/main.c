/* vim: set ts=4:
 *
 *  File:		main.c
 *
 *	Desc:		Reads temperature, humidity and illuminance from i2c-sensors
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
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <setterm.h>
#include <tsl2561.h>

#define _POSIX_SOURCE 1

#define MAXDATASIZE 128
#define BAUDRATE B19200
#define HYT221_SLAVE_ADDR 0x28
#define TSL2561_SLAVE_ADDR 0x39
#define USBI2CDEVICE "/dev/ttyUSB0"

static void die_errno(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, ": %s\n", strerror(errno));
	exit(EXIT_FAILURE);
}

static void die(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}


// read/write single byte for non-registered devices
// returns 0 on success, -1 on failure
int usb_i2c_write_byte(int fd, unsigned char addr, unsigned char value)
{
	ssize_t sz;
	unsigned char data[3];

	fd_set set;
	struct timeval timeout;

	data[0] = 0x53;
	data[1] = addr << 1;
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

	// wait for sensor to prepare data
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

int main(int argc, char *argv[])
{
	int	fd, ret, c, verbosity = 0, errflag = 0, try, success;
	char port[MAXDATASIZE] = USBI2CDEVICE;
	struct termios oldtio;
	speed_t baudrate = BAUDRATE;
	unsigned int ch0, ch1;
	unsigned char data[5];
	float humidity, temp, lux;
	humidity = temp = lux = 0.0;

	while(( c = getopt( argc, argv, "p:v" )) != -1 )
	{
		switch( c ) {
			case 'p':
				strncpy(port, optarg, MAXDATASIZE);
				port[MAXDATASIZE-1] = '\0';
				break;
			case 'v':
				verbosity++;
				break;
			case '?': ++errflag;
				break;
		}
	}

	if ( errflag || optind < argc ) {
		fprintf (stderr,"Usage:\n%s [options]\n"
				"\t-p <port>   use this device, default: " USBI2CDEVICE "\n"
				"\t-v          increase verbosity level\n"
				, *argv);
		exit (2);
	}
	

	fd = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd < 0)
		die_errno("opening port %s", port);

	// save port settings
	tcgetattr(fd, &oldtio);
	
	// port setup
	ret = setterm(fd, baudrate);
	if (ret != 0) {
		tcsetattr(fd,TCSANOW,&oldtio);
		if (ret < 0)
			die_errno("setterm(), maybe the device is not available?");
		if (ret > 0) 
			die("setterm(), maybe the device is not available?");
	}

	/*
	// wake sensor TSL2561 from sleep: write 0x3 to control register once!
	for (success = 0, try = 0; try < 3; try++) {
		if (usb_i2c_write_byte(fd, TSL2561_SLAVE_ADDR, 0x3) < 0) {
			if (verbosity)
				fprintf(stderr, "usb_i2c_write_byte()\n");
			continue;
		}
		++success;
		usleep(400000); // wait for sensor to ready data
		break;
	}
	if (!success)
		die("Cannot read TSL2561");
	}
	*/

	// read raw values from TSL2561 and compute illuminance
	for (success = 0, try = 0; try < 3; try++) {
		/* read channel 0: sensitive to both visible and infrared light */
		// ask i2c-usb: write single command byte 0xac to TSL2561_SLAVE_ADDR
		if (usb_i2c_write_byte(fd, TSL2561_SLAVE_ADDR, 0xac) < 0) {
			if (verbosity)
				fprintf(stderr, "usb_i2c_write_byte()\n");
			continue;
		}
	
		// i2c-usb: read multiple bytes from TSL2561_SLAVE_ADDR
		if (usb_i2c_read_byte_data(fd, TSL2561_SLAVE_ADDR, data, 2, 40000) < 2) {
			if (verbosity)
				fprintf(stderr, "usb_i2c_read_byte_data()\n");
			continue;
		}
		ch0 = 256 * data[1] + data[0];
	
		/* read channel 1: sensitive primarily to infrared light */
		if (usb_i2c_write_byte(fd, TSL2561_SLAVE_ADDR, 0xae) < 0) {
			if (verbosity)
				fprintf(stderr, "usb_i2c_write_byte()\n");
			continue;
		}
	
		if (usb_i2c_read_byte_data(fd, TSL2561_SLAVE_ADDR, data, 2, 40000) < 2) {
			if (verbosity)
				fprintf(stderr, "usb_i2c_read_byte_data()\n");
			continue;
		}
		ch1 = 256 * data[1] + data[0];
	
		printf("%d\n%d\n%d\n", ch0, ch1, calculate_lux(ch0, ch1));
		++success;
		break;
	}
	if (!success)
		die("Cannot read TSL2561");

	// read temperature and humidity from HYT221
	for (success = 0, try = 0; try < 3; try++) {
		// i2c-usb: write single, empty command byte to HYT221_SLAVE_ADDR
		if (usb_i2c_write_byte(fd, HYT221_SLAVE_ADDR, 0x0) < 0) {
			if (verbosity)
				fprintf(stderr, "usb_i2c_write_byte()\n");
			continue;
		}
	
		// i2c-usb: read multiple bytes from HYT221_SLAVE_ADDR
		if (usb_i2c_read_byte_data(fd, HYT221_SLAVE_ADDR, data, 4, 65000) < 4) {
			if (verbosity)
				fprintf(stderr, "usb_i2c_read_byte_data()\n");
			continue;
		}
			
		// plausibility check
		if ((data[0] == 0xff) && (data[1] == 0xff) && (data[2] == 0xff) && (data[3] == 0xff))
			continue;

		humidity = ((data[0] & 0x3f) << 8 | data[1]) * (100.0 / 0x3fff);
		temp = (data[2] << 8 | (data[3] & 0xfc)) * (165.0 / 0xfffc) - 40;
		printf("%.1f\n%.1f\n", humidity, temp);
		++success;
		break;
	}
	if (!success)
		die("Cannot read HYT221");

	// restore saved terminal settings, close fd
	tcsetattr(fd, TCSANOW, &oldtio);
	close(fd);
	return 0;
}
