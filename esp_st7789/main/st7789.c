#include <string.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <driver/spi_master.h>
#include <driver/gpio.h>
#include "esp_log.h"

#include "st7789.h"

#define TAG "ST7789"
#define	_DEBUG_ 1

#ifdef CONFIG_IDF_TARGET_ESP32
#define LCD_HOST    HSPI_HOST
#elif defined CONFIG_IDF_TARGET_ESP32S2
#define LCD_HOST    SPI2_HOST
#elif defined CONFIG_IDF_TARGET_ESP32C3
#define LCD_HOST    SPI2_HOST
#endif

static const int SPI_Command_Mode = 0;
static const int SPI_Data_Mode = 1;
////static const int SPI_Frequency = SPI_MASTER_FREQ_20M;
//static const int SPI_Frequency = SPI_MASTER_FREQ_26M;
static const int SPI_Frequency = SPI_MASTER_FREQ_40M;
///static const int SPI_Frequency = SPI_MASTER_FREQ_80M;
TFT7789_t dev_st7789;

void spi_master_init(int16_t GPIO_MOSI, int16_t GPIO_SCLK, int16_t GPIO_CS, int16_t GPIO_DC, int16_t GPIO_RESET, int16_t GPIO_BL)
{
	esp_err_t ret;
TFT7789_t *pdev = &dev_st7789;
	ESP_LOGI(TAG, "GPIO_CS=%d",GPIO_CS);
	if ( GPIO_CS >= 0 ) {
		//gpio_pad_select_gpio( GPIO_CS );
		gpio_reset_pin( GPIO_CS );
		gpio_set_direction( GPIO_CS, GPIO_MODE_OUTPUT );
		gpio_set_level( GPIO_CS, 0 );
	}

	ESP_LOGI(TAG, "GPIO_DC=%d",GPIO_DC);
	//gpio_pad_select_gpio( GPIO_DC );
	gpio_reset_pin( GPIO_DC );
	gpio_set_direction( GPIO_DC, GPIO_MODE_OUTPUT );
	gpio_set_level( GPIO_DC, 0 );

	ESP_LOGI(TAG, "GPIO_RESET=%d",GPIO_RESET);
	if ( GPIO_RESET >= 0 ) {
		//gpio_pad_select_gpio( GPIO_RESET );
		gpio_reset_pin( GPIO_RESET );
		gpio_set_direction( GPIO_RESET, GPIO_MODE_OUTPUT );
		gpio_set_level( GPIO_RESET, 1 );
		delayMS(50);
		gpio_set_level( GPIO_RESET, 0 );
		delayMS(50);
		gpio_set_level( GPIO_RESET, 1 );
		delayMS(50);
	}

	ESP_LOGI(TAG, "GPIO_BL=%d",GPIO_BL);
	if ( GPIO_BL >= 0 ) {
		//gpio_pad_select_gpio(GPIO_BL);
		gpio_reset_pin(GPIO_BL);
		gpio_set_direction( GPIO_BL, GPIO_MODE_OUTPUT );
		gpio_set_level( GPIO_BL, 0 );
	}

	ESP_LOGI(TAG, "GPIO_MOSI=%d",GPIO_MOSI);
	ESP_LOGI(TAG, "GPIO_SCLK=%d",GPIO_SCLK);
	spi_bus_config_t buscfg = {
		.mosi_io_num = GPIO_MOSI,
		.miso_io_num = -1,
		.sclk_io_num = GPIO_SCLK,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1,
		.max_transfer_sz = 0,
		.flags = 0
	};

	ret = spi_bus_initialize( LCD_HOST, &buscfg, SPI_DMA_CH_AUTO );
	ESP_LOGD(TAG, "spi_bus_initialize=%d",ret);
	assert(ret==ESP_OK);

	spi_device_interface_config_t devcfg;
	memset(&devcfg, 0, sizeof(devcfg));
	devcfg.clock_speed_hz = SPI_Frequency;
	devcfg.queue_size = 7;
	devcfg.mode = 2;
	devcfg.flags = SPI_DEVICE_NO_DUMMY;

	if ( GPIO_CS >= 0 ) {
		devcfg.spics_io_num = GPIO_CS;
	} else {
		devcfg.spics_io_num = -1;
	}
	
	spi_device_handle_t handle;
	ret = spi_bus_add_device( LCD_HOST, &devcfg, &handle);
	ESP_LOGD(TAG, "spi_bus_add_device=%d",ret);
	assert(ret==ESP_OK);
pdev->_dc = GPIO_DC;
pdev->_bl = GPIO_BL;
pdev->_SPIHandle = handle;
}


