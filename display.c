#include "display.h"
#include "font.h"

static void Display_command( struct DisplayGL * display, uint8_t command );
static void Display_data( struct DisplayGL *display, uint8_t data );
static void Display_setRamAddr( struct DisplayGL * display, uint8_t x0, uint8_t y0, uint8_t width, uint8_t height );

/*
 * @brief Initialize display
 *
 * Initialization is carried out in 3 stages:
 * GPIO initialization,
 * SPI/I2C initialization,
 * sending a packet of control bytes for initial display setup.
 * @param display - pointer to display struct
 * @return void
 */
void Display_init( struct DisplayGL * display )
{
	uint16_t i;

	GPIO_clockInit();	/* Enable GPIO */

#if defined(DISPLAY_USE_HW_4SPI)

	GPIO_configPin(display->clkPinPort, display->clkPin, GPIO_OUTPUT_AF_PUSH_PULL);		/* Set CLK as alternate output */
	GPIO_configPin(display->dataPinPort, display->dataPin, GPIO_OUTPUT_AF_PUSH_PULL);	/* Set DATA as alternate output */

	SPI_clockEn();	/* Enable SPI */

	SPI_masterInit(display->spi);	/* Init SPI as master */

#elif defined(DISPLAY_USE_SW_SPI)

	GPIO_configPin(display->clkPinPort, display->clkPin, GPIO_OUTPUT_PUSH_PULL);	/* Set CLK as output */
	GPIO_configPin(display->dataPinPort, display->dataPin, GPIO_OUTPUT_PUSH_PULL);	/* Set DATA as output */

#endif

	GPIO_configPin(display->csPinPort, display->csPin, GPIO_OUTPUT_PUSH_PULL);	/* Set CS, DC, RES as output */
	GPIO_configPin(display->dcPinPort, display->dcPin, GPIO_OUTPUT_PUSH_PULL);
	GPIO_configPin(display->resPinPort, display->resPin, GPIO_OUTPUT_PUSH_PULL);

	GPIO_setPin(display->csPinPort, display->csPin, 1);		/* Unselect display (CS = 1) */
	GPIO_setPin(display->resPinPort, display->resPin, 0);	/* Reset display (RES = 0) */

	for(i = 0; i < 1024; i++); 	/* Delay */

	GPIO_setPin(display->resPinPort, display->resPin, 1);	/* End reset (RES = 1) */

	/* Send control bytes to the display */

	/* Set command lock */
	Display_command(display, 0xFD);
	Display_data(display, 0x12);

	/* Set command lock */
	Display_command(display, 0xFD);
	Display_data(display, 0xB1);

	/* Turn display off */
	Display_command(display, 0xAE);

	/* Set display clock div */
	Display_command(display, 0xB3);

	/* 7:4 = oscillator frequency, 3:0 = CLK div ratio (A[3:0]+1 = 1..16) */
	Display_command(display, 0xF1);

	/* Set display mux ratio */
	Display_command(display, 0xCA);
	Display_data(display, 0x7f);

	/* Set display remap */
	Display_command(display, 0xA0);
	Display_data(display, 0x74);

	/* Set display column */
	Display_command(display, 0x15);
	Display_data(display, 0x00);
	Display_data(display, 0x7F);

	/* Set display row*/
	Display_command(display, 0x75);
	Display_data(display, 0x00);
	Display_data(display, 0x7F);

	/* Set display start line */
	Display_command(display, 0xA1);
	Display_data(display, 0x00);

	/* Set display offset */
	Display_command(display, 0xA2);
	Display_data(display, 0x00);

	/* Set display controller GPIO */
	Display_command(display, 0xB5);
	Display_data(display, 0x00);

	/* Display function select */
	Display_command(display, 0xAB);
	Display_data(display, 0x01);

	/* Set display precharge */
	Display_command(display, 0xB1);
	Display_data(display, 0x32);

	/* Set display VCOMH */
	Display_command(display, 0xBE);
	Display_data(display, 0x05);

	/* Set display normal state (not inverted) */
	Display_command(display, 0xA6);

	/* Set display contrast */
	Display_command(display, 0xC1);
	Display_data(display, 0xC8);
	Display_data(display, 0x80);
	Display_data(display, 0xC8);

	/* Set CONTRASTMSTR */
	Display_command(display, 0xC7);
	Display_data(display, 0x0F);

	/* Set display VSL */
	Display_command(display, 0xB4);
	Display_data(display, 0xA0);
	Display_data(display, 0xB5);
	Display_data(display, 0x55);

	/* Set display precharge2 */
	Display_command(display, 0xB6);
	Display_data(display, 0x01);

	/* Turn display on */
	Display_command(display, 0xAF);

	Display_setBackColor(display, DISPLAY_DEFAULT_BACK_COLOR);	/* Default color settings */
	Display_setDrawColor(display, DISPLAY_DEFAULT_DRAW_COLOR);

	Display_setDrawMode(display, DISPLAY_DEFAULT_DRAW_MODE);

	Display_clear(display);	/* Clear display RAM */
}


