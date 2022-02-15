#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/unistd.h>

#include "displ.h"

////			x = lcdDrawChar(dev, fx, x, y, ascii[i], color);
extern int lcdDrawChar(TFT7789_t* dev, FontxFile *fxs
	, uint16_t x, uint16_t y, uint8_t ascii, uint16_t color) ;

void _dput(char ch)
{

}
///=============================================