bool spi_master_write_byte(spi_device_handle_t SPIHandle, const uint8_t* Data, size_t DataLength)
{
	spi_transaction_t SPITransaction;
	esp_err_t ret;

	if ( DataLength > 0 ) {
		memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );
		SPITransaction.length = DataLength * 8;
		SPITransaction.tx_buffer = Data;
#if 1
		ret = spi_device_transmit( SPIHandle, &SPITransaction );
#else
		ret = spi_device_polling_transmit( SPIHandle, &SPITransaction );
#endif
		assert(ret==ESP_OK); 
	}

	return true;
}

bool spi_master_write_command(uint8_t cmd)
{
TFT7789_t *pdev = &dev_st7789;
	static uint8_t Byte = 0;
	Byte = cmd;
	gpio_set_level( pdev->_dc, SPI_Command_Mode );
	return spi_master_write_byte( pdev->_SPIHandle, &Byte, 1 );
}

bool spi_master_write_data_byte( uint8_t data)
{
TFT7789_t *pdev = &dev_st7789;
	static uint8_t Byte = 0;
	Byte = data;
	gpio_set_level( pdev->_dc, SPI_Data_Mode );
	return spi_master_write_byte( pdev->_SPIHandle, &Byte, 1 );
}


bool spi_master_write_data_word( uint16_t data)
{
TFT7789_t *pdev = &dev_st7789;
	static uint8_t Byte[2];
	Byte[0] = (data >> 8) & 0xFF;
	Byte[1] = data & 0xFF;
	gpio_set_level( pdev->_dc, SPI_Data_Mode );
	return spi_master_write_byte( pdev->_SPIHandle, Byte, 2);
}

bool spi_master_write_addr( uint16_t addr1, uint16_t addr2)
{
TFT7789_t *pdev = &dev_st7789;
	static uint8_t Byte[4];
	Byte[0] = (addr1 >> 8) & 0xFF;
	Byte[1] = addr1 & 0xFF;
	Byte[2] = (addr2 >> 8) & 0xFF;
	Byte[3] = addr2 & 0xFF;
	gpio_set_level( pdev->_dc, SPI_Data_Mode );
	return spi_master_write_byte( pdev->_SPIHandle, Byte, 4);
}

bool spi_master_write_color(uint16_t color, uint16_t size)
{
TFT7789_t *pdev = &dev_st7789;
	static uint8_t Byte[1024];
	int index = 0;
	for(int i=0;i<size;i++) {
		Byte[index++] = (color >> 8) & 0xFF;
		Byte[index++] = color & 0xFF;
	}
	gpio_set_level( pdev->_dc, SPI_Data_Mode );
	return spi_master_write_byte( pdev->_SPIHandle, Byte, size*2);
}

// Add 202001
bool spi_master_write_colors(uint16_t * colors, uint16_t size)
{
TFT7789_t *pdev = &dev_st7789;
	static uint8_t Byte[1024];
	int index = 0;
	for(int i=0;i<size;i++) {
		Byte[index++] = (colors[i] >> 8) & 0xFF;
		Byte[index++] = colors[i] & 0xFF;
	}
	gpio_set_level( pdev->_dc, SPI_Data_Mode );
	return spi_master_write_byte( pdev->_SPIHandle, Byte, size*2);
}

void delayMS(int ms) {
	int _ms = ms + (portTICK_PERIOD_MS - 1);
	TickType_t xTicksToDelay = _ms / portTICK_PERIOD_MS;
	ESP_LOGD(TAG, "ms=%d _ms=%d portTICK_PERIOD_MS=%d xTicksToDelay=%d",ms,_ms,portTICK_PERIOD_MS,xTicksToDelay);
	vTaskDelay(xTicksToDelay);
}


void lcdInit(int width, int height, int offsetx, int offsety)
{
TFT7789_t *pdev = &dev_st7789;

	pdev->_width = width;
	pdev->_height = height;
	pdev->_offsetx = offsetx;
	pdev->_offsety = offsety;
	pdev->_font_direction = DIRECTION0;
	pdev->_font_fill = false;
	pdev->_font_underline = false;

	spi_master_write_command( 0x01);	//Software Reset
	delayMS(150);

	spi_master_write_command( 0x11);	//Sleep Out
	delayMS(255);
	
	spi_master_write_command( 0x3A);	//Interface Pixel Format
	spi_master_write_data_byte( 0x55);
	delayMS(10);
	
	spi_master_write_command( 0x36);	//Memory Data Access Control
	spi_master_write_data_byte( 0x00);

	spi_master_write_command( 0x2A);	//Column Address Set
	spi_master_write_data_byte( 0x00);
	spi_master_write_data_byte( 0x00);
	spi_master_write_data_byte( 0x00);
	spi_master_write_data_byte( 0xF0);

	spi_master_write_command( 0x2B);	//Row Address Set
	spi_master_write_data_byte( 0x00);
	spi_master_write_data_byte( 0x00);
	spi_master_write_data_byte( 0x00);
	spi_master_write_data_byte( 0xF0);

	spi_master_write_command( 0x21);	//Display Inversion On
	delayMS(10);

	spi_master_write_command( 0x13);	//Normal Display Mode On
	delayMS(10);

	spi_master_write_command( 0x29);	//Display ON
	delayMS(255);

	if(pdev->_bl >= 0) {
		gpio_set_level( pdev->_bl, 1 );
	}
}


