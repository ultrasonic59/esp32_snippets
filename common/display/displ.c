#include <stdint.h>
#include "fontx.h"

////			x = lcdDrawChar(dev, fx, x, y, ascii[i], color);
extern int lcdDrawChar(TFT_t * dev, FontxFile *fxs
	, uint16_t x, uint16_t y, uint8_t ascii, uint16_t color) ;

void _dput(char ch)
{

}
///=============================================
