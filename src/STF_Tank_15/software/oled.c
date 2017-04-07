/*
 * oled.c
 *
 *  Created on: Mar 28, 2016
 *      Author: haoyifan
 */

#include "drivers/mss_i2c/mss_i2c.h"
#include "drivers/mss_gpio/mss_gpio.h"
#include "oled.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "math.h"


#define TOTALFONTS 1


static uint8_t screenmemory [] = {
	/* LCD Memory organised in 64 horizontal pixel and 6 rows of byte
	 B  B .............B  -----
	 y  y .............y        \
	 t  t .............t         \
	 e  e .............e          \
	 0  1 .............63          \
	                                \
	 D0 D0.............D0            \
	 D1 D1.............D1            / ROW 0
	 D2 D2.............D2           /
	 D3 D3.............D3          /
	 D4 D4.............D4         /
	 D5 D5.............D5        /
	 D6 D6.............D6       /
	 D7 D7.............D7  ----
	*/
	//SparkFun Electronics LOGO

	// ROW0, BYTE0 to BYTE63
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xF8, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0x0F, 0x07, 0x07, 0x06, 0x06, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	// ROW1, BYTE64 to BYTE127
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x81, 0x07, 0x0F, 0x3F, 0x3F, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFC, 0xFC, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFC, 0xF8, 0xE0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	// ROW2, BYTE128 to BYTE191
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC,
	0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF1, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xF0, 0xFD, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	// ROW3, BYTE192 to BYTE255
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x1F, 0x07, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	// ROW4, BYTE256 to BYTE319
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x1F, 0x1F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x0F, 0x0F, 0x0F, 0x0F, 0x07, 0x07, 0x07, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	// ROW5, BYTE320 to BYTE383
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
	0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void oled_init(uint8_t dc_pin)
{
	// default 5x7 font
	setFontType(0);
	setColor(WHITE);
	setDrawMode(NORM);
	setCursor(0,0);

	// Set up the selected interface:
	i2cSetup();
	if (dc_pin == 1)
			i2c_address = I2C_ADDRESS_SA0_1;
	else
			i2c_address = I2C_ADDRESS_SA0_0;


	// Display reset routine
	//MSS_GPIO_init();
	MSS_GPIO_config( MSS_GPIO_15, MSS_GPIO_OUTPUT_MODE);
	MSS_GPIO_set_output
	(
		MSS_GPIO_15,
		0x01
	);
	oled_delay(5);	// VDD (3.3V) goes high at start, lets just chill for 5 ms
	MSS_GPIO_set_output
	(
		MSS_GPIO_15,
		0x00
	);
	oled_delay(10);	// wait 10ms
	MSS_GPIO_set_output
	(
		MSS_GPIO_15,
		0x01
	);

	// Display Init sequence for 64x48 OLED module
	command(DISPLAYOFF);			// 0xAE

	command(SETDISPLAYCLOCKDIV);	// 0xD5
	command(0x80);					// the suggested ratio 0x80

	command(SETMULTIPLEX);			// 0xA8
	command(0x2F);

	command(SETDISPLAYOFFSET);		// 0xD3
	command(0x0);					// no offset

	command(SETSTARTLINE | 0x0);	// line #0

	command(CHARGEPUMP);			// enable charge pump
	command(0x14);

	command(NORMALDISPLAY);			// 0xA6
	command(DISPLAYALLONRESUME);	// 0xA4

	command(SEGREMAP | 0x1);
	command(COMSCANDEC);

	command(SETCOMPINS);			// 0xDA
	command(0x12);

	command(SETCONTRAST);			// 0x81
	command(0x8F);

	command(SETPRECHARGE);			// 0xd9
	command(0xF1);

	command(SETVCOMDESELECT);			// 0xDB
	command(0x40);

	command(DISPLAYON);				//--turn on oled panel
	clear(ALL);						// Erase hardware memory inside the OLED controller to avoid random data in memory.
}

void command(uint8_t c) {

		// Write to our address, make sure it knows we're sending a
		// command:
		i2cWrite(i2c_address, I2C_COMMAND, c);

}

void data(uint8_t c) {

		// Write to our address, make sure it knows we're sending a
		// data byte:
		i2cWrite(i2c_address, I2C_DATA, c);

}

/** \brief Set SSD1306 page address.

    Send page address command and address to the SSD1306 OLED controller.
*/
void setPageAddress(uint8_t add) {
	add=0xb0|add;
	command(add);
	return;
}

/** \brief Set SSD1306 column address.

    Send column address command and address to the SSD1306 OLED controller.
*/
void setColumnAddress(uint8_t add) {
	command((0x10|(add>>4))+0x02);
	command((0x0f&add));
	return;
}

/** \brief Clear screen buffer or SSD1306's memory.

    To clear GDRAM inside the LCD controller, pass in the variable mode = ALL and to clear screen page buffer pass in the variable mode = PAGE.
*/
void clear(uint8_t mode) {
	//	uint8_t page=6, col=0x40;
	int i, j;
	if (mode==ALL) {
		for (i=0;i<8; i++) {
			setPageAddress(i);
			setColumnAddress(0);
			for (j=0; j<0x80; j++) {
				data(0);
			}
		}
	}
	else
	{
		memset(screenmemory,0,384);			// (64 x 48) / 8 = 384
		//display();
	}
}


/** \brief Invert display.

    The WHITE color of the display will turn to BLACK and the BLACK will turn to WHITE.
*/
void invert(uint8_t inv) {
	if (inv == 1)
	command(INVERTDISPLAY);
	else
	command(NORMALDISPLAY);
}

/** \brief Set contrast.

    OLED contract value from 0 to 255. Note: Contrast level is not very obvious.
*/
void contrast(uint8_t contrast) {
	command(SETCONTRAST);			// 0x81
	command(contrast);
}

/** \brief Transfer display memory.

    Bulk move the screen buffer to the SSD1306 controller's memory so that images/graphics drawn on the screen buffer will be displayed on the OLED.
*/
void display(void) {
	uint8_t i, j;

	for (i=0; i<6; i++) {
		setPageAddress(i);
		setColumnAddress(0);
		for (j=0;j<0x40;j++) {
			data(screenmemory[i*0x40+j]);
		}
	}
}

/** \brief Override Arduino's Print.

    Arduino's print overridden so that we can use uView.print().
*/
uint8_t oled_write(uint8_t c) {
	if (c == '\n') {
		cursorY += fontHeight;
		cursorX  = 0;
	} else if (c == '\r') {
		// skip
	} else {
		_drawChar(cursorX, cursorY, c, foreColor, drawMode);
		cursorX += fontWidth+1;
		if ((cursorX > (LCDWIDTH - fontWidth))) {
			cursorY += fontHeight;
			cursorX = 0;
		}
	}

	return 1;
}

/** \brief Set cursor position.

MicroOLED's cursor position to x,y.
*/
void setCursor(uint8_t x, uint8_t y) {
	cursorX=x;
	cursorY=y;
}

/** \brief Draw pixel.

Draw pixel using the current fore color and current draw mode in the screen buffer's x,y position.
*/
void pixel(uint8_t x, uint8_t y) {
	_pixel(x,y,foreColor,drawMode);
}

/** \brief Draw pixel with color and mode.

Draw color pixel in the screen buffer's x,y position with NORM or XOR draw mode.
*/
void _pixel(uint8_t x, uint8_t y, uint8_t color, uint8_t mode) {
	if ((x<0) ||  (x>=LCDWIDTH) || (y<0) || (y>=LCDHEIGHT))
	return;

	if (mode==XOR) {
		if (color==WHITE)
		screenmemory[x+ (y/8)*LCDWIDTH] ^= _BV((y%8));
	}
	else {
		if (color==WHITE)
		screenmemory[x+ (y/8)*LCDWIDTH] |= _BV((y%8));
		else
		screenmemory[x+ (y/8)*LCDWIDTH] &= ~_BV((y%8));
	}
}

/** \brief Draw line.

Draw line using current fore color and current draw mode from x0,y0 to x1,y1 of the screen buffer.
*/
void line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
	_line(x0,y0,x1,y1,foreColor,drawMode);
}