// Draw pixel
// x:X coordinate
// y:Y coordinate
// color:color
void lcdDrawPixel(uint16_t x, uint16_t y, uint16_t color){
TFT7789_t *pdev = &dev_st7789;

	if (x >= pdev->_width)
		return;
	if (y >= pdev->_height)
		return;

	uint16_t _x = x + pdev->_offsetx;
	uint16_t _y = y + pdev->_offsety;

	spi_master_write_command( 0x2A);	// set column(x) address
	spi_master_write_addr( _x, _x);
	spi_master_write_command( 0x2B);	// set Page(y) address
	spi_master_write_addr( _y, _y);
	spi_master_write_command( 0x2C);	//	Memory Write
	spi_master_write_data_word( color);
}


// Draw multi pixel
// x:X coordinate
// y:Y coordinate
// size:Number of colors
// colors:colors
void lcdDrawMultiPixels(uint16_t x, uint16_t y, uint16_t size, uint16_t * colors) {
TFT7789_t *pdev = &dev_st7789;

	if (x+size > pdev->_width)
		return;
	if (y >= pdev->_height)
		return;

	uint16_t _x1 = x + pdev->_offsetx;
	uint16_t _x2 = _x1 + size;
	uint16_t _y1 = y + pdev->_offsety;
	uint16_t _y2 = _y1;

	spi_master_write_command( 0x2A);	// set column(x) address
	spi_master_write_addr( _x1, _x2);
	spi_master_write_command( 0x2B);	// set Page(y) address
	spi_master_write_addr( _y1, _y2);
	spi_master_write_command( 0x2C);	//	Memory Write
	spi_master_write_colors( colors, size);
}

// Draw rectangle of filling
// x1:Start X coordinate
// y1:Start Y coordinate
// x2:End X coordinate
// y2:End Y coordinate
// color:color
void lcdDrawFillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
TFT7789_t *pdev = &dev_st7789;
	if (x1 >= pdev->_width)
		return;
	if (x2 >= pdev->_width) x2=pdev->_width-1;
	if (y1 >= pdev->_height)
		return;
	if (y2 >= pdev->_height) y2=pdev->_height-1;

	ESP_LOGD(TAG,"offset(x)=%d offset(y)=%d",pdev->_offsetx,pdev->_offsety);
	uint16_t _x1 = x1 + pdev->_offsetx;
	uint16_t _x2 = x2 + pdev->_offsetx;
	uint16_t _y1 = y1 + pdev->_offsety;
	uint16_t _y2 = y2 + pdev->_offsety;

	spi_master_write_command( 0x2A);	// set column(x) address
	spi_master_write_addr( _x1, _x2);
	spi_master_write_command( 0x2B);	// set Page(y) address
	spi_master_write_addr( _y1, _y2);
	spi_master_write_command( 0x2C);	//	Memory Write
	for(int i=_x1;i<=_x2;i++){
		uint16_t size = _y2-_y1+1;
		spi_master_write_color( color, size);
#if 0
		for(j=y1;j<=y2;j++){
			//ESP_LOGD(TAG,"i=%d j=%d",i,j);
			spi_master_write_data_word(dev, color);
		}
#endif
	}
}

// Display OFF
void lcdDisplayOff(void) {
///TFT7789_t *pdev = &dev_st7789;

	spi_master_write_command( 0x28);	//Display off
}
 
// Display ON
void lcdDisplayOn(void) {
///	TFT7789_t *pdev = &dev_st7789;
	spi_master_write_command( 0x29);	//Display on
}

// Fill screen
// color:color
void lcdFillScreen(uint16_t color) {
	TFT7789_t *pdev = &dev_st7789;
	lcdDrawFillRect( 0, 0, pdev->_width-1, pdev->_height-1, color);
}

