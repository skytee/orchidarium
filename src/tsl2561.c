/* vim: set ts=4:
 *
 *  File:		tsl2561.c
 *
 *	Desc:		TAOS TSL2560 illuminance calculation function.
 *
 *  Doc:		Computation of illuminance as recommended by 
 *				data sheet: "TSL2560, TSL2561 LIGHT-TO-DIGITAL CONVERTER",
 *				Texas Advanced Optoelectronic Solutions Inc
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
#include <tsl2561.h>

// ch0 - raw value from channel 0 of TSL2561
// ch1 - raw value from channel 1 of TSL2561
unsigned int calculate_lux(unsigned int ch0, unsigned int ch1)
{
	unsigned long channel0, channel1, ratio, _ratio;
	unsigned int b, m;

	// Assume gain and integration time are nominal values, i.e. 16x, 402ms
	// WARNING: scaling necessary necessary here if integration time is NOT 402 ms
	channel0 = ch0; // assume no scaling
	channel1 = ch1; // assume no scaling

	// compute and round ratio of raw values
	_ratio = 0;
	if (channel0 != 0)
		_ratio = (channel1 << (RATIO_SCALE+1)) / channel0;
	ratio = (_ratio + 1) >> 1;
	
	// based on coefficients of T package (different for CS package)
	if ((ratio >= 0) && (ratio <= K1T))
		{b=B1T; m=M1T;}
	else if (ratio <= K2T)
		{b=B2T; m=M2T;}
	else if (ratio <= K3T)
		{b=B3T; m=M3T;}
	else if (ratio <= K4T)
		{b=B4T; m=M4T;}
	else if (ratio <= K5T)
		{b=B5T; m=M5T;}
	else if (ratio <= K6T)
		{b=B6T; m=M6T;}
	else if (ratio <= K7T)
		{b=B7T; m=M7T;}
	else if (ratio > K8T)
		{b=B8T; m=M8T;}

	unsigned long temp;
	temp = ((channel0 * b) - (channel1 * m));

	// do not allow negative lux value
	if (temp < 0) temp = 0;

	// round lsb (2^(LUX_SCALE-1))
	temp += (1 << (LUX_SCALE-1));

	// strip off fractional portion
	unsigned long lux = temp >> LUX_SCALE;
	return(lux);
}