/** \brief Draw line with color and mode.

Draw line using color and mode from x0,y0 to x1,y1 of the screen buffer.
*/
void _line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color, uint8_t mode) {
	uint8_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}

	uint8_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int8_t err = dx / 2;
	int8_t ystep;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;}

	for (; x0<x1; x0++) {
		if (steep) {
			_pixel(y0, x0, color, mode);
		} else {
			_pixel(x0, y0, color, mode);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

/** \brief Draw horizontal line.

Draw horizontal line using current fore color and current draw mode from x,y to x+width,y of the screen buffer.
*/
void lineH(uint8_t x, uint8_t y, uint8_t width) {
	_line(x,y,x+width,y,foreColor,drawMode);
}

/** \brief Draw horizontal line with color and mode.

Draw horizontal line using color and mode from x,y to x+width,y of the screen buffer.
*/
void _lineH(uint8_t x, uint8_t y, uint8_t width, uint8_t color, uint8_t mode) {
	_line(x,y,x+width,y,color,mode);
}

/** \brief Draw vertical line.

Draw vertical line using current fore color and current draw mode from x,y to x,y+height of the screen buffer.
*/
void lineV(uint8_t x, uint8_t y, uint8_t height) {
	_line(x,y,x,y+height,foreColor,drawMode);
}

/** \brief Draw vertical line with color and mode.

Draw vertical line using color and mode from x,y to x,y+height of the screen buffer.
*/
void _lineV(uint8_t x, uint8_t y, uint8_t height, uint8_t color, uint8_t mode) {
	_line(x,y,x,y+height,color,mode);
}

/** \brief Draw rectangle.

Draw rectangle using current fore color and current draw mode from x,y to x+width,y+height of the screen buffer.
*/
void rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
	_rect(x,y,width,height,foreColor,drawMode);
}

void clearBlock(uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
	_rect(x,y,width,height,BLACK,drawMode);
}



/** \brief Draw rectangle with color and mode.

Draw rectangle using color and mode from x,y to x+width,y+height of the screen buffer.
*/
void _rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color , uint8_t mode) {
	uint8_t tempHeight;

	_lineH(x,y, width, color, mode);
	_lineH(x,y+height-1, width, color, mode);

	tempHeight=height-2;

	// skip drawing vertical lines to avoid overlapping of pixel that will
	// affect XOR plot if no pixel in between horizontal lines
	if (tempHeight<1) return;

	_lineV(x,y+1, tempHeight, color, mode);
	_lineV(x+width-1, y+1, tempHeight, color, mode);
}