// Draw line
// x1:Start X coordinate
// y1:Start Y coordinate
// x2:End X coordinate
// y2:End Y coordinate
// color:color 
void lcdDrawLine( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {

	int i;
	int dx,dy;
	int sx,sy;
	int E;

	/* distance between two points */
	dx = ( x2 > x1 ) ? x2 - x1 : x1 - x2;
	dy = ( y2 > y1 ) ? y2 - y1 : y1 - y2;

	/* direction of two point */
	sx = ( x2 > x1 ) ? 1 : -1;
	sy = ( y2 > y1 ) ? 1 : -1;

	/* inclination < 1 */
	if ( dx > dy ) {
		E = -dx;
		for ( i = 0 ; i <= dx ; i++ ) {
			lcdDrawPixel( x1, y1, color);
			x1 += sx;
			E += 2 * dy;
			if ( E >= 0 ) {
			y1 += sy;
			E -= 2 * dx;
		}
	}

	/* inclination >= 1 */
	} else {
		E = -dy;
		for ( i = 0 ; i <= dy ; i++ ) {
			lcdDrawPixel( x1, y1, color);
			y1 += sy;
			E += 2 * dx;
			if ( E >= 0 ) {
				x1 += sx;
				E -= 2 * dy;
			}
		}
	}
}
#define MIN_Y 	0
#define MIN_X 	0
#define MAX_Y 	239
#define MAX_X 	134
#define GRD_SIZE_X 		33
#define GRD_SIZE_Y 		33
#define GRD_DOT_DELTA 	6
#define GRD_MAX_X 	(33*4)
#define GRD_MAX_Y 	(33*6)
#define OSC_MAX_X GRD_MAX_X
#define OSC_MAX_Y GRD_MAX_Y
#define X_SIZE OSC_MAX_Y

///=======================================================================
void Display_Grid(uint16_t color)
{
  unsigned short i,j;
  for(j=MIN_Y; j<=GRD_MAX_Y; j+=GRD_SIZE_Y){
    for(i=MIN_X; i<GRD_MAX_X+1; i+=GRD_DOT_DELTA){
		lcdDrawPixel( i, j, color);
    }
  }
  for(i=MIN_X;i<=GRD_MAX_X; i+=GRD_SIZE_X){
    for(j=MIN_Y; j<=GRD_MAX_Y; j+=GRD_DOT_DELTA){
		lcdDrawPixel( i, j, color);
    }
  }
}
///=======================================================================
void draw_seg(uint16_t i, uint16_t y1, uint16_t y2, uint16_t color)
{
///uint16_t j;
uint16_t Tmp;

Tmp=y2;
if(y1>y2) {
    y2=y1;
    y1=Tmp;
  }
  if(y2>=MAX_Y)
	  y2=MAX_Y-1;
  if(y1<=MIN_Y)
	  y1=MIN_Y+1;
 //// for(j=y1; j<=y2; ++j)
////	  __Erase_Color(i+MIN_X, j, Color);
	lcdDrawLine( i+MIN_X, y1, i+MIN_X, y2, color);
}
uint8_t view_buffer[256];
uint8_t signal_buffer[256];

void draw_wave(uint16_t color)
{
uint16_t i;
uint8_t y1;
uint8_t y2;

y1=view_buffer[0];

  for(i=0; i<X_SIZE; ++i){
    y2=view_buffer[i];
    draw_seg(i, y1, y2, color);
    y1 = y2;
  }
}


#if 0
void draw_wave(uint16_t color)
{
uint16_t i;
uint8_t y1, y2, y3, y4;

  y1=View_Buffer[0];
  y3= Ref_Buffer[0];

  for(i=0; i<X_SIZE; ++i){
    y2=View_Buffer[i];
    y4= Ref_Buffer[i];
    Erase_SEG(i, y1, y2, CH1_COLOR);
    Erase_SEG(i, y3, y4, CH3_COLOR);
    y1 = y2;
    y3 = y4;
  }
}
#endif
///=======================================================================
// Draw rectangle
// x1:Start X coordinate
// y1:Start Y coordinate
// x2:End	X coordinate
// y2:End	Y coordinate
// color:color
void lcdDrawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {

	lcdDrawLine( x1, y1, x2, y1, color);
	lcdDrawLine( x2, y1, x2, y2, color);
	lcdDrawLine( x2, y2, x1, y2, color);
	lcdDrawLine( x1, y2, x1, y1, color);
}

// Draw rectangle with angle
// xc:Center X coordinate
// yc:Center Y coordinate
// w:Width of rectangle
// h:Height of rectangle
// angle :Angle of rectangle
// color :color

//When the origin is (0, 0), the point (x1, y1) after rotating the point (x, y) by the angle is obtained by the following calculation.
// x1 = x * cos(angle) - y * sin(angle)
// y1 = x * sin(angle) + y * cos(angle)
void lcdDrawRectAngle(uint16_t xc, uint16_t yc, uint16_t w, uint16_t h, uint16_t angle, uint16_t color) {

	double xd,yd,rd;
	int x1,y1;
	int x2,y2;
	int x3,y3;
	int x4,y4;
	rd = -angle * M_PI / 180.0;
	xd = 0.0 - w/2;
	yd = h/2;
	x1 = (int)(xd * cos(rd) - yd * sin(rd) + xc);
	y1 = (int)(xd * sin(rd) + yd * cos(rd) + yc);

	yd = 0.0 - yd;
	x2 = (int)(xd * cos(rd) - yd * sin(rd) + xc);
	y2 = (int)(xd * sin(rd) + yd * cos(rd) + yc);

	xd = w/2;
	yd = h/2;
	x3 = (int)(xd * cos(rd) - yd * sin(rd) + xc);
	y3 = (int)(xd * sin(rd) + yd * cos(rd) + yc);

	yd = 0.0 - yd;
	x4 = (int)(xd * cos(rd) - yd * sin(rd) + xc);
	y4 = (int)(xd * sin(rd) + yd * cos(rd) + yc);

	lcdDrawLine( x1, y1, x2, y2, color);
	lcdDrawLine( x1, y1, x3, y3, color);
	lcdDrawLine( x2, y2, x4, y4, color);
	lcdDrawLine( x3, y3, x4, y4, color);
}

// Draw triangle
// xc:Center X coordinate
// yc:Center Y coordinate
// w:Width of triangle
// h:Height of triangle
// angle :Angle of triangle
// color :color

//When the origin is (0, 0), the point (x1, y1) after rotating the point (x, y) by the angle is obtained by the following calculation.
// x1 = x * cos(angle) - y * sin(angle)
// y1 = x * sin(angle) + y * cos(angle)
void lcdDrawTriangle(uint16_t xc, uint16_t yc, uint16_t w, uint16_t h, uint16_t angle, uint16_t color) {

	double xd,yd,rd;
	int x1,y1;
	int x2,y2;
	int x3,y3;
	rd = -angle * M_PI / 180.0;
	xd = 0.0;
	yd = h/2;
	x1 = (int)(xd * cos(rd) - yd * sin(rd) + xc);
	y1 = (int)(xd * sin(rd) + yd * cos(rd) + yc);

	xd = w/2;
	yd = 0.0 - yd;
	x2 = (int)(xd * cos(rd) - yd * sin(rd) + xc);
	y2 = (int)(xd * sin(rd) + yd * cos(rd) + yc);

	xd = 0.0 - w/2;
	x3 = (int)(xd * cos(rd) - yd * sin(rd) + xc);
	y3 = (int)(xd * sin(rd) + yd * cos(rd) + yc);

	lcdDrawLine( x1, y1, x2, y2, color);
	lcdDrawLine( x1, y1, x3, y3, color);
	lcdDrawLine( x2, y2, x3, y3, color);
}

// Draw circle
// x0:Central X coordinate
// y0:Central Y coordinate
// r:radius
// color:color
void lcdDrawCircle( uint16_t x0, uint16_t y0, uint16_t r, uint16_t color) {

	int x;
	int y;
	int err;
	int old_err;

	x=0;
	y=-r;
	err=2-2*r;
	do{
		lcdDrawPixel( x0-x, y0+y, color);
		lcdDrawPixel( x0-y, y0-x, color);
		lcdDrawPixel( x0+x, y0-y, color);
		lcdDrawPixel( x0+y, y0+x, color);
		if ((old_err=err)<=x)
			err+=++x*2+1;
		if (old_err>y || err>x)
			err+=++y*2+1;
	} while(y<0);
}

// Draw circle of filling
// x0:Central X coordinate
// y0:Central Y coordinate
// r:radius
// color:color
void lcdDrawFillCircle( uint16_t x0, uint16_t y0, uint16_t r, uint16_t color) {
	int x;
	int y;
	int err;
	int old_err;
	int ChangeX;

	x=0;
	y=-r;
	err=2-2*r;
	ChangeX=1;
	do{
		if(ChangeX) {
			lcdDrawLine( x0-x, y0-y, x0-x, y0+y, color);
			lcdDrawLine( x0+x, y0-y, x0+x, y0+y, color);
		} // endif
		ChangeX=(old_err=err)<=x;
		if (ChangeX)
			err+=++x*2+1;
		if (old_err>y || err>x)
			err+=++y*2+1;
	} while(y<=0);
} 

// Draw rectangle with round corner
// x1:Start X coordinate
// y1:Start Y coordinate
// x2:End	X coordinate
// y2:End	Y coordinate
// r:radius
// color:color
void lcdDrawRoundRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t r, uint16_t color) {

	int x;
	int y;
	int err;
	int old_err;
	unsigned char temp;

	if(x1>x2) {
		temp=x1; x1=x2; x2=temp;
	} // endif
	  
	if(y1>y2) {
		temp=y1; y1=y2; y2=temp;
	} // endif

	ESP_LOGD(TAG, "x1=%d x2=%d delta=%d r=%d",x1, x2, x2-x1, r);
	ESP_LOGD(TAG, "y1=%d y2=%d delta=%d r=%d",y1, y2, y2-y1, r);
	if (x2-x1 < r)
		return; // Add 20190517
	if (y2-y1 < r)
		return; // Add 20190517

	x=0;
	y=-r;
	err=2-2*r;

	do{
		if(x) {
			lcdDrawPixel( x1+r-x, y1+r+y, color);
			lcdDrawPixel( x2-r+x, y1+r+y, color);
			lcdDrawPixel( x1+r-x, y2-r-y, color);
			lcdDrawPixel( x2-r+x, y2-r-y, color);
		} // endif 
		if ((old_err=err)<=x)
			err+=++x*2+1;
		if (old_err>y || err>x)
			err+=++y*2+1;
	} while(y<0);

	ESP_LOGD(TAG, "x1+r=%d x2-r=%d",x1+r, x2-r);
	lcdDrawLine( x1+r,y1  ,x2-r,y1	,color);
	lcdDrawLine( x1+r,y2  ,x2-r,y2	,color);
	ESP_LOGD(TAG, "y1+r=%d y2-r=%d",y1+r, y2-r);
	lcdDrawLine( x1  ,y1+r,x1  ,y2-r,color);
	lcdDrawLine( x2  ,y1+r,x2  ,y2-r,color);
} 

