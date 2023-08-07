#include "display.h"
#include "font.h"

/*
 * Display initialization.
 * Initialization is carried out in 3 stages:
 * GPIO initialization,
 * SPI/I2C initialization,
 * sending a packet of control bytes for initial display setup.
 * @param display - pointer to display struct
 * @return void
 */
void Display_init(struct DisplayGL *display)
{
	uint16_t i;

	GPIO_clockInit();

#if defined(DISPLAY_USE_SPI)

	GPIO_configPin(display->clkPinPort, display->clkPin, GPIO_OUTPUT_AF_PUSH_PULL);
	GPIO_configPin(display->dataPinPort, display->dataPin, GPIO_OUTPUT_AF_PUSH_PULL);

	SPI_clockEn();

	SPI_masterInit(display->spi);

#elif defined(DISPLAY_USE_SW_SPI)

	GPIO_configPin(display->clkPinPort, display->clkPin, GPIO_OUTPUT_PUSH_PULL);
	GPIO_configPin(display->dataPinPort, display->dataPin, GPIO_OUTPUT_PUSH_PULL);

#endif

	GPIO_configPin(display->csPinPort, display->csPin, GPIO_OUTPUT_PUSH_PULL);
	GPIO_configPin(display->dcPinPort, display->dcPin, GPIO_OUTPUT_PUSH_PULL);
	GPIO_configPin(display->resPinPort, display->resPin, GPIO_OUTPUT_PUSH_PULL);

	GPIO_setPin(display->csPinPort, display->csPin, 1);
	GPIO_setPin(display->resPinPort, display->resPin, 0);

	for(i = 0; i < DISPLAY_BUFFER_SIZE; i++) /*clear display buffer from data-trash*/
	{
		display->displayBuffer[i] = 0;
	}

	GPIO_setPin(display->resPinPort, display->resPin, 1);

	Display_command(display, 0xAE); /*Send control bytes*/
	Display_command(display, 0xD5);
	Display_command(display, 0x80);
	Display_command(display, 0xA8);
	Display_command(display, 0x3F);
	Display_command(display, 0xD3);
	Display_command(display, 0x00);
	Display_command(display, 0x40);
	Display_command(display, 0x8D);
	Display_command(display, 0x14);
	Display_command(display, 0x20);
	Display_command(display, 0x00);
	Display_command(display, 0xA1);
	Display_command(display, 0xC8);
	Display_command(display, 0xDA);
	Display_command(display, 0x12);
	Display_command(display, 0x81);
	Display_command(display, 0x8F);
	Display_command(display, 0xD9);
	Display_command(display, 0xF1);
	Display_command(display, 0xDB);
	Display_command(display, 0x10);
	Display_command(display, 0xA4);
	Display_command(display, 0xA6);
	Display_command(display, 0xAF);
}


/*
 * Send command to the display
 * @param display - pointer to display struct
 * @param command - command byte
 * @return - void
 */
void Display_command(struct DisplayGL *display, uint8_t command)
{
	GPIO_setPin(display->dcPinPort, display->dcPin, 0);

	GPIO_setPin(display->csPinPort, display->csPin, 0);

#if defined(DISPLAY_USE_SPI)

	SPI_sendByte(display->spi, command);

#elif defined(DISPLAY_USE_SW_SPI)

	swSpiWrite(display, command);

#endif

	GPIO_setPin(display->csPinPort, display->csPin, 1);
}


/*
 * Send data to the display
 * @param display - pointer to display struct
 * @param data - data byte
 * @return - void
 */
void Display_data(struct DisplayGL *display, uint8_t data)
{
	GPIO_setPin(display->dcPinPort, display->dcPin, 1);

	GPIO_setPin(display->csPinPort, display->csPin, 0);

#if defined(DISPLAY_USE_SPI)

	SPI_sendByte(display->spi, data);

#elif defined(DISPLAY_USE_SW_SPI)

	swSpiWrite(display, data);
#endif

	GPIO_setPin(display->csPinPort, display->csPin, 1);
}