/*
 * Send command to the display
 * @param display - pointer to display struct
 * @param command - command byte
 * @return - void
 */
void Display_command( struct DisplayGL * display, uint8_t command )
{
	GPIO_setPin(display->dcPinPort, display->dcPin, 0);	/* Set command-mode (DC = 0) */

	GPIO_setPin(display->csPinPort, display->csPin, 0);	/* Select display (CS = 0) */

#if defined(DISPLAY_USE_HW_4SPI)

	SPI_sendByte(display->spi, command );	/* Send command byte */

#elif defined(DISPLAY_USE_SW_SPI)

	swSpiWrite( display, command );

#endif

	GPIO_setPin(display->csPinPort, display->csPin, 1);	/* Unselect display */
}


/*
 * Send data to the display
 * @param display - pointer to display struct
 * @param data - data byte
 * @return - void
 */
void Display_data( struct DisplayGL * display, uint8_t data )
{
	GPIO_setPin(display->dcPinPort, display->dcPin, 1);	/* Set data-mode  (DC = 1) */

	GPIO_setPin(display->csPinPort, display->csPin, 0);	/* Select display (CS = 0) */

#if defined(DISPLAY_USE_HW_4SPI)

	SPI_sendByte(display->spi, data);	/* Send data byte */

#elif defined(DISPLAY_USE_SW_SPI)

	swSpiWrite(display, data);
#endif

	GPIO_setPin(display->csPinPort, display->csPin, 1);	/* Unselect display */
}

/*
 * Select display RAM address
 * @param display - pointer to display struct
 * @param x0
 */
void Display_setRamAddr( struct DisplayGL * display, uint8_t x0, uint8_t y0, uint8_t width, uint8_t height )
{
	uint16_t x1, y1;

	if(x0 > DISPLAY_WIDTH || y0 > DISPLAY_HEIGHT) return;

	x1 = x0 + width - 1;
	y1 = y0 + height - 1;

	if(x1 > DISPLAY_WIDTH || y1 > DISPLAY_HEIGHT) return;

	Display_command(display, 0x15); /* set column */
	Display_data(display, x0);
	Display_data(display, x1);

	Display_command(display, 0x75); /* set row*/
	Display_data(display, y0);
	Display_data(display, y1);

	Display_command(display, 0x5C); /* enable display RAM output */
}

/*
 * Clear data that loaded to the display. Also clears buffer
 * @param display - pointer to the display struct
 * @return - void
 */
void Display_clear(struct DisplayGL *display)
{
	uint16_t i;

	Display_setRamAddr(display, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);

	for(i = 0; i < DISPLAY_RAM_SIZE; i++)
	{
		Display_data(display, DISPLAY_DEFAULT_BACK_COLOR);
	}
}