// Draw arrow
// x1:Start X coordinate
// y1:Start Y coordinate
// x2:End	X coordinate
// y2:End	Y coordinate
// w:Width of the botom
// color:color
// Thanks http://k-hiura.cocolog-nifty.com/blog/2010/11/post-2a62.html
void lcdDrawArrow( uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t w,uint16_t color) {

	double Vx= x1 - x0;
	double Vy= y1 - y0;
	double v = sqrt(Vx*Vx+Vy*Vy);
	//	 printf("v=%f\n",v);
	double Ux= Vx/v;
	double Uy= Vy/v;

	uint16_t L[2],R[2];
	L[0]= x1 - Uy*w - Ux*v;
	L[1]= y1 + Ux*w - Uy*v;
	R[0]= x1 + Uy*w - Ux*v;
	R[1]= y1 - Ux*w - Uy*v;
	//printf("L=%d-%d R=%d-%d\n",L[0],L[1],R[0],R[1]);

	//lcdDrawLine(x0,y0,x1,y1,color);
	lcdDrawLine( x1, y1, L[0], L[1], color);
	lcdDrawLine( x1, y1, R[0], R[1], color);
	lcdDrawLine( L[0], L[1], R[0], R[1], color);
}


// Draw arrow of filling
// x1:Start X coordinate
// y1:Start Y coordinate
// x2:End	X coordinate
// y2:End	Y coordinate
// w:Width of the botom
// color:color
void lcdDrawFillArrow( uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t w,uint16_t color) {

	double Vx= x1 - x0;
	double Vy= y1 - y0;
	double v = sqrt(Vx*Vx+Vy*Vy);
	//printf("v=%f\n",v);
	double Ux= Vx/v;
	double Uy= Vy/v;

	uint16_t L[2],R[2];
	L[0]= x1 - Uy*w - Ux*v;
	L[1]= y1 + Ux*w - Uy*v;
	R[0]= x1 + Uy*w - Ux*v;
	R[1]= y1 - Ux*w - Uy*v;
	//printf("L=%d-%d R=%d-%d\n",L[0],L[1],R[0],R[1]);

	lcdDrawLine( x0, y0, x1, y1, color);
	lcdDrawLine( x1, y1, L[0], L[1], color);
	lcdDrawLine( x1, y1, R[0], R[1], color);
	lcdDrawLine( L[0], L[1], R[0], R[1], color);

	int ww;
	for(ww=w-1;ww>0;ww--) {
		L[0]= x1 - Uy*ww - Ux*v;
		L[1]= y1 + Ux*ww - Uy*v;
		R[0]= x1 + Uy*ww - Ux*v;
		R[1]= y1 - Ux*ww - Uy*v;
		//printf("Fill>L=%d-%d R=%d-%d\n",L[0],L[1],R[0],R[1]);
		lcdDrawLine( x1, y1, L[0], L[1], color);
		lcdDrawLine( x1, y1, R[0], R[1], color);
	}
}


