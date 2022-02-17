#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <stdio.h>
#include <stdarg.h>

#include "displ.h"
TFT7789_t dev;

displ7789_t cur_displ;

extern int lcdDrawString(TFT_t * dev, FontxFile *fx
	, uint16_t x, uint16_t y, uint8_t * ascii, uint16_t color);

#if 0
////			x = lcdDrawChar(dev, fx, x, y, ascii[i], color);
extern int lcdDrawChar(TFT7789_t* dev, FontxFile *fxs
	, uint16_t x, uint16_t y, uint8_t ascii, uint16_t color) ;

void _dput(char ch)
{
lcdDrawChar( &cur_displ.tft,  &cur_displ.cur_font
	,cur_displ.cur_x, cur_displ.cur_y, ch, cur_displ.cur_color) ;
if(cur_displ.cur_x+ cur_displ.cur_font.w*2 < cur_displ.tft._width)
	cur_displ.cur_x += cur_displ.cur_font.w;
if(ch=='\n')
	{
	if(cur_displ.cur_y+ cur_displ.cur_font.h*2 < cur_displ.tft._height)
	cur_displ.cur_y += cur_displ.cur_font.h;
	}
}
	uint16_t cur_color;
	TFT7789_t tft;
	FontxFile cur_font;
#endif

///=============================================
void init_displ(FontxFile *font,TFT7789_t *tft,uint16_t color)
{
cur_displ.cur_y=0;
cur_displ.cur_x=0;
cur_displ.cur_color= color;
cur_displ.cur_font = font;
cur_displ.tft = tft;
}
void set_color(uint16_t color)
{
cur_displ.cur_color= color;
}


void printd(char *format, ...)
{
uint8_t buffer[256];
va_list args;
va_start (args, format);
vsnprintf ((char*)buffer, 255, format, args);
///======================================
lcdDrawString(cur_displ.tft, cur_displ.cur_font
			,cur_displ.cur_x, cur_displ.cur_y
			,buffer, cur_displ.cur_color);
cur_displ.cur_y++;
if(cur_displ.cur_y>cur_displ.tft->_width)
	cur_displ.cur_y=0;
cur_displ.cur_x++;
if(cur_displ.cur_x>cur_displ.tft->_height)
	cur_displ.cur_x=0;

///======================================
va_end (args);
}