void Display_fill( struct DisplayGL * display, uint16_t color )
{
	uint16_t i;

	Display_setRamAddr(display, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);

	for(i = 0; i < DISPLAY_RAM_SIZE; i++)
	{
		Display_data(display, color >> 8);
		Display_data(display, color & 0xFF);
	}
}


/*
 * Change display draw color
 * @param display - pointer to the display struct
 * @param color - the color that should be fixed
 * @return - void
 */
void Display_setDrawColor(struct DisplayGL *display, uint16_t color)
{
	display->currentDrawColor = color;
}

void Display_setBackColor(struct DisplayGL *display, uint16_t color)
{
	display->currentBackColor = color;
}

void Display_setDrawMode(struct DisplayGL *display, uint8_t mode)
{
	display->drawMode = mode;
}

/*
 * Draw a pixel at the specified coordinates
 * @param display - pointer to the display struct
 * @param x - x-axis coordinate
 * @param y - y-axis coordinate
 * @return - void
 */
void Display_drawPixel(struct DisplayGL *display, uint8_t x, uint8_t y, uint16_t color)
{
	if(x > DISPLAY_WIDTH - 1 || y > DISPLAY_HEIGHT - 1) return;

	Display_setRamAddr(display, x, y, 1, 1);

	Display_data(display, color >> 8);
	Display_data(display, color & 0x00FF );
}




/*
 * Draw a ascii char on specified row and column
 * @param display - pointer to the display struct
 * @param str - string(0 - 7)
 * @param col - column(0 - 20)
 * @param asciiChr - char to draw
 * @return - void
 */
void Display_drawAsciiChar(struct DisplayGL *display, uint8_t x, uint8_t y, uint8_t asciiChr)
{
	uint8_t i, j;

	if(x > DISPLAY_WIDTH || y > DISPLAY_HEIGHT) return; /*checking for compliance with restrictions*/

	if((asciiChr >= 0x20) && (asciiChr <= 0x7f)) /*offset selection*/
	{
		asciiChr -= 32;
	}
	else if(asciiChr >= 0xc0)
	{
		asciiChr -= 96;
	}
	else
	{
		asciiChr = 85;
	}

	/*
	 * Note: this code imposes restrictions on the locations of characters,
	 * it is recommended to rewrite it later
	 */


	for(i = 0; i < 5; i++)
	{
		for(j = 0; j < 8; j++)
		{
			if(font_5x8[asciiChr * 5 + i] & (1 << j))
			{
				Display_drawPixel(display, x + i, y + j, display->currentDrawColor);
			}
			else if(!(font_5x8[asciiChr * 5 + i] & (1 << j)) && display->drawMode == DISPLAY_DRAW_MODE_OVERRIDE)
			{
				Display_drawPixel(display, x + i, y + j, display->currentBackColor);
			}
		}
	}

}


/*
 * Set display cursor to the specified position
 * @param display
 */
void Display_setCursor(struct DisplayGL *display, uint8_t x, uint8_t y)
{
	if(x > DISPLAY_WIDTH || y > DISPLAY_HEIGHT) return;

	display->cursorX = x;
	display->cursorY = y;
}

#define DISPLAY_CURSOR_OFFSET_X	(uint8_t)(DISPLAY_FONT_WIDTH + 1)
#define DISPLAY_CURSOR_OFFSET_Y	(uint8_t)(DISPLAY_FONT_HEIGHT + 1)

void Display_printString(struct DisplayGL *display, char str[])
{
	uint8_t i = 0;

	if(display->cursorX > DISPLAY_WIDTH || display->cursorY > DISPLAY_HEIGHT) return;

	while(str[i] != 0 && display->cursorX < DISPLAY_WIDTH)
	{
		Display_drawAsciiChar(display, display->cursorX, display->cursorY, str[i]);
		display->cursorX += 6;
		i++;
	}

}

