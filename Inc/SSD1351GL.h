/* ******************************************
 	 * File: display.h
 	 * Description: SSD1351GL main header
 	 * Author: A_131
 	 * Created: 7 july 2023
 *******************************************/

/* SSD1351GL uses STM32F103device library: https://github.com/AMOGUS1939/STM32f103device.git */

#ifndef DISPLAY_H
#define DISPLAY_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "displayConfig.h"

#include "lib2f4.h"	/* include peripheral library that allows interface with Spi, i2c and Gpio */

#include "stdarg.h"		/* include C standard lib */
#include "stdlib.h"


#define COLOR_BLACK		(uint16_t)0x0000	/* Most used RGB colors */
#define COLOR_RED		(uint16_t)0xF800
#define COLOR_GREEN		(uint16_t)0x07E0
#define COLOR_BLUE		(uint16_t)0x001F
#define COLOR_YELLOW		(uint16_t)0xFFE0
#define COLOR_CYAN		(uint16_t)0x07FF
#define COLOR_MAGENTA		(uint16_t)0xF81F
#define COLOR_BROWN 		(uint16_t)0x9260
#define COLOR_WHITE		(uint16_t)0xFFFF

/*
 * @brief Struct that contains information about display
 */
struct SSD1351GL
{
	uint8_t csPin; 		/* Chip Select pin. Set low to communicate with display. Not used in I2C mode*/
	uint8_t dcPin; 		/* Data/Command select pin. Set low to write command, high to write data*/
	uint8_t resPin;		/* Reset pin. Set low to reset display*/

	uint8_t dataPin; 	/* Data pin. Used as MOSI in SPI mode, used as SDA in I2C mode*/
	uint8_t clkPin;		/* Clock pin. Used as SCK in SPI mode, used as SCL in I2C mode*/

	GPIO_TypeDef *csPinPort; /* Ports that contains CS, DC, RES pins*/
	GPIO_TypeDef *dcPinPort;
	GPIO_TypeDef *resPinPort;

	GPIO_TypeDef *dataPinPort; /* Ports that contains DATA and CLK pins*/
	GPIO_TypeDef *clkPinPort;

#if defined(DISPLAY_USE_HW_4SPI)

	SPI_TypeDef *spi;	/* SPI unit that will be used in Hardware SPI Mode*/

#endif	/* DISPLAY_USE_HW_4SPI */

	uint16_t currentDrawColor;
	uint16_t currentBackColor;

	uint8_t drawMode;

	int16_t cursorX;	/* Cursor position. Used  */
	int16_t cursorY;

};

void Display_Init(struct SSD1351GL *display);

void Display_Clear(struct SSD1351GL *display);

void Display_SetDrawColor(struct SSD1351GL *display, uint16_t color);
void Display_SetBackColor(struct SSD1351GL *display, uint16_t color);
void Display_SetDrawMode(struct SSD1351GL *display, uint8_t mode);

void Display_DrawPixel(struct SSD1351GL *display, uint8_t x, uint8_t y, uint16_t color);
void Display_ClearPixel(struct SSD1351GL *display, uint8_t x, uint8_t y);
void Display_Fill( struct SSD1351GL * display, uint16_t color );
void Display_Invert(struct SSD1351GL *display);

void Display_DrawAsciiChar(struct SSD1351GL *display, uint8_t str, uint8_t col, uint8_t asciiChr);
void Display_PrintNum(struct SSD1351GL *display, int32_t num);
void Display_SetCursor(struct SSD1351GL *display, uint8_t str, uint8_t col);
void Display_Home(struct SSD1351GL *display);
void Display_PrintString(struct SSD1351GL *display, char str[]);

void Display_DrawFrame(struct SSD1351GL *display, uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void Display_DrawBox(struct SSD1351GL *display, uint8_t x, uint8_t y, uint8_t width, uint8_t height);

void Display_DrawXBM(struct SSD1351GL *display, uint8_t xbmStartx, uint8_t xbmStarty, uint8_t xbmWidth, uint8_t xbmHeight, uint8_t xbm[]);

void Display_Printf(struct SSD1351GL *display, const char *format, ...); //todo

#if defined(__cplusplus)
}
#endif

#endif /* DISPLAY_H */