/** \brief Draw filled rectangle.

Draw filled rectangle using current fore color and current draw mode from x,y to x+width,y+height of the screen buffer.
*/
void rectFill(uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
	_rectFill(x,y,width,height,foreColor,drawMode);
}

/** \brief Draw filled rectangle with color and mode.

Draw filled rectangle using color and mode from x,y to x+width,y+height of the screen buffer.
*/
void _rectFill(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color , uint8_t mode) {
	// TODO - need to optimise the memory map draw so that this function will not call pixel one by one
	int i;
	for (i=x; i<x+width;i++) {
		_lineV(i,y, height, color, mode);
	}
}

/** \brief Draw circle.

    Draw circle with radius using current fore color and current draw mode at x,y of the screen buffer.
*/
void circle(uint8_t x0, uint8_t y0, uint8_t radius) {
	_circle(x0,y0,radius,foreColor,drawMode);
}

/** \brief Draw circle with color and mode.

Draw circle with radius using color and mode at x,y of the screen buffer.
*/
void _circle(uint8_t x0, uint8_t y0, uint8_t radius, uint8_t color, uint8_t mode) {
	//TODO - find a way to check for no overlapping of pixels so that XOR draw mode will work perfectly
	int8_t f = 1 - radius;
	int8_t ddF_x = 1;
	int8_t ddF_y = -2 * radius;
	int8_t x = 0;
	int8_t y = radius;

	_pixel(x0, y0+radius, color, mode);
	_pixel(x0, y0-radius, color, mode);
	_pixel(x0+radius, y0, color, mode);
	_pixel(x0-radius, y0, color, mode);

	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		_pixel(x0 + x, y0 + y, color, mode);
		_pixel(x0 - x, y0 + y, color, mode);
		_pixel(x0 + x, y0 - y, color, mode);
		_pixel(x0 - x, y0 - y, color, mode);

		_pixel(x0 + y, y0 + x, color, mode);
		_pixel(x0 - y, y0 + x, color, mode);
		_pixel(x0 + y, y0 - x, color, mode);
		_pixel(x0 - y, y0 - x, color, mode);

	}
}

