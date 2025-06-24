/*
* Project Name: SSD1305_OLED_RPI
* File: SSD1305_OLED_font.c
* Description: font file , full ASCII
* Author: Gavin Lyons.
* URL: https://github.com/gavinlyonsrepo/SSD1305_OLED_RPI
*/

#ifndef _SSD1305_OLED_icon_H
#define _SSD1305_OLED_icon_H

#include <stdint.h>

struct icon_t{
	unsigned char rowSize;
	//unsigned char columnSize;
	unsigned char map[]; 
};

// Standard ASCII 5x8 font , Column padding added by software
static const icon_t Ble_icon = {
	7,
	{
		//0x42, 0x24, 0x18, 0xFF, 0x99, 0x5A, 0x24,  //thick
		0x22, 0x14, 0x08, 0x7F, 0x49, 0x2A, 0x14,  //thin
	},
};

static const icon_t Battery_empty_icon = {
	11,
	{
		//0xFF, 0x81, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0x81, 0xFF, 0x00, 0x7E, //think
		0x7F, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x7F, 0x00, 0x1C, //thin
	},
};

static const icon_t Battery_half_less_icon = {
	11,
	{
		//0xFF, 0x81, 0xBD, 0xBD, 0x81, 0x81, 0x81, 0x81, 0xFF, 0x00, 0x7E, //think
		0x7F, 0x41, 0x5D, 0x5D, 0x41, 0x41, 0x41, 0x41, 0x7F, 0x00, 0x1C, //thin
	},
};

static const icon_t Battery_half_plus_icon = {
	11,
	{
		//0xFF, 0x81, 0xBD, 0xBD, 0x81, 0x81, 0x81, 0x81, 0xFF, 0x00, 0x7E, //think
		0x7F, 0x41, 0x5D, 0x5D, 0x5D, 0x41, 0x41, 0x41, 0x7F, 0x00, 0x1C, //thin
	},
};

static const icon_t Battery_full_icon = {
	11,
	{
		//0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF, 0x00, 0x7E, //think
		0x7F, 0x41, 0x5D, 0x5D, 0x5D, 0x5D, 0x5D, 0x41, 0x7F, 0x00, 0x1C, //thin
	},
};

static const icon_t Rec_icon = {
	7,
	{
		0x1C, 0x22, 0x49, 0x5D, 0x49, 0x22, 0x1C,
	}
};

static const icon_t Arrow_icon = {
    5, // Width of the arrow (4 columns)
    {
        0b1111111, // Column 1 → 7 pixels
        0b0111110, // Column 2 → 5 pixels
        0b0011100, // Column 3 → 3 pixels
        0b0001000, // Column 5 → 1 pixel (tip)
    },
};

static const icon_t Micron_icon = {
    8, // Width in pixels
    {
        0b11111111, // Column 1
        0b00100000, // Column 2
        0b01000000, // Column 3
        0b00100000, // Column 4
        0b01111111, // Column 5
        0b01000000, // Column 6
        0b10000000, // Column 7
        0b01000000  // Column 8
    }
};

static const icon_t Degree_icon = {
    4, // Width in pixels
    {
        0b0110, // Column 1
        0b1001, // Column 2
        0b1001, // Column 3
        0b0110, // Column 4
    }
};

static const icon_t RevArrow_icon = {
    7, 
    {
        0b00001,
        0b00011, 
        0b00111, 
        0b01111,
		0b00111,
		0b00011,
		0b00001, 
    },
};

#endif
