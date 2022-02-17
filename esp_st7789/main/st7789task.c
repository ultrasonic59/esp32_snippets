/*
 * st7789task.c
 *
 *  Created on: Feb 16, 2022
 *      Author: vagol
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_vfs.h"
#include "esp_spiffs.h"

#include "st7789.h"
#include "fonts.h"
#include "displ.h"

///#include "dprint.h"

///================================================
#define	INTERVAL		50 ///100	///400
#define WAIT	vTaskDelay(INTERVAL)
///================================================
uint16_t tbl_col[]={
		RED,
		GREEN,
		BLUE,
		WHITE,
		GRAY,
		YELLOW,
		CYAN,
		PURPLE
};
void st7789task(void *pvParameters)
{
	uint8_t ptr_col=0;
	uint8_t b_tst=0;
	// set font file
	FontxFile fx16G[2];
	FontxFile fx24G[2];
	FontxFile fx32G[2];
////	FontxFile fx32L[2];
	InitFontx(fx16G,"/spiffs/ILGH16XB.FNT",""); // 8x16Dot Gothic
	InitFontx(fx24G,"/spiffs/ILGH24XB.FNT",""); // 12x24Dot Gothic
#if 0
	InitFontx(fx32G,"/spiffs/ILGH32XB.FNT",""); // 16x32Dot Gothic
	InitFontx(fx32L,"/spiffs/LATIN32B.FNT",""); // 16x32Dot Latin

	FontxFile fx16M[2];
	FontxFile fx24M[2];
	FontxFile fx32M[2];
	InitFontx(fx16M,"/spiffs/ILMH16XB.FNT",""); // 8x16Dot Mincyo
	InitFontx(fx24M,"/spiffs/ILMH24XB.FNT",""); // 12x24Dot Mincyo
	InitFontx(fx32M,"/spiffs/ILMH32XB.FNT",""); // 16x32Dot Mincyo
#endif

	uint16_t color = GREEN;

////	TFT_t dev;
	spi_master_init(&dev, CONFIG_MOSI_GPIO, CONFIG_SCLK_GPIO, CONFIG_CS_GPIO, CONFIG_DC_GPIO, CONFIG_RESET_GPIO, CONFIG_BL_GPIO);
	lcdInit(&dev, CONFIG_WIDTH, CONFIG_HEIGHT, CONFIG_OFFSETX, CONFIG_OFFSETY);

///	_dprint("test");
////	_dput(0x33);
	init_displ(fx16G,&dev,WHITE);
	lcdFillScreen(&dev, BLACK);
	lcdSetFontDirection(&dev, 3);

for(;;)
{
printd("\n test[%x]", b_tst);
b_tst++;
WAIT;
lcdFillScreen(&dev, BLACK);

}

while(1) {
		// Multi Font Test
		uint16_t color;
		uint8_t ascii[40];
		uint16_t margin = 10;
		lcdFillScreen(&dev, BLACK);
		color = GREEN;///YELLOW;///WHITE;
		lcdSetFontDirection(&dev, 0);
		uint16_t xpos = 0;
		uint16_t ypos = 15;
		int xd = 0;
		int yd = 1;
#if 0
		if(CONFIG_WIDTH < CONFIG_HEIGHT) {
			lcdSetFontDirection(&dev, 1);
			xpos = (CONFIG_WIDTH-1)-16;
			ypos = 0;
			xd = 1;
			yd = 0;
		}
#endif
		color=tbl_col[ptr_col&0x7];
		strcpy((char *)ascii, "Hello Irina");
		lcdDrawString(&dev, fx16G, xpos, ypos, ascii, color);
////		WAIT;

		xpos = xpos - (24 * xd) - (margin * xd);
		ypos = ypos + (16 * yd) + (margin * yd);
		strcpy((char *)ascii, "Hi Irina");
		lcdDrawString(&dev, fx24G, xpos, ypos, ascii, color);
		WAIT;

		xpos = xpos - (32 * xd) - (margin * xd);
		ypos = ypos + (24 * yd) + (margin * yd);
		if (CONFIG_WIDTH >= 240) {
			strcpy((char *)ascii, "32Dot Gothic Font");
			lcdDrawString(&dev, fx32G, xpos, ypos, ascii, color);
			xpos = xpos - (32 * xd) - (margin * xd);;
			ypos = ypos + (32 * yd) + (margin * yd);
		}
		ptr_col++;
#if 0
		xpos = xpos - (10 * xd) - (margin * xd);
		ypos = ypos + (10 * yd) + (margin * yd);
		strcpy((char *)ascii, "16Dot Mincyo Font");
		lcdDrawString(&dev, fx16M, xpos, ypos, ascii, color);
		WAIT;

		xpos = xpos - (24 * xd) - (margin * xd);;
		ypos = ypos + (16 * yd) + (margin * yd);
		strcpy((char *)ascii, "24Dot Mincyo Font");
		lcdDrawString(&dev, fx24M, xpos, ypos, ascii, color);
		WAIT;

		if (CONFIG_WIDTH >= 240) {
			xpos = xpos - (32 * xd) - (margin * xd);;
			ypos = ypos + (24 * yd) + (margin * yd);
			strcpy((char *)ascii, "32Dot Mincyo Font");
			lcdDrawString(&dev, fx32M, xpos, ypos, ascii, color);
		}
#endif
		lcdSetFontDirection(&dev, 0);
////		WAIT;

	} // end while

	// never reach
	while (1) {
		vTaskDelay(2000 / portTICK_PERIOD_MS);
	}
}