/** \brief Draw filled circle.

    Draw filled circle with radius using current fore color and current draw mode at x,y of the screen buffer.
*/
void circleFill(uint8_t x0, uint8_t y0, uint8_t radius) {
	_circleFill(x0,y0,radius,foreColor,drawMode);
}

/** \brief Draw filled circle with color and mode.

    Draw filled circle with radius using color and mode at x,y of the screen buffer.
*/
void _circleFill(uint8_t x0, uint8_t y0, uint8_t radius, uint8_t color, uint8_t mode) {
	// TODO - - find a way to check for no overlapping of pixels so that XOR draw mode will work perfectly
	int8_t f = 1 - radius;
	int8_t ddF_x = 1;
	int8_t ddF_y = -2 * radius;
	int8_t x = 0;
	int8_t y = radius;
	uint8_t i;
	// Temporary disable fill circle for XOR mode.
	if (mode==XOR) return;

	for (i=y0-radius; i<=y0+radius; i++) {
		_pixel(x0, i, color, mode);
	}

	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		for (i=y0-y; i<=y0+y; i++) {
			_pixel(x0+x, i, color, mode);
			_pixel(x0-x, i, color, mode);
		}
		for (i=y0-x; i<=y0+x; i++) {
			_pixel(x0+y, i, color, mode);
			_pixel(x0-y, i, color, mode);
		}
	}
}

/** \brief Get LCD height.

    The height of the LCD return as byte.
*/
uint8_t getLCDHeight(void) {
	return LCDHEIGHT;
}

/** \brief Get LCD width.

    The width of the LCD return as byte.
*/
uint8_t getLCDWidth(void) {
	return LCDWIDTH;
}

/** \brief Get font width.

    The cucrrent font's width return as byte.
*/
uint8_t getFontWidth(void) {
	return fontWidth;
}

/** \brief Get font height.

    The current font's height return as byte.
*/
uint8_t getFontHeight(void) {
	return fontHeight;
}

/** \brief Get font starting character.

    Return the starting ASCII character of the currnet font, not all fonts start with ASCII character 0. Custom fonts can start from any ASCII character.
*/
uint8_t getFontStartChar(void) {
	return fontStartChar;
}

/** \brief Get font total characters.

    Return the total characters of the current font.
*/
uint8_t getFontTotalChar(void) {
	return fontTotalChar;
}

/** \brief Get total fonts.

    Return the total number of fonts loaded into the MicroOLED's flash memory.
*/
uint8_t getTotalFonts(void) {
	return TOTALFONTS;
}

/** \brief Get font type.

    Return the font type number of the current font.
*/
uint8_t getFontType(void) {
	return fontType;
}

/** \brief Set font type.

    Set the current font type number, ie changing to different fonts base on the type provided.
*/
uint8_t setFontType(uint8_t type) {
	if ((type>=TOTALFONTS) || (type<0))
	return false;

	fontType=type;
	fontWidth=font8x16[0];
	fontHeight=font8x16[1];
	fontStartChar=font8x16[2];
	fontTotalChar=font8x16[3];
	fontMapWidth=font8x16[4]*100+font8x16[5]; // two bytes values into integer 16
	return true;
}

/** \brief Set color.

    Set the current draw's color. Only WHITE and BLACK available.
*/
void setColor(uint8_t color) {
	foreColor=color;
}

