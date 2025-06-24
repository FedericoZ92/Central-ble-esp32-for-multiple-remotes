/*
* Project Name: SSD1305_OLED_RPI
* Author: Gavin Lyons.
* URL: https://github.com/gavinlyonsrepo/SSD1305_OLED_RPI
*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "screen-driver/SSD1305_OLED.h"
#include <stdbool.h>

#include "debug.h"


SSD1305 ::SSD1305(int16_t oledWidth, int16_t oledHeight) : 
	SSD1305_graphics(oledWidth, oledHeight)
{
	OLEDheight = oledHeight;
	OLEDwidth = oledWidth;
	OLEDpageNumber = (OLEDheight / 8);
	bufferWidth = oledWidth;
	bufferHeight = oledHeight;
	buffer = (uint8_t*) &screenBuffer;
	xOledMutex = xSemaphoreCreateMutex();
}

SSD1305::~SSD1305(){}

int SSD1305::getHeight()
{
    return OLEDheight;
}

int SSD1305::getLength()
{
    return OLEDwidth;
}

// Call when powering down
void SSD1305::OLEDPowerDown(void)
{
	OLEDEnable(0);
	initializedByCommands = false;
	vTaskDelay(100 / portTICK_PERIOD_MS); // bcm2835_delay(100);
}

// Desc: Called from OLEDbegin carries out Power on sequence and register init
void SSD1305::OLEDinit()
{
	vTaskDelay(pdMS_TO_TICKS(SSD1305_INITDELAY));

	ESP_LOGI(I2C_DISPLAY_TAG, "Inside OLEDinit");

	SSD1305_command( SSD1305_DISPLAY_OFF);
	SSD1305_command( SSD1305_MEMORY_ADDR_MODE );
	SSD1305_command(0x00);  //Horizontal Addressing Mode is Used
	SSD1305_command(SSD1305_SET_START_LINE); // set start line to 0
	SSD1305_command(SSD1305_SET_SEGMENT_REMAP); // 0xA1 column address 131 is mapped to SEG0 - 0xA0 column address 0 is mapped to SEG0
	SSD1305_command(SSD1305_SET_MULTIPLEX_RATIO); //Set MUX ratio to N+1 MUX
	SSD1305_command(0x1F); // N=31 --> 32MUX : from 16MUX to 64MUX
	SSD1305_command(SSD1305_COM_SCAN_DIR_INC); //Set COM Output	Scan Direction - remapped mode. Scan from COM[MUX ratio-1] to COM0
	SSD1305_command(SSD1305_SET_DISPLAY_OFFSET); // Set vertical shift by COM from 0~63 (31).
	SSD1305_command(0x00); // vertical shift = 0
	SSD1305_command(SSD1305_SET_DISPLAY_CLOCK_DIV_RATIO); //Set Display Clock Divide Ratio/Oscillator Frequency
	SSD1305_command(0xF0); // Divide Ratio = 0 - Oscillator Frequency = 1111b
	SSD1305_command(SSD1305_SET_AREA_COLOR_MODE); // Set Area Color Mode ON/OFF & Low Power Display Mode
	SSD1305_command(0x05); //X[5:4]= 00b (RESET) : monochrome mode; X[2]=1b and X[0]=1b: Set low power display mode
	SSD1305_command(SSD1305_SET_COM_PINS);
	SSD1305_command(0x12); //A[4]=1b--> Alternative COM pin configuration; A[5]=0b--> Disable COM Left/Right remap
	SSD1305_command(SSD1305_SET_CONTRAST_CONTROL);
	switch (getHeight()){
	case 64: // NOTE: not tested, lacking part. most likely not gonna work
		SSD1305_command(0xCF);
		break;
	case 32:
		SSD1305_command(0x8F);
		break;
	case 16: // NOTE: not tested, lacking part. most likely not gonna work
		SSD1305_command(0xAF);
		break;
	}
	SSD1305_command(SSD1305_SET_PRECHARGE_PERIOD);
	SSD1305_command(0xF1);
	SSD1305_command(SSD1305_SET_VCOM_DESELECT);
	SSD1305_command(0x40);
	SSD1305_command(SSD1305_DISPLAY_ALL_ON_RESUME);
	SSD1305_command(SSD1305_NORMAL_DISPLAY);
	SSD1305_command(SSD1305_DEACTIVATE_SCROLL);
	OLEDclearBuffer();
	OLEDupdate();
	SSD1305_command(SSD1305_DISPLAY_ON);

	//OLEDclearBuffer();
	//OLEDupdate();

	initializedByCommands = true;
	vTaskDelay(SSD1305_INITDELAY / portTICK_PERIOD_MS); 

	//OLEDclearBuffer();
    //OLEDupdate(); 
}

// Desc: Turns On Display
// Param1: bits, 1 on , 0 off
void SSD1305::OLEDEnable(uint8_t bits)
{
	bits ? SSD1305_command(SSD1305_DISPLAY_ON) : SSD1305_command(SSD1305_DISPLAY_OFF);
}

// Desc: Adjusts contrast
// Param1: Contrast 0x00 to 0xFF , default 0x80
void SSD1305::OLEDContrast(uint8_t contrast)
{
	SSD1305_command(SSD1305_SET_CONTRAST_CONTROL);
	SSD1305_command(contrast);
}

// Desc: invert the display
// Param1: true invert , false normal
void SSD1305::OLEDInvert(bool value)
{
	value ? SSD1305_command(SSD1305_INVERT_DISPLAY) : SSD1305_command(SSD1305_NORMAL_DISPLAY);
}

// Desc: Fill the screen NOT the buffer with a datapattern
// Param1: datapattern can be set to zero to clear screen (not buffer) range 0x00 to 0ff
// Param2: optional delay in milliseconds can be set to zero normally.
void SSD1305::OLEDFillScreen(uint8_t dataPattern, uint8_t msDelay)
{
	for (uint8_t row = 0; row < OLEDpageNumber; row++){
		SSD1305_command(0xB0 | row);
		SSD1305_command(SSD1305_SET_LOWER_COLUMN);
		SSD1305_command(SSD1305_SET_HIGHER_COLUMN);
		for (uint8_t col = 0; col < OLEDwidth; col++){
			SSD1305_data(dataPattern);
			vTaskDelay(msDelay / portTICK_PERIOD_MS); //bcm2835_delay(delay);
		}
	}
}

// Desc: Fill the chosen page(1-8)  with a datapattern
// Param1: datapattern can be set to 0 to FF (not buffer)
// Param2: optional delay in milliseconds can be set to zero normally.
void SSD1305::OLEDFillPage(uint8_t page_num, uint8_t dataPattern, uint8_t msDelay)
{
	uint8_t Result = 0xB0 | page_num;
	SSD1305_command(Result);
	SSD1305_command(SSD1305_SET_LOWER_COLUMN);
	SSD1305_command(SSD1305_SET_HIGHER_COLUMN);
	uint8_t numofbytes = OLEDwidth;
	for (uint8_t i = 0; i < numofbytes; i++){
		SSD1305_data(dataPattern);
		vTaskDelay(msDelay / portTICK_PERIOD_MS); //bcm2835_delay(msDelay);
	}
}

//Desc: Draw a bitmap  to the screen
//Param1: x offset 0-128
//Param2: y offset 0-64
//Param3: width 0-128
//Param4 height 0-64
//Param5: pointer to bitmap data
//Param6: Invert color
void SSD1305::OLEDBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *data, bool invert)
{
	int16_t byteWidth = (w + 7) / 8;
	uint8_t byte = 0;
	uint8_t color, bgcolor;
	if (invert == false){
		color = WHITE;
		bgcolor = BLACK;
	}else{
		color = BLACK;
		bgcolor = WHITE;
	}

	for (int16_t j = 0; j < h; j++, y++){
		for (int16_t i = 0; i < w; i++){
			if (i & 7){
				byte <<= 1;
			}else{
				byte = data[j * byteWidth + i / 8];
			}
			drawPixel(x + i, y, (byte & 0x80) ? color : bgcolor);
		}
	}
}

//Desc: updates the buffer i.e. writes it to the screen
void SSD1305::OLEDupdate() //write to active buffer
{
	uint8_t x = 0;
	uint8_t y = 0;
	uint8_t w = this->bufferWidth;
	uint8_t h = this->bufferHeight;
	OLEDBuffer(x, y, w, h, (uint8_t *)this->buffer);
}

//Desc: clears the buffer memory i.e. does NOT write to the screen
void SSD1305::OLEDclearBuffer()
{
	memset(this->buffer, 0x00, (this->bufferWidth * (this->bufferHeight / 8)));
}

//Desc: Draw a bitmap to the screen
//Param1: x offset 0-128
//Param2: y offset 0-64
//Param3: width 0-128
//Param4 height 0-64
//Param5 the buffer data
//Note: Called by OLEDupdate
void SSD1305::OLEDBuffer(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t *data)
{
    if (xSemaphoreTake(xOledMutex, portMAX_DELAY) == pdTRUE){
		SSD1305_command(SSD1305_SET_COLUMN_ADDR);
		/*SSD1305 132*64 - SSD1309 128*64*/
		SSD1305_command(0);				  // Column start address (0 = reset)
		SSD1305_command(0x7F); 			  // Column end address (SSD1305 131 = reset - SSD1309 127 = reset)

		SSD1305_command(SSD1305_SET_PAGE_ADDR);
		SSD1305_command(0); // Page start address (0 = reset)

		switch (OLEDheight){
		case 64:
			SSD1305_command(7);
			break;
		case 32:
			SSD1305_command(3);
			break;
		case 16:
			SSD1305_command(1);
			break;
		}

		//For buffer 256x64. 
		for (int i = 0; i < 8; ++i) {
			SSD1305_row_data(&data[i * 64], 64);
		}

		xSemaphoreGive(xOledMutex);
	}
}

