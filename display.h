/*******************************************
 	 * File: display.h
 	 * Description: DisplayGL main header
 	 * Author: A_131
 	 * Created: 7 july 2023
 *******************************************/

/*DisplayGL uses STM32F103device library: https://github.com/AMOGUS1939/STM32f103device.git */

#ifndef DISPLAY_H
#define DISPLAY_H

#include "STMdevice.h"	/*include perephirial library that allows interface with Spi, i2c and Gpio*/

#include "stdarg.h"		/*include C standart lib*/
#include "stdlib.h"
#include "stdio.h"

#define DISPLAY_HEIGHT 64	/*display height in pixels*/
#define DISPLAY_WIDTH  128	/*displal width in pixels*/

#define DISPLAY_BUFFER_SIZE 1024 /*size of display buffer*/

/*
 * Select communication interface for display:
 *
 * HW_SPI - use hardware spi
 * I2C - use hardware i2c
 * SW_SPI - emulate spi using gpio
 */

#define DISPLAY_USE_SPI
//#define DISPLAY_USE_I2C
//#define DISPLAY_USE_SW_SPI

typedef enum
{
	COLOR_BLACK = 0,
	COLOR_WHITE
}DisplayColor_e;

typedef enum
{
	XBM_MODE_OVERRIDE = 0,
	XBM_MODE_ADD
}DisplayXbmMode_e;

/*
 * @struct DisplayGL
 * Struct that contains information about display.
 */
struct DisplayGL
{
	uint8_t csPin; 	/*Chip Select pin. Set low to communicate with display. Not used in I2C mode*/
	uint8_t dcPin; 	/*Data/Command select pin. Set low to write command, high to write data*/
	uint8_t resPin;	/*Reset pin. Set low to reset display*/

	uint8_t dataPin; 	/*Data pin. Used as MOSI in spi mode, used as SDA in I2C mode*/
	uint8_t clkPin;		/*Clk pin. Used as SCK in spi mode, used as SCL in I2C mode*/

	GPIO_TypeDef *csPinPort; /*Ports that contains CS, DC, RES pins*/
	GPIO_TypeDef *dcPinPort;
	GPIO_TypeDef *resPinPort;

	GPIO_TypeDef *dataPinPort; /*Ports that contains DATA and CLK pins*/
	GPIO_TypeDef *clkPinPort;

#if defined(DISPLAY_USE_SPI)
	SPI_TypeDef *spi;	/*Spi unit that used in Hardware Spi Mode*/
#endif

	uint8_t displayBuffer[DISPLAY_BUFFER_SIZE]; /*Array that contains display page*/

	DisplayColor_e currentDrawColor;

	int8_t cursorStr;
	int8_t cursorCol;

	DisplayXbmMode_e xbmMode; /*XBM draw mode. Override mode*/

};

void Display_init(struct DisplayGL *display);

void Display_command(struct DisplayGL *display, uint8_t command);
void Display_data(struct DisplayGL *display, uint8_t data);

void Display_sendBuffer(struct DisplayGL *display);
void Display_clearBuffer(struct DisplayGL *display);
void Display_clear(struct DisplayGL *display);

void Display_setDrawColor(struct DisplayGL *display, DisplayColor_e color);

void Display_drawPixel(struct DisplayGL *display, uint8_t x, uint8_t y);
void Display_clearPixel(struct DisplayGL *display, uint8_t x, uint8_t y);
void Display_fill(struct DisplayGL *display);
void Display_invert(struct DisplayGL *display);

void Display_drawAsciiChar(struct DisplayGL *display, uint8_t str, uint8_t col, uint8_t asciiChr);
void Display_printNum(struct DisplayGL *display, int32_t num);
void Display_setCursor(struct DisplayGL *display, uint8_t str, uint8_t col);
void Display_home(struct DisplayGL *display);
void Display_printString(struct DisplayGL *display, uint8_t str[]);
void Display_clearString(struct DisplayGL *display, uint8_t str);

void Display_drawXBM(struct DisplayGL *display, uint8_t xbmStartx, uint8_t xbmStarty, uint8_t xbmWidth, uint8_t xbmHeight, uint8_t xbm[]);
void Display_drawXBM8(struct DisplayGL *display, uint8_t xbmStartx, uint8_t xbmStarty, uint8_t xbmWidth, uint8_t xbmHeight, uint8_t xbm[]);
void Display_setXbmMode(struct DisplayGL *display, DisplayXbmMode_e mode);

void Display_print(struct DisplayGL *display, const char *format, ...);

#if defined(DISPLAY_USE_SW_SPI)
void swSpiWrite(struct DisplayGL * display, uint8_t data);
#endif

#endif