/** \brief Set draw mode.

    Set current draw mode with NORM or XOR.
*/
void setDrawMode(uint8_t mode) {
	drawMode=mode;
}

/** \brief Draw character.

    Draw character c using current color and current draw mode at x,y.
*/
void  drawChar(uint8_t x, uint8_t y, uint8_t c) {
	_drawChar(x,y,c,foreColor,drawMode);
}

/** \brief Draw character with color and mode.

    Draw character c using color and draw mode at x,y.
*/
void  _drawChar(uint8_t x, uint8_t y, uint8_t c, uint8_t color, uint8_t mode) {
	// TODO - New routine to take font of any height, at the moment limited to font height in multiple of 8 pixels

	uint8_t rowsToDraw,row, tempC;
	uint8_t i,j,temp;
	uint16_t charPerBitmapRow,charColPositionOnBitmap,charRowPositionOnBitmap,charBitmapStartPosition;

	if ((c<fontStartChar) || (c>(fontStartChar+fontTotalChar-1)))		// no bitmap for the required c
	return;

	tempC=c-fontStartChar;

	// each row (in datasheet is call page) is 8 bits high, 16 bit high character will have 2 rows to be drawn
	rowsToDraw=fontHeight/8;	// 8 is LCD's page size, see SSD1306 datasheet
	if (rowsToDraw<=1) rowsToDraw=1;

	// the following draw function can draw anywhere on the screen, but SLOW pixel by pixel draw
	if (rowsToDraw==1) {
		for  (i=0;i<fontWidth+1;i++) {
			if (i==fontWidth) // this is done in a weird way because for 5x7 font, there is no margin, this code add a margin after col 5
			temp=0;
			else
			temp=font8x16[FONTHEADERSIZE+(tempC*fontWidth)+i];

			for (j=0;j<8;j++) {			// 8 is the LCD's page height (see datasheet for explanation)
				if (temp & 0x1) {
					_pixel(x+i, y+j, color,mode);
				}
				else {
					_pixel(x+i, y+j, !color,mode);
				}

				temp >>=1;
			}
		}
		return;
	}

	// font height over 8 bit
	// take character "0" ASCII 48 as example
	charPerBitmapRow=fontMapWidth/fontWidth;  // 256/8 =32 char per row
	charColPositionOnBitmap=tempC % charPerBitmapRow;  // =16
	charRowPositionOnBitmap=tempC/charPerBitmapRow; // =1
	charBitmapStartPosition=(charRowPositionOnBitmap * fontMapWidth * (fontHeight/8)) + (charColPositionOnBitmap * fontWidth) ;

	// each row on LCD is 8 bit height (see datasheet for explanation)
	for(row=0;row<rowsToDraw;row++) {
		for (i=0; i<fontWidth;i++) {
			temp=font8x16[FONTHEADERSIZE+(charBitmapStartPosition+i+(row*fontMapWidth))];
			for (j=0;j<8;j++) {			// 8 is the LCD's page height (see datasheet for explanation)
				if (temp & 0x1) {
					_pixel(x+i,y+j+(row*8), color, mode);
				}
				else {
					_pixel(x+i,y+j+(row*8), !color, mode);
				}
				temp >>=1;
			}
		}
	}

}

/** \brief Stop scrolling.

    Stop the scrolling of graphics on the OLED.
*/
void scrollStop(void){
	command(DEACTIVATESCROLL);
}

/** \brief Right scrolling.

Set row start to row stop on the OLED to scroll right. Refer to http://learn.microview.io/intro/general-overview-of-microview.html for explanation of the rows.
*/
void scrollRight(uint8_t start, uint8_t stop){
	if (stop<start)		// stop must be larger or equal to start
	return;
	scrollStop();		// need to disable scrolling before starting to avoid memory corrupt
	command(RIGHTHORIZONTALSCROLL);
	command(0x00);
	command(start);
	command(0x7);		// scroll speed frames , TODO
	command(stop);
	command(0x00);
	command(0xFF);
	command(ACTIVATESCROLL);
}