// RGB565 conversion
// RGB565 is R(5)+G(6)+B(5)=16bit color format.
// Bit image "RRRRRGGGGGGBBBBB"
uint16_t rgb565_conv(uint16_t r,uint16_t g,uint16_t b) {
	return (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

// Draw ASCII character
// x:X coordinate
// y:Y coordinate
// ascii: ascii code
// color:color
int lcdDrawChar(FontxFile *fxs, uint16_t x, uint16_t y, uint8_t ascii, uint16_t color) {
	TFT7789_t *pdev = &dev_st7789;

	uint16_t xx,yy,bit,ofs;
	unsigned char fonts[128]; // font pattern
	unsigned char pw, ph;
	int h,w;
	uint16_t mask;
	bool rc;

	if(_DEBUG_)
		printf("_font_direction=%d\n",pdev->_font_direction);
	rc = GetFontx(fxs, ascii, fonts, &pw, &ph);
	if(_DEBUG_)
		printf("GetFontx rc=%d pw=%d ph=%d\n",rc,pw,ph);
	if (!rc)
		return 0;

	int16_t xd1 = 0;
	int16_t yd1 = 0;
	int16_t xd2 = 0;
	int16_t yd2 = 0;
	uint16_t xss = 0;
	uint16_t yss = 0;
	int16_t xsd = 0;
	int16_t ysd = 0;
	int16_t next = 0;
	uint16_t x0  = 0;
	uint16_t x1  = 0;
	uint16_t y0  = 0;
	uint16_t y1  = 0;
	if (pdev->_font_direction == 0) {
		xd1 = +1;
		yd1 = +1; //-1;
		xd2 =  0;
		yd2 =  0;
		xss =  x;
		yss =  y - (ph - 1);
		xsd =  1;
		ysd =  0;
		next = x + pw;

		x0	= x;
		y0	= y - (ph-1);
		x1	= x + (pw-1);
		y1	= y;
	} else if (pdev->_font_direction == 2) {
		xd1 = -1;
		yd1 = -1; //+1;
		xd2 =  0;
		yd2 =  0;
		xss =  x;
		yss =  y + ph + 1;
		xsd =  1;
		ysd =  0;
		next = x - pw;

		x0	= x - (pw-1);
		y0	= y;
		x1	= x;
		y1	= y + (ph-1);
	} else if (pdev->_font_direction == 1) {
		xd1 =  0;
		yd1 =  0;
		xd2 = -1;
		yd2 = +1; //-1;
		xss =  x + ph;
		yss =  y;
		xsd =  0;
		ysd =  1;
		next = y + pw; //y - pw;

		x0	= x;
		y0	= y;
		x1	= x + (ph-1);
		y1	= y + (pw-1);
	} else if (pdev->_font_direction == 3) {
		xd1 =  0;
		yd1 =  0;
		xd2 = +1;
		yd2 = -1; //+1;
		xss =  x - (ph - 1);
		yss =  y;
		xsd =  0;
		ysd =  1;
		next = y - pw; //y + pw;

		x0	= x - (ph-1);
		y0	= y - (pw-1);
		x1	= x;
		y1	= y;
	}

	if (pdev->_font_fill)
		lcdDrawFillRect( x0, y0, x1, y1, pdev->_font_fill_color);

	int bits;
	if(_DEBUG_)
		printf("xss=%d yss=%d\n",xss,yss);
	ofs = 0;
	yy = yss;
	xx = xss;
	for(h=0;h<ph;h++) {
		if(xsd) xx = xss;
		if(ysd) yy = yss;
		//for(w=0;w<(pw/8);w++) {
		bits = pw;
		for(w=0;w<((pw+4)/8);w++) {
			mask = 0x80;
			for(bit=0;bit<8;bit++) {
				bits--;
				if (bits < 0) continue;
				//if(_DEBUG_)printf("xx=%d yy=%d mask=%02x fonts[%d]=%02x\n",xx,yy,mask,ofs,fonts[ofs]);
				if (fonts[ofs] & mask) {
					lcdDrawPixel( xx, yy, color);
				} else {
					//if (dev->_font_fill) lcdDrawPixel(dev, xx, yy, dev->_font_fill_color);
				}
				if (h == (ph-2) && pdev->_font_underline)
					lcdDrawPixel( xx, yy, pdev->_font_underline_color);
				if (h == (ph-1) && pdev->_font_underline)
					lcdDrawPixel( xx, yy, pdev->_font_underline_color);
				xx = xx + xd1;
				yy = yy + yd2;
				mask = mask >> 1;
			}
			ofs++;
		}
		yy = yy + yd1;
		xx = xx + xd2;
	}

	if (next < 0) next = 0;
	return next;
}

int lcdDrawString( FontxFile *fx, uint16_t x, uint16_t y, uint8_t * ascii, uint16_t color) {
	TFT7789_t *pdev = &dev_st7789;

	int length = strlen((char *)ascii);
	if(_DEBUG_)
		printf("lcdDrawString length=%d\n",length);
	for(int i=0;i<length;i++) {
		if(_DEBUG_)
			printf("ascii[%d]=%x x=%d y=%d\n",i,ascii[i],x,y);
		if (pdev->_font_direction == 0)
			x = lcdDrawChar( fx, x, y, ascii[i], color);
		if (pdev->_font_direction == 1)
			y = lcdDrawChar( fx, x, y, ascii[i], color);
		if (pdev->_font_direction == 2)
			x = lcdDrawChar( fx, x, y, ascii[i], color);
		if (pdev->_font_direction == 3)
			y = lcdDrawChar( fx, x, y, ascii[i], color);
	}
	if (pdev->_font_direction == 0)
		return x;
	if (pdev->_font_direction == 2)
		return x;
	if (pdev->_font_direction == 1)
		return y;
	if (pdev->_font_direction == 3)
		return y;
	return 0;
}


// Draw Non-Alphanumeric character
// x:X coordinate
// y:Y coordinate
// code: charcter code
// color:color
int lcdDrawCode( FontxFile *fx, uint16_t x,uint16_t y,uint8_t code,uint16_t color) {
	TFT7789_t *pdev = &dev_st7789;

	if(_DEBUG_)
		printf("code=%x x=%d y=%d\n",code,x,y);
	if (pdev->_font_direction == 0)
		x = lcdDrawChar( fx, x, y, code, color);
	if (pdev->_font_direction == 1)
		y = lcdDrawChar( fx, x, y, code, color);
	if (pdev->_font_direction == 2)
		x = lcdDrawChar( fx, x, y, code, color);
	if (pdev->_font_direction == 3)
		y = lcdDrawChar( fx, x, y, code, color);
	if (pdev->_font_direction == 0)
		return x;
	if (pdev->_font_direction == 2)
		return x;
	if (pdev->_font_direction == 1)
		return y;
	if (pdev->_font_direction == 3)
		return y;
	return 0;
}

#if 0
// Draw UTF8 character
// x:X coordinate
// y:Y coordinate
// utf8: UTF8 code
// color:color
int lcdDrawUTF8Char(TFT_t * dev, FontxFile *fx, uint16_t x,uint16_t y,uint8_t *utf8,uint16_t color) {
	uint16_t sjis[1];

	sjis[0] = UTF2SJIS(utf8);
	if(_DEBUG_)printf("sjis=%04x\n",sjis[0]);
	return lcdDrawSJISChar(dev, fx, x, y, sjis[0], color);
}

// Draw UTF8 string
// x:X coordinate
// y:Y coordinate
// utfs: UTF8 string
// color:color
int lcdDrawUTF8String(TFT_t * dev, FontxFile *fx, uint16_t x, uint16_t y, unsigned char *utfs, uint16_t color) {

	int i;
	int spos;
	uint16_t sjis[64];
	spos = String2SJIS(utfs, strlen((char *)utfs), sjis, 64);
	if(_DEBUG_)printf("spos=%d\n",spos);
	for(i=0;i<spos;i++) {
		if(_DEBUG_)printf("sjis[%d]=%x y=%d\n",i,sjis[i],y);
		if (dev->_font_direction == 0)
			x = lcdDrawSJISChar(dev, fx, x, y, sjis[i], color);
		if (dev->_font_direction == 1)
			y = lcdDrawSJISChar(dev, fx, x, y, sjis[i], color);
		if (dev->_font_direction == 2)
			x = lcdDrawSJISChar(dev, fx, x, y, sjis[i], color);
		if (dev->_font_direction == 3)
			y = lcdDrawSJISChar(dev, fx, x, y, sjis[i], color);
	}
	if (dev->_font_direction == 0) return x;
	if (dev->_font_direction == 2) return x;
	if (dev->_font_direction == 1) return y;
	if (dev->_font_direction == 3) return y;
	return 0;
}
#endif

// Set font direction
// dir:Direction
void lcdSetFontDirection( uint16_t dir) {
	TFT7789_t *pdev = &dev_st7789;

	pdev->_font_direction = dir;
}

// Set font filling
// color:fill color
void lcdSetFontFill( uint16_t color) {
	TFT7789_t *pdev = &dev_st7789;

	pdev->_font_fill = true;
	pdev->_font_fill_color = color;
}

// UnSet font filling
void lcdUnsetFontFill(void) {
	TFT7789_t *pdev = &dev_st7789;

	pdev->_font_fill = false;
}

// Set font underline
// color:frame color
void lcdSetFontUnderLine( uint16_t color) {
	TFT7789_t *pdev = &dev_st7789;

	pdev->_font_underline = true;
	pdev->_font_underline_color = color;
}

// UnSet font underline
void lcdUnsetFontUnderLine(void) {
	TFT7789_t *pdev = &dev_st7789;
	pdev->_font_underline = false;
}

// Backlight OFF
void lcdBacklightOff(void) {
	TFT7789_t *pdev = &dev_st7789;

	if(pdev->_bl >= 0) {
		gpio_set_level( pdev->_bl, 0 );
	}
}

// Backlight ON
void lcdBacklightOn(void) {
	TFT7789_t *pdev = &dev_st7789;

	if(pdev->_bl >= 0) {
		gpio_set_level( pdev->_bl, 1 );
	}
}

// Display Inversion Off
void lcdInversionOff(void) {

	spi_master_write_command( 0x20);	//Display Inversion Off
}

// Display Inversion On
void lcdInversionOn(void) {

	spi_master_write_command( 0x21);	//Display Inversion On
}