// Desc: Draws a Pixel to the screen overrides the gfx lib if defined
// Passed x and y co-ords and colour of pixel.
void SSD1305::drawPixel(int16_t x, int16_t y, uint8_t color)
{
	if ((x < 0) || (x >= this->bufferWidth) || (y < 0) || (y >= this->bufferHeight)){
		return;
	}
	int16_t temp;
	switch (rotation){
	case 1:
		temp = x;
		x = WIDTH - 1 - y;
		y = temp;
		break;
	case 2:
		x = WIDTH - 1 - x;
		y = HEIGHT - 1 - y;
		break;
	case 3:
		temp = x;
		x = y;
		y = HEIGHT - 1 - temp;
		break;
	}
	uint16_t tc = (bufferWidth * (y / 8)) + x;
	switch (color){
	case WHITE:
		this->buffer[tc] |= (1 << (y & 7));
		break;
	case BLACK:
		this->buffer[tc] &= ~(1 << (y & 7));
		break;
	case INVERSE:
		this->buffer[tc] ^= (1 << (y & 7));
		break;
	}
}

void SSD1305::OLED_StartScrollRight(uint8_t start, uint8_t stop)
{
	SSD1305_command(SSD1305_RIGHT_HORIZONTAL_SCROLL);
	SSD1305_command(0X00);
	SSD1305_command(start); // start page
	SSD1305_command(0X00);
	SSD1305_command(stop); // end page
	SSD1305_command(0X00);
	SSD1305_command(0XFF);
	SSD1305_command(SSD1305_ACTIVATE_SCROLL);
}

