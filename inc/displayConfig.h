#ifndef DISPLAY_CONFIG_H_
#define DISPLAY_CONFIG_H_

/****************************************
 	 * File: displayConfig.h
 	 * Description: configuration file for DisplayGL1351
 	 * Created:  18.08.2023 21:17
 	 * Author: A_131
 ****************************************/


#define DISPLAY_HEIGHT 128 	/* display height in pixels */
#define DISPLAY_WIDTH 128	/* display width in pixels */


//#define DISPLAY_USE_BUFFER  	/* full graphic buffer for display. Requires 32k bytes of ram */

#if defined(DISPLAY_USE_FULL_BUFFER)
	#define	DISPLAY_BUFFER_SIZE 32768
#endif


/* Select the communication interface for the display */

/* Display use hardware SPI unit (4-wire mode) */
#define DISPLAY_USE_HW_4SPI

/* Display use software emulated SPI (4-wire mode) */
/* #define DISPLAY_USE_SW_4SPI */

/* Display use hardware I2C(TWI) unit */
/* #define DISPLAY_USE_I2C */


#define DISPLAY_COLOR_DEPTH 16 	/* display use 16bit rgb color */

#define RED_COLOR_BIT_DEPTH 5 	/* 5 bits for red color */
#define GREEN_COLOR_BIT_DEPTH 6	/* 6 bits for green color */
#define BLUE_COLOR_BIT_DEPTH 5	/* 5 bits for blue color */

#define DISPLAY_RAM_SIZE (uint16_t)(DISPLAY_HEIGHT * DISPLAY_WIDTH * DISPLAY_COLOR_DEPTH / 8)

#define DISPLAY_DEFAULT_BACK_COLOR COLOR_BLACK
#define DISPLAY_DEFAULT_DRAW_COLOR COLOR_WHITE

#define DISPLAY_DRAW_MODE_OVERRIDE 	(uint8_t)1
#define DISPLAY_DRAW_MODE_COMPOSE 	(uint8_t)0

#define DISPLAY_DEFAULT_DRAW_MODE 	DISPLAY_DRAW_MODE_OVERRIDE

#define DISPLAY_FONT_HEIGHT 8
#define DISPLAY_FONT_WIDTH 5

#endif