void Display_printNum(struct DisplayGL *display, int32_t num)
{
	uint8_t numLength = 0;
	int32_t numBuff;
	uint8_t numChar;

	if(num == 0)
	{
		Display_drawAsciiChar(display, display->cursorX, display->cursorY, '0');
		display->cursorX += DISPLAY_CURSOR_OFFSET_X;
		return;
	}

	if(num < 0)
	{
		Display_drawAsciiChar(display, display->cursorX, display->cursorY, '-');
		display->cursorX += DISPLAY_CURSOR_OFFSET_X;

		num = num * -1;
	}

	numBuff = num;

	while(numBuff)
	{
		numLength++;
		numBuff /= 10;
	}

	while((numLength--) && display->cursorX < DISPLAY_WIDTH)
	{
		numBuff = (num / powl(10, numLength));
		numChar = numBuff + '0';
		num -= numBuff * powl(10, numLength);

		Display_drawAsciiChar(display, display->cursorX, display->cursorY, numChar);

		display->cursorX += DISPLAY_CURSOR_OFFSET_X;
	}
}

void Display_print(struct DisplayGL *display, const char *format, ...)
{
	//TODO
}

void Display_drawFrame(struct DisplayGL *display, uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	uint8_t i, j;

	for(i = 0; i < height; i++)
	{
		Display_drawPixel(display, x, y + i, display->currentDrawColor);
	}

	for(i = 1; i < width - 1; i++)
	{
		Display_drawPixel(display, x + i, y, display->currentDrawColor);

		if(display->drawMode == DISPLAY_DRAW_MODE_OVERRIDE)
		{
			for(j = 1; j < height; j++)
			{
				Display_drawPixel(display, x + i, y + j, display->currentBackColor);
			}
		}

		Display_drawPixel(display, x + i, y + height - 1, display->currentDrawColor);

	}

	for(i = 0; i < height; i++)
	{
		Display_drawPixel(display, x + width - 1, y + i, display->currentDrawColor);
	}
}


void Display_drawBox(struct DisplayGL *display, uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	uint8_t i, j;

	if(x > DISPLAY_WIDTH || y > DISPLAY_HEIGHT) return;

	for(i = 0; i < width; i++)
	{
		for(j = 0; j < height; j++)
		{
			Display_drawPixel(display, x + i, y + j, display->currentDrawColor);
		}
	}
}



void Display_drawXBM(struct DisplayGL *display, uint8_t xbmStartx, uint8_t xbmStarty, uint8_t xbmWidth, uint8_t xbmHeight, uint8_t xbm[])
{
	uint8_t xbmArrayLength;
	uint8_t i;
	uint8_t j;
	int8_t k;

	xbmArrayLength = (xbmWidth / 8) + 1;
	if((xbmWidth % 8) == 0) xbmArrayLength = (xbmWidth / 8);

	for(i = 0; i < xbmHeight; i++)
	{
		k = 0;
		for(j = 0; j < xbmWidth; j++)
		{
			if(k == 8) k = 0;
			if(xbm[(j / 8) + (i * xbmArrayLength)] & (1 << k))
			{
				Display_drawPixel(display, xbmStartx + j, xbmStarty + i, display->currentDrawColor);
			}
			k++;
		}
	}
}

#if defined(DISPLAY_USE_SW_SPI)
void swSpiWrite(struct DisplayGL * display, uint8_t data)
{
	uint8_t i;

	GPIO_setPin(display->csPinPort, display->csPin, 0);

	for(i = 8; i > 0; i--)
	{
		GPIO_setPin(display->dataPinPort, display->dataPin, (data & (1 << (i - 1))));
		GPIO_setPin(display->clkPinPort, display->clkPin, 1);

		GPIO_setPin(display->clkPinPort, display->clkPin, 0);

	}

	GPIO_setPin(display->csPinPort, display->csPin, 1);
}
#endif