void SSD1305::OLED_StartScrollLeft(uint8_t start, uint8_t stop)
{
	SSD1305_command(SSD1305_LEFT_HORIZONTAL_SCROLL);
	SSD1305_command(0X00);
	SSD1305_command(start);
	SSD1305_command(0X00);
	SSD1305_command(stop);
	SSD1305_command(0X00);
	SSD1305_command(0XFF);
	SSD1305_command(SSD1305_ACTIVATE_SCROLL);
}

void SSD1305::OLED_StartScrollDiagRight(uint8_t start, uint8_t stop)
{
	SSD1305_command(SSD1305_SET_VERTICAL_SCROLL_AREA);
	SSD1305_command(0X00);
	SSD1305_command(OLEDheight);
	SSD1305_command(SSD1305_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
	SSD1305_command(0X00);
	SSD1305_command(start);
	SSD1305_command(0X00);
	SSD1305_command(stop);
	SSD1305_command(0X01);
	SSD1305_command(SSD1305_ACTIVATE_SCROLL);
}

void SSD1305::OLED_StartScrollDiagLeft(uint8_t start, uint8_t stop)
{
	SSD1305_command(SSD1305_SET_VERTICAL_SCROLL_AREA);
	SSD1305_command(0X00);
	SSD1305_command(OLEDheight);
	SSD1305_command(SSD1305_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
	SSD1305_command(0X00);
	SSD1305_command(start);
	SSD1305_command(0X00);
	SSD1305_command(stop);
	SSD1305_command(0X01);
	SSD1305_command(SSD1305_ACTIVATE_SCROLL);
}

void SSD1305::OLED_StopScroll(void)
{
	SSD1305_command(SSD1305_DEACTIVATE_SCROLL);
}

bool SSD1305::isInitialized() const
{
	return initializedByCommands;
}