/*
 * Send the contents of the buffer to the display
 * @param display - pointer to the display struct
 * @return - void
 */
void Display_sendBuffer(struct DisplayGL *display)
{
	uint16_t i;

	Display_command(display, 0x21);
	Display_command(display, 0);
	Display_command(display, 127);

	Display_command(display, 0x22);
	Display_command(display, 0);
	Display_command(display, 7);

	for(i = 0; i < DISPLAY_BUFFER_SIZE; i++)
	{
		Display_data(display, display->displayBuffer[i]);
	}
}


/*
 * clear the contents of display buffer.
 * The data already loaded to the display does not change
 * @param display - pointer to the display struct
 * @return - void
 */
void Display_clearBuffer(struct DisplayGL *display)
{
	uint16_t i;

	if(display->currentDrawColor == COLOR_WHITE)
	{
		for(i = 0; i < DISPLAY_BUFFER_SIZE; i++)
		{
			display->displayBuffer[i] = 0;
		}
	}
	else if(display->currentDrawColor == COLOR_BLACK)
	{
		for(i = 0; i < DISPLAY_BUFFER_SIZE; i++)
		{
			display->displayBuffer[i] = 0xff;
		}

	}
}


/*
 * Clear data that loaded to the display. Also clears buffer
 * @param display - pointer to the display struct
 * @return - void
 */
void Display_clear(struct DisplayGL *display)
{
	Display_clearBuffer(display);
	Display_sendBuffer(display);
}


/*
 * Change display draw color
 * @param display - pointer to the display struct
 * @param color - the color that should be fixed
 * @return - void
 */
void Display_setDrawColor(struct DisplayGL *display, DisplayColor_e color)
{
	display->currentDrawColor = color;
}


/*
 * Draw a pixel at the specified coordinates
 * @param display - pointer to the display struct
 * @param x - x-axis coordinate
 * @param y - y-axis coordinate
 * @return - void
 */
void Display_drawPixel(struct DisplayGL *display, uint8_t x, uint8_t y)
{
	if(x > DISPLAY_WIDTH - 1 || y > DISPLAY_HEIGHT - 1) return;

	if(display->currentDrawColor == COLOR_WHITE)
	{
		display->displayBuffer[x + (y / 8) * DISPLAY_WIDTH] |= (1 << (y % 8));
	}
	else if(display->currentDrawColor == COLOR_BLACK)
	{
		display->displayBuffer[x + (y / 8) * DISPLAY_WIDTH] &= ~(1 << (y % 8));
	}
}


/*
 * Clear a pixel at the specified coordinates
 * @param display - pointer to the display struct
 * @param x - x-axis coordinate
 * @param y - y-axis coordinate
 * @return - void
 */
void Display_clearPixel(struct DisplayGL *display, uint8_t x, uint8_t y)
{
	if(x > DISPLAY_WIDTH - 1 || y > DISPLAY_HEIGHT - 1) return;

	if(display->currentDrawColor == COLOR_BLACK)
	{
		display->displayBuffer[x + (y / 8) * DISPLAY_WIDTH] |= (1 << (y % 8));
	}
	else if(display->currentDrawColor == COLOR_WHITE)
	{
		display->displayBuffer[x + (y / 8) * DISPLAY_WIDTH] &= ~(1 << (y % 8));
	}
}


/*
 * Fill the display with the current draw color
 * @param display - pointer to the display struct
 * @return - void
 */
void Display_fill(struct DisplayGL *display)
{
	uint16_t i;

	if(display->currentDrawColor == COLOR_WHITE)
	{
		for(i = 0; i < DISPLAY_BUFFER_SIZE; i++)
		{
			display->displayBuffer[i] = 0xff;
		}
	}
	else if(display->currentDrawColor == COLOR_BLACK)
	{
		for(i = 0; i < DISPLAY_BUFFER_SIZE; i++)
		{
			display->displayBuffer[i] = 0x00;
		}

	}
}


