/* vim: set ts=4:
 *
 *  File:		setterm.c
 *
 *	Desc:		Functions to set a serial interface to sane values.
 *
 *  Doc:		man 3 termios
 *
 *  Author:		Thorsten haas, dev@thorstenhaas.de
 *
 *  History:	n/a
 *
 *  License:	This program is free software; you can redistribute it and/or modify
 *				it under the terms of the GNU General Public License as published by
 *				the Free Software Foundation; either version 2 of the License, or
 *				(at your option) any later version.
*/
#include <string.h>
#include <fcntl.h> 
#include <setterm.h>

// returns 0 on success, -1 on failure (errno is set), >0 on failure (errno not set)
int setterm(int fd, speed_t baudrate)
{
	struct termios newtio, cmptio, *tio = &newtio;

	// zero struct and set prefs	
	bzero(tio, sizeof(*tio));
	
	// terminal hardware control (c_cflag)
	// CS8 / CS5-	Specify character size to 8bit/5bit
	// CLOCAL	- 	Ignore modem control lines
	tio->c_cflag = baudrate | CS8 | CLOCAL | CREAD;

	// !CRTSCTS - 	Do not enable hardware flow control using RTS and CTS 
	tio->c_cflag &= ~CRTSCTS;

	// basic terminal input control (c_iflag)
	// !IGNBRK -	Do not ignore the break condition
	// !IGNCR - 	Do not ignore CR chars
	// !INLCR -	Do not map NL to CR
	// !ICRNL -	Do not map CR to NL
	// !IUCLC - 	Do not map uppercase chars to lowercase
	// !IXANY -	Do not enable any input char to restart suspended output
	// !IXON -	Do not enable start and stop output control by START STOP chars
	// !IXOFF -	Do not enable start and stop input control by START STOP chars
	// !INPCK -	Do not enable input parity checking
	// !ISTRIP - 	Do not strip 8bit chars to 7bit
	// !PARMRK -	Do not prefix chars with a parity error or framing errors
	tio->c_iflag &= ~(IGNBRK | IGNCR | INLCR | ICRNL | IUCLC |
			IXANY | IXON | IXOFF | INPCK | ISTRIP | PARMRK);
	// BRKINT -	Enable interrupt signal on break condition
	// IGNPAR -	Ignore parity errors and other framing errors
	tio->c_iflag |= (BRKINT | IGNPAR);
						
	// system output treatment (c_oflag) 
	// !OPOST -	Do not enable post processing of chars
	tio->c_oflag &= ~OPOST;

	// various terminal functions (c_lflag)
	// !XCASE -	Do not enable canonical uppercase and lowercase presentation
	// !ECHONL -	Do not enable line clear on NL. Prevent terminal to hang up on EOF
       	// !NOFLSH -	Do not disable queue flushing
	tio->c_lflag &= ~(XCASE | ECHONL | NOFLSH);
	// !ICANON - 	Do not enable canonical input
	// !ISIG - 	Do not enable signals (Do not check against special control chars)
	tio->c_lflag &= ~(ICANON | ISIG | ECHO);
	
	// enable receiver (c_cflag)
	// CREAD -	Enable receiver
	tio->c_cflag |= CREAD;

	// control characters (c_cc)
	// VTIME=0 and VMIN=1 block read for at least one char
	tio->c_cc[VTIME] = 0;
	tio->c_cc[VMIN] = 1;
	
	// flush all data that is received but not read
	if (tcflush(fd, TCIOFLUSH) == -1)
		return -1;

	// immediately set attributes
	if (tcsetattr(fd, TCSANOW, tio) == -1)
		return -1;

	// reget and compare attributes
	if (tcgetattr(fd, &cmptio) == -1)
		return -1;

	// return 0 when attributes are OK
	return (!memcmp(tio, &cmptio, sizeof(*tio)));
}