/** \brief Vertical flip.

Flip the graphics on the OLED vertically.
*/
void flipVertical(uint8_t flip) {
	if (flip == 1) {
		command(COMSCANINC);
	}
	else {
		command(COMSCANDEC);
	}
}

/** \brief Horizontal flip.

    Flip the graphics on the OLED horizontally.
*/
void flipHorizontal(uint8_t flip) {
	if (flip == 1) {
		command(SEGREMAP | 0x0);
	}
	else {
		command(SEGREMAP | 0x1);
	}
}

/*
	Return a pointer to the start of the RAM screen buffer for direct access.
*/
uint8_t *getScreenBuffer(void) {
	return screenmemory;
}

/*
Draw Bitmap image on screen. The array for the bitmap can be stored in the Arduino file, so user don't have to mess with the library files.
To use, create uint8_t array that is 64x48 pixels (384 bytes). Then call .drawBitmap and pass it the array.
*/
void drawBitmap(uint8_t * bitArray)
{
	int i;
  for (i=0; i<(LCDWIDTH * LCDHEIGHT / 8); i++)
    screenmemory[i] = bitArray[i];
}


void drawSmile()
{
	clear(PAGE);
	uint8_t middle_x = getLCDWidth() / 2;
	uint8_t middle_y = getLCDHeight() / 2;
	uint8_t radius = getLCDWidth() / 3;
	circle(middle_x, middle_y, radius); //face
	circleFill(middle_x / 3 * 2, middle_y / 3 * 2, 2); //left eye
	circleFill(middle_x / 3 * 4, middle_y / 3 * 2, 2); //right eye

	//draw mouth
	double angle = 220;
	double val = 3.1415 / 180.0;
	uint8_t x, y, mouth_radius, mouth_x, mouth_y;
	mouth_radius = radius / 2;
	mouth_x = middle_x;
	mouth_y = middle_y;
	for (; angle < 320; angle += 2){
		x = (uint8_t) mouth_x + mouth_radius * cos(angle * val);
		y = (uint8_t) mouth_y - mouth_radius * sin(angle * val);
		pixel(x, y);
	}
	display();
}

void drawCry()
{
	clear(PAGE);
	uint8_t middle_x = getLCDWidth() / 2;
	uint8_t middle_y = getLCDHeight() / 2;
	uint8_t radius = getLCDWidth() / 3;
	circle(middle_x, middle_y, radius); //face
	circleFill(middle_x / 3 * 2, middle_y / 3 * 2, 2); //left eye
	circleFill(middle_x / 3 * 4, middle_y / 3 * 2, 2); //right eye

	//draw mouth
	double angle = 40;
	double val = 3.1415 / 180.0;
	uint8_t x, y, mouth_radius, mouth_x, mouth_y;
	mouth_radius = radius / 2;
	mouth_x = middle_x;
	mouth_y = middle_y / 3 * 5;
	for (; angle < 140; angle += 2){
		x = (uint8_t) mouth_x + mouth_radius * cos(angle * val);
		y = (uint8_t) mouth_y - mouth_radius * sin(angle * val);
		pixel(x, y);
	}
	display();
}

void drawSignal(uint8_t good)
{
	uint8_t x0, y0, w, h;
	x0 = 0;
	y0 = 0;
	w = 10;
	h = 10;
	clearBlock(x0, y0, w, h);
	if (good){
		line(x0, y0, x0 + 5, y0);
		line(x0, y0, x0 + 3, y0 + 3);
		line(x0 + 3, y0 + 3, x0 + 5, y0);
		line(x0 + 3, y0 + 3, x0 + 3, y0 + h);
		line(x0 + 6, y0 + 3, x0 + 6, y0 + h);
		line(x0 + 8, y0 + 2, x0 + 8, y0 + h);
		line(x0 + w, y0, x0 + w, y0 + h);
	}
	else{
		line(x0, y0, x0 + 5, y0);
		line(x0, y0, x0 + 3, y0 + 3);
		line(x0 + 3, y0 + 3, x0 + 5, y0);
		line(x0 + 3, y0 + 3, x0 + 3, y0 + h);
		line(x0 + 4, y0 + 4, x0 + w, y0 + h);
		line(x0 + 4, y0 + h, x0 + w, y0 + 4);
	}
	display();
}

