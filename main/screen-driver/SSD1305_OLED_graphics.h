/*
* Project Name: SSD1305_OLED_RPI
* File: SSD1305_OLED_graphics.h
* Description: header file for the custom graphics functions
* Author: Gavin Lyons.
* URL: https://github.com/gavinlyonsrepo/SSD1305_OLED_RPI
*/

#ifndef _SSD1305_OLED_GRAPHICS_H
#define _SSD1305_OLED_GRAPHICS_H

#include "screen-driver/SSD1305_OLED_Print.h"
#include "screen-driver/SSD1305_OLED_font.h"
#include "screen-driver/SSD1305_OLED_icons.h"
#include <stdio.h>
#include <stdint.h>

enum FONT_TYPE{
	FONT_UNDEFINED = 0,
	FONT_DEFAULT_1 = 1,
	FONT_THICK_2 = 2,
	FONT_SEVEN_SEGMENTS_3 = 3,
	FONT_WIDE_4 = 4,
	FONT_BIG_NUMS_5 = 5,
	FONT_TYPE_6 = 6,
	FONT_TYPE_SEITRON_7 = 7,
};

#define OLEDswap(a, b) { int16_t t = a; a = b; b = t; }
// Fonts setup
#define SSD1305_ASCII_OFFSET 0x00
#define SSD1305_ASCII_OFFSET_SP 0x20 // Starts at Space
#define SSD1305_ASCII_OFFSET_NUM  0x30 // Starts at number 0

class SSD1305_graphics : public Print{

public:
    SSD1305_graphics(int16_t w, int16_t h); // Constructor

    // This is defined by the subclass:
    virtual void drawPixel(int16_t x, int16_t y, uint8_t color) = 0;
    virtual void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color);
	virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint8_t color);
	virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint8_t color);
	virtual void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color);
	virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color);
	virtual void fillScreen(uint8_t color);
	virtual void fillScreenBlack();
	virtual void fillScreenWhite();

    // no subclass overrides
    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint8_t color);
	void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint8_t color);
	void fillCircle(int16_t x0, int16_t y0, int16_t r, uint8_t color);
	void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint8_t color);
	void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);
	void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);
	void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint8_t color);
	void fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint8_t color);
	bool drawChar(int16_t x, int16_t y, unsigned char c, uint8_t color, uint8_t bg, uint8_t size);
	void setCursor(int16_t x, int16_t y);
	void setTextColor(uint8_t c);
	void setTextColor(uint8_t c, uint8_t bg);
	void setTextSize(uint8_t s);
	void setTextWrap(bool w);
	void setFontType(FONT_TYPE fontType);
	void setRotation(uint8_t r);
	
    virtual size_t vwrite(uint8_t);

	void drawCharNumFont(uint8_t x, uint8_t y, uint8_t c, uint8_t color ,uint8_t bg);
	void drawTextNumFont(uint8_t x, uint8_t y, char *pText, uint8_t color, uint8_t bg);
	void drawIcon(uint8_t x, uint8_t y, const icon_t *pIcon, uint8_t color, uint8_t bg);
	int16_t height(void) const;
	int16_t width(void) const;
	uint8_t getRotation(void) const;

protected:
    const int16_t WIDTH, HEIGHT;   // This is the 'raw' display w/h - never changes
    int16_t _width, _height; // Display w/h as modified by current rotation
	int16_t cursor_x, cursor_y;
	uint8_t textcolor, textbgcolor;
	uint8_t textsize;
	uint8_t rotation;
	bool wrap; // If set, 'wrap' text at right edge of display
	FONT_TYPE _FontType = FONT_TYPE_SEITRON_7;//DEFAULT_1;
	uint8_t _CurrentFontWidth = 5;
	uint8_t _CurrentFontoffset = 0;
	uint8_t _CurrentFontheight = 8;
};

#endif 
