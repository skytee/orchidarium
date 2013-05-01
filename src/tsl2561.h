/* vim: set ts=4:
 *
 *  File:		tsl2561.h
 *
 *	Desc:		TAOS TSL2561 defines.
 *
 *  Doc:		See data sheet: "TSL2560, TSL2561 LIGHT-TO-DIGITAL CONVERTER"
 *				by Texas Advanced Optoelectronic Solutions Inc
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
#ifndef _TSL2561_H
#define _TSL2561_H

// approximation coefficients, as per data sheet, for T (6-lead TMB) package
#define TSL2561_K1T 0x0040 // 0.125 * 2^RATIO_SCALE
#define TSL2561_B1T 0x01f2 // 0.0304 * 2^LUX_SCALE
#define TSL2561_M1T 0x01be // 0.0272 * 2^LUX_SCALE
#define TSL2561_K2T 0x0080 // 0.250 * 2^RATIO_SCALE
#define TSL2561_B2T 0x0214 // 0.0325 * 2^LUX_SCALE
#define TSL2561_M2T 0x02d1 // 0.0440 * 2^LUX_SCALE
#define TSL2561_K3T 0x00c0 // 0.375 * 2^RATIO_SCALE
#define TSL2561_B3T 0x023f // 0.0351 * 2^LUX_SCALE
#define TSL2561_M3T 0x037b // 0.0544 * 2^LUX_SCALE
#define TSL2561_K4T 0x0100 // 0.50 * 2^RATIO_SCALE
#define TSL2561_B4T 0x0270 // 0.0381 * 2^LUX_SCALE
#define TSL2561_M4T 0x03fe // 0.0624 * 2^LUX_SCALE
#define TSL2561_K5T 0x0138 // 0.61 * 2^RATIO_SCALE
#define TSL2561_B5T 0x016f // 0.0224 * 2^LUX_SCALE
#define TSL2561_M5T 0x01fc // 0.0310 * 2^LUX_SCALE
#define TSL2561_K6T 0x019a // 0.80 * 2^RATIO_SCALE
#define TSL2561_B6T 0x00d2 // 0.0128 * 2^LUX_SCALE
#define TSL2561_M6T 0x00fb // 0.0153 * 2^LUX_SCALE
#define TSL2561_K7T 0x029a // 1.3 * 2^RATIO_SCALE
#define TSL2561_B7T 0x0018 // 0.00146 * 2^LUX_SCALE
#define TSL2561_M7T 0x0012 // 0.00112 * 2^LUX_SCALE
#define TSL2561_K8T 0x029a // 1.3 * 2^RATIO_SCALE
#define TSL2561_B8T 0x0000 // 0.000 * 2^LUX_SCALE
#define TSL2561_M8T 0x0000 // 0.000 * 2^LUX_SCALE
#define TSL2561_LUX_SCALE 14 // scale by 2^14
#define TSL2561_RATIO_SCALE 9 // scale ratio by 2^9

// register defines
#define TSL2561_COMMAND_BIT (0x80)
#define TSL2561_REGISTER_CONTROL (0x00)
#define TSL2561_CONTROL_POWERON (0x03)

// Illuminance approximation function, see data sheet: "TSL2560, TSL2561
// LIGHT-TO-DIGITAL CONVERTER" by Texas Advanced Optoelectronic Solutions Inc
// ch0 - raw value from channel 0 of TSL2561
// ch1 - raw value from channel 1 of TSL2561
// Test:: ch0 = 1735, ch1 = 709; // --> 22 lux
unsigned int calculate_lux(unsigned int ch0, unsigned int ch1);

#endif
