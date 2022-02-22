#ifndef MAIN_ST7789_H_
#define MAIN_ST7789_H_

#include "driver/spi_master.h"
//// #include "fontx.h"
#include "displ.h"

#define RED				0xf800
#define GREEN			0x07e0
#define BLUE			0x001f
#define BLACK			0x0000
#define WHITE			0xffff
#define GRAY			0x8c51
#define YELLOW			0xFFE0
#define CYAN			0x07FF
#define PURPLE			0xF81F


#define DIRECTION0			0
#define DIRECTION90			1
#define DIRECTION180		2
#define DIRECTION270		3

#if 0
typedef struct {
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
} TFT_t;
#endif

#if 1
void spi_master_init(int16_t GPIO_MOSI, int16_t GPIO_SCLK, int16_t GPIO_CS, int16_t GPIO_DC, int16_t GPIO_RESET, int16_t GPIO_BL);
bool spi_master_write_byte(spi_device_handle_t SPIHandle, const uint8_t* Data, size_t DataLength);
bool spi_master_write_command( uint8_t cmd);
bool spi_master_write_data_byte( uint8_t data);
bool spi_master_write_data_word( uint16_t data);
bool spi_master_write_addr( uint16_t addr1, uint16_t addr2);
bool spi_master_write_color( uint16_t color, uint16_t size);
bool spi_master_write_colors( uint16_t * colors, uint16_t size);

void delayMS(int ms);
void lcdInit( int width, int height, int offsetx, int offsety);
void lcdDrawPixel(uint16_t x, uint16_t y, uint16_t color);
void lcdDrawMultiPixels( uint16_t x, uint16_t y, uint16_t size, uint16_t * colors);
void lcdDrawFillRect( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void lcdDisplayOff(void);
void lcdDisplayOn(void);
void lcdFillScreen( uint16_t color);
void lcdDrawLine( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void lcdDrawRect( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void lcdDrawRectAngle( uint16_t xc, uint16_t yc, uint16_t w, uint16_t h, uint16_t angle, uint16_t color);
void lcdDrawTriangle( uint16_t xc, uint16_t yc, uint16_t w, uint16_t h, uint16_t angle, uint16_t color);
void lcdDrawCircle( uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
void lcdDrawFillCircle( uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
void lcdDrawRoundRect( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t r, uint16_t color);
void lcdDrawArrow( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t w, uint16_t color);
void lcdDrawFillArrow( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t w, uint16_t color);
uint16_t rgb565_conv(uint16_t r, uint16_t g, uint16_t b);
int lcdDrawChar( FontxFile *fx, uint16_t x, uint16_t y, uint8_t ascii, uint16_t color);
int lcdDrawString( FontxFile *fx, uint16_t x, uint16_t y, uint8_t * ascii, uint16_t color);
int lcdDrawCode( FontxFile *fx, uint16_t x,uint16_t y,uint8_t code,uint16_t color);
//int lcdDrawUTF8Char( FontxFile *fx, uint16_t x, uint16_t y, uint8_t *utf8, uint16_t color);
//int lcdDrawUTF8String( FontxFile *fx, uint16_t x, uint16_t y, unsigned char *utfs, uint16_t color);
void lcdSetFontDirection( uint16_t);
void lcdSetFontFill( uint16_t color);
void lcdUnsetFontFill(void);
void lcdSetFontUnderLine( uint16_t color);
void lcdUnsetFontUnderLine(void);
void lcdBacklightOff(void);
void lcdBacklightOn(void);
void lcdInversionOff(void);
void lcdInversionOn(void);
#endif

#endif /* MAIN_ST7789_H_ */