void drawSpeedup()
{
	clear(PAGE);
	uint8_t middle_x = getLCDWidth() / 2;
	uint8_t middle_y = getLCDHeight() / 2;
	line(middle_x - 15, middle_y + 15, middle_x, middle_y);
	line(middle_x, middle_y, middle_x + 15, middle_y + 15);
	line(middle_x - 15, middle_y, middle_x, middle_y - 15);
	line(middle_x, middle_y - 15, middle_x + 15, middle_y);
	display();
}

void drawHealing()
{
	clear(PAGE);
	uint8_t middle_x = getLCDWidth() / 2;
	uint8_t middle_y = getLCDHeight() / 2;
	circle(middle_x, middle_y, 20);
	rectFill(middle_x - 10, middle_y - 3, 20, 6);
	rectFill(middle_x - 3, middle_y - 10, 6, 20);
	display();
}

void drawBomb()
{
	clear(PAGE);
	uint8_t middle_x = getLCDWidth() / 2;
	uint8_t middle_y = getLCDHeight() / 2;
	circle(middle_x, middle_y, 14);
	line(middle_x - 9, middle_y - 5, middle_x + 5, middle_y + 9);
	line(middle_x - 5, middle_y - 9, middle_x + 9, middle_y + 5);
	line(middle_x - 9, middle_y + 5, middle_x + 5, middle_y - 9);
	line(middle_x - 5, middle_y + 9, middle_x + 9, middle_y - 5);

	line(middle_x, middle_y - 16, middle_x + 12, middle_y - 9);
	line(middle_x + 4, middle_y - 21, middle_x + 16, middle_y - 14);
	line(middle_x, middle_y - 16, middle_x + 4, middle_y - 21);
	line(middle_x + 12, middle_y - 9, middle_x + 16, middle_y - 14);

	line(middle_x + 8, middle_y - 18, middle_x + 13, middle_y - 19);
	display();
}

void drawShoot()
{
	clear(PAGE);
	drawChar(10, 20, 'F');
	drawChar(18, 20, 'i');
	drawChar(26, 20, 'r');
	drawChar(34, 20, 'e');
	drawChar(42, 20, '!');
	display();
}

void drawString(uint8_t row, uint8_t col, uint8_t length, char *string, uint8_t hold){
	if (!hold) clear(PAGE);
	uint8_t y = row * 12;
	uint8_t x = col * 8;
	uint8_t i;
	for (i = 0; i < length; i++){
		drawChar(x + i * 8, y, string[i]);
	}
	display();
}

void printU16(uint8_t row, uint8_t col, uint16_t data)
{
	char data_disp[5];
	int i = 0;
	for( i = 0; i < 5; ++i)
	{
		data_disp[4 - i] = data%10 + 48;
		data = data/10;
	}
	drawString(row, col, 5, data_disp, 1);
}

void printU8(uint8_t row, uint8_t col, uint8_t data)
{
	char data_disp[3];
	int i = 0;
	for( i = 0; i < 3; ++i)
	{
		data_disp[2 - i] = data%10 + 48;
		data = data/10;
	}
	drawString(row, col, 3, data_disp, 1);
}

void i2cSetup()
{
	MSS_I2C_init( &g_mss_i2c1, 0, MSS_I2C_PCLK_DIV_60 );
}

void i2cWrite(byte address, byte control, byte data)
{
	uint8_t tx_buffer[2] = {control, data};
	MSS_I2C_write( &g_mss_i2c1, address, tx_buffer, sizeof(tx_buffer),
	                       MSS_I2C_RELEASE_BUS );
	        // Wait for completion and record the outcome
	mss_i2c_status_t status = MSS_I2C_wait_complete( &g_mss_i2c1, MSS_I2C_NO_TIMEOUT );
}


void oled_delay(uint32_t time)
{
	volatile uint32_t i = time * 50000;
	while (i > 0)
	{
		asm("nop;");
		i--;
		//printf("delay %d", i);
	}
}
