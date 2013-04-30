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
		_ratio = (channel1 << (TSL2561_RATIO_SCALE+1)) / channel0;
	ratio = (_ratio + 1) >> 1;
	
	// based on coefficients of T package (different for CS package)
	if ((ratio >= 0) && (ratio <= TSL2561_K1T))
		{b=TSL2561_B1T; m=TSL2561_M1T;}
	else if (ratio <= TSL2561_K2T)
		{b=TSL2561_B2T; m=TSL2561_M2T;}
	else if (ratio <= TSL2561_K3T)
		{b=TSL2561_B3T; m=TSL2561_M3T;}
	else if (ratio <= TSL2561_K4T)
		{b=TSL2561_B4T; m=TSL2561_M4T;}
	else if (ratio <= TSL2561_K5T)
		{b=TSL2561_B5T; m=TSL2561_M5T;}
	else if (ratio <= TSL2561_K6T)
		{b=TSL2561_B6T; m=TSL2561_M6T;}
	else if (ratio <= TSL2561_K7T)
		{b=TSL2561_B7T; m=TSL2561_M7T;}
	else if (ratio > TSL2561_K8T)
		{b=TSL2561_B8T; m=TSL2561_M8T;}

	unsigned long temp;
	temp = ((channel0 * b) - (channel1 * m));

	// do not allow negative lux value
	if (temp < 0) temp = 0;

	// round lsb (2^(TSL2561_LUX_SCALE-1))
	temp += (1 << (TSL2561_LUX_SCALE-1));

	// strip off fractional portion
	unsigned long lux = temp >> TSL2561_LUX_SCALE;
	return(lux);
}
