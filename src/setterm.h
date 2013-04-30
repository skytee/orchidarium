/* vim: set ts=4:
 *
 *  File:		setterm.h
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
#ifndef _SETTERM_H
#define _SETTERM_H
#include <string.h>
#include <fcntl.h> 
#include <termios.h>
int setterm(int fd, speed_t baudrate);
#endif
