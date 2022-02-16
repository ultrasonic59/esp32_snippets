#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/unistd.h>

#include "displ.h"
displ7789_t cur_displ;
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
///=============================================
////	uint8_t w;
////	uint8_t h;
