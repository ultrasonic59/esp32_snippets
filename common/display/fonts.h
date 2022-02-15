#ifndef _FONTS_H_
#define _FONTS_H_
////#define FontxGlyphBufSize (32*32/8)

typedef struct {
	const char *path;
	char  fxname[10];
	bool  opened;
	bool  valid;
	bool  is_ank;
	uint8_t w;
	uint8_t h;
	uint16_t fsz;
	uint8_t bc;
	FILE *file;
} FontxFile;


#endif /* _FONTS_H_ */

