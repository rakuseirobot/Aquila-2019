/*
 * spi_lcd.hpp
 *
 *  Created on: 2019/04/10
 *      Author: shun2
 */

#ifndef SOURCE_SPI_LCD_HPP_
#define SOURCE_SPI_LCD_HPP_

#include <stdint.h>


#define ROW 320
#define COL 240
/*
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
*/
#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

#define COLOR_BLACK 0x0000
#define COLOR_BLUE 0x001f
#define COLOR_RED 0xf800
#define COLOR_PURPLE 0xf81f
#define COLOR_GREEN 0x07e0
#define COLOR_CYAN 0x07ff
#define COLOR_YELLOW 0xffe0
#define COLOR_WHITE 0xffff

//long int color_table[8] = {
	//COLOR_BLACK ,
	//COLOR_BLUE ,
	//COLOR_RED ,
	//COLOR_PURPLE ,
	//COLOR_GREEN ,
	//COLOR_CYAN ,
	//COLOR_YELLOW ,
	//COLOR_WHITE
//};
void Reset(int i);
void lcd_com(uint16_t comm);
void lcd_data(uint16_t data);
void lcd_ctrl(uint16_t com,uint16_t data);
void init_lcd2(void);
void lcd_block(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend);	//範囲指定で塗りつぶす
void lcd_color(unsigned int color1);
void lcd_dot(uint16_t x,uint16_t y,uint16_t color);	//x軸y軸指定し、点表示
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
//void drawText(int nCol, int nRow, char *string, int color);
//void drawTextChar(int nCol, int nRow, char c, int color);
void drawPixel(int x, int y, unsigned int color);
void BlockWrite(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend);
void BoxFill(int x, int y, uint16_t width, uint16_t height, unsigned int color1);
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
int MakeColorGray(int Kosa);
void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);



#endif /* SOURCE_SPI_LCD_HPP_ */