/*
 * Invert colors of display buffer
 * The data already loaded to the display does not change
 * @param display - pointer to the display struct
 */
void Display_invert(struct DisplayGL *display)
{
	uint16_t i;

	for(i = 0; i < DISPLAY_BUFFER_SIZE; i++)
	{
		display->displayBuffer[i] = ~display->displayBuffer[i];
	}
}


/*
 * Draw a ascii char on specified row and column
 * @param display - pointer to the display struct
 * @param str - string(0 - 7)
 * @param col - column(0 - 20)
 * @param asciiChr - char to draw
 * @return - void
 */
void Display_drawAsciiChar(struct DisplayGL *display, uint8_t str, uint8_t col, uint8_t asciiChr)
{
	uint8_t i;

	if(col > 20 || str > 7) return; /*checking for compliance with restrictions*/

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

	if(display->currentDrawColor == COLOR_WHITE)
	{
		for(i = 0; i < 5; i ++)
		{
			display->displayBuffer[DISPLAY_WIDTH * str + col*6 + i] = font_5x8[asciiChr * 5 + i];
		}
	}
	else if(display->currentDrawColor == COLOR_BLACK)
	{
		for(i = 0; i < 5; i ++)
		{
			display->displayBuffer[DISPLAY_WIDTH * str + col + i] = ~font_5x8[asciiChr * 5 + i];
		}
	}
}


/*
 * Set display cursor to the specified position
 * @param display
 */
void Display_setCursor(struct DisplayGL *display, uint8_t str, uint8_t col)
{
	if(col > 21 || str > 7) return;

	display->cursorCol = col;
	display->cursorStr = str;
}

void Display_printString(struct DisplayGL *display, uint8_t str[])
{
	uint8_t i = 0;

	if(display->cursorStr > 7 || display->cursorCol > 127) return;

	while(str[i] != 0 && display->cursorCol < 21)
	{
		Display_drawAsciiChar(display, display->cursorStr, display->cursorCol, str[i]);
		display->cursorCol++;
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
		Display_drawAsciiChar(display, display->cursorStr, display->cursorCol, '0');
		display->cursorCol++;
		return;
	}

	numBuff = num;

	if(num < 0)
	{
		Display_drawAsciiChar(display, display->cursorStr, display->cursorCol, '-');
		display->cursorCol++;
		num = num * -1;
	}

	numBuff = num;

	while(numBuff)
	{
		numLength++;
		numBuff /= 10;
	}

	while((numLength--) && display->cursorCol < 21)
	{
		numBuff = (num / powl(10, numLength));
		numChar = numBuff + '0';
		num -= numBuff*powl(10, numLength);

		Display_drawAsciiChar(display, display->cursorStr, display->cursorCol, numChar);
		display->cursorCol++;
	}
}

void Display_print(struct DisplayGL *display, const char *format, ...)
{
	uint8_t buf[21];

	va_list arg;

	va_start(arg, format);
	//printf(buf, format, arg);
	va_end(arg);

	Display_printString(display, buf);
}

void Display_clearString(struct DisplayGL *display, uint8_t str)
{
	uint8_t i;

	for(i = 0; i < DISPLAY_WIDTH; i++)
	{
		display->displayBuffer[(str * DISPLAY_WIDTH) + i] = 0x00;
	}
}

void Display_setXbmMode(struct DisplayGL *display, DisplayXbmMode_e mode)
{
	display->xbmMode = mode;
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
				Display_drawPixel(display, xbmStartx + j, xbmStarty + i);
			}
			else if(display->xbmMode == XBM_MODE_OVERRIDE)
			{
				Display_clearPixel(display, xbmStartx + j, xbmStarty + i);
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
