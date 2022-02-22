#ifndef _DISPL_H_
#define _DISPL_H_

#include "fonts.h"
#include "driver/spi_master.h"

typedef struct tft7789_s_{
	uint16_t _width;
	uint16_t _height;
	uint16_t _offsetx;
	uint16_t _offsety;
	uint16_t _font_direction;
	uint16_t _font_fill;
	uint16_t _font_fill_color;
	uint16_t _font_underline;
	uint16_t _font_underline_color;
	int16_t _dc;
	int16_t _bl;
	spi_device_handle_t _SPIHandle;
} TFT7789_t;
typedef struct displ7789_s_{
	uint16_t cur_x;
	uint16_t cur_y;
	uint16_t cur_color;
	TFT7789_t *tft;
	FontxFile *cur_font;
} displ7789_t;

////#define  TFT_t TFT7789_t
////extern TFT7789_t dev;
extern TFT7789_t dev;
extern void printd(char * format, ...);
extern void init_displ(FontxFile *font,TFT7789_t *tft
					,uint16_t color);
extern void set_color(uint16_t color);

///===============================================
#endif
