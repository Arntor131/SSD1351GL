/*******************************************
 	 * File: display.h
 	 * Description: DisplayGL main header
 	 * Author: A_131
 	 * Created: 7 july 2023
 *******************************************/

/*DisplayGL uses STM32F103device library: https://github.com/AMOGUS1939/STM32f103device.git */

#ifndef DISPLAY_H
#define DISPLAY_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "displayConfig.h"

#include "STMdevice.h"	/*include peripheral library that allows interface with Spi, i2c and Gpio*/

#include "stdarg.h"		/*include C standard lib*/
#include "stdlib.h"


#define COLOR_BLACK		(uint16_t)0x0000	/* Most used RGB colors */
#define COLOR_RED		(uint16_t)0xF800
#define COLOR_GREEN		(uint16_t)0x07E0
#define COLOR_BLUE		(uint16_t)0x001F
#define COLOR_YELLOW	(uint16_t)0xFFE0
#define COLOR_CYAN		(uint16_t)0x07FF
#define COLOR_MAGENTA	(uint16_t)0xF81F
#define COLOR_BROWN 	(uint16_t)0x9260
#define COLOR_WHITE		(uint16_t)0xFFFF

/*
 * @brief Struct that contains information about display
 */
struct DisplayGL
{
	uint8_t csPin; 	/*Chip Select pin. Set low to communicate with display. Not used in I2C mode*/
	uint8_t dcPin; 	/*Data/Command select pin. Set low to write command, high to write data*/
	uint8_t resPin;	/*Reset pin. Set low to reset display*/

	uint8_t dataPin; 	/*Data pin. Used as MOSI in SPI mode, used as SDA in I2C mode*/
	uint8_t clkPin;		/*Clock pin. Used as SCK in SPI mode, used as SCL in I2C mode*/

	GPIO_TypeDef *csPinPort; /*Ports that contains CS, DC, RES pins*/
	GPIO_TypeDef *dcPinPort;
	GPIO_TypeDef *resPinPort;

	GPIO_TypeDef *dataPinPort; /*Ports that contains DATA and CLK pins*/
	GPIO_TypeDef *clkPinPort;

#if defined(DISPLAY_USE_HW_4SPI)

	SPI_TypeDef *spi;	/*SPI unit that used in Hardware SPI Mode*/

#endif	/* display use SPI */

	uint16_t currentDrawColor;
	uint16_t currentBackColor;

	uint8_t drawMode;

	int16_t cursorX;	/* Cursor position. Used  */
	int16_t cursorY;

};

void Display_init(struct DisplayGL *display);

void Display_clear(struct DisplayGL *display);

void Display_setDrawColor(struct DisplayGL *display, uint16_t color);
void Display_setBackColor(struct DisplayGL *display, uint16_t color);
void Display_setDrawMode(struct DisplayGL *display, uint8_t mode);

void Display_drawPixel(struct DisplayGL *display, uint8_t x, uint8_t y, uint16_t color);
void Display_clearPixel(struct DisplayGL *display, uint8_t x, uint8_t y);
void Display_fill( struct DisplayGL * display, uint16_t color );
void Display_invert(struct DisplayGL *display);

void Display_drawAsciiChar(struct DisplayGL *display, uint8_t str, uint8_t col, uint8_t asciiChr);
void Display_printNum(struct DisplayGL *display, int32_t num);
void Display_setCursor(struct DisplayGL *display, uint8_t str, uint8_t col);
void Display_home(struct DisplayGL *display);
void Display_printString(struct DisplayGL *display, char str[]);

void Display_drawFrame(struct DisplayGL *display, uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void Display_drawBox(struct DisplayGL *display, uint8_t x, uint8_t y, uint8_t width, uint8_t height);

void Display_drawXBM(struct DisplayGL *display, uint8_t xbmStartx, uint8_t xbmStarty, uint8_t xbmWidth, uint8_t xbmHeight, uint8_t xbm[]);

void Display_print(struct DisplayGL *display, const char *format, ...); //TODO

#if defined(DISPLAY_USE_SW_SPI)
void swSpiWrite(struct DisplayGL * display, uint8_t data);
#endif

#if defined(__cplusplus)
}
#endif

#endif
