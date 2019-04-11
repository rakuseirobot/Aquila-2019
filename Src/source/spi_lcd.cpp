/*
 * spi_lcd.cpp
 *
 *  Created on: 2019/04/10
 *      Author: shun2
 */


/*
・nCS：Lowでセレクテッド、Highでノットセレクテッド						PF4
・nWR/SCL：SPIモードの時はクロックで同期させること						SCK
・nRESET：LOWにするとILI9328をリセットする								PC1 ???
・SDI：SPIインターフェースの入力ピン。SCL信号の立ち上がりでラッチされる。 MOSI
・SDO：SPIインターフェースの出力ピン。SCL信号の立ち下がりで出力される。	MISO
*/

#include "spi_control.hpp"
#include "../peripheral.hpp"
#include "delay.hpp"
#include <stdint.h>
#include <math.h>
#include "spi_lcd.hpp"
//void CS(int x){
	//PORTD.OUT |= (x<<PIN4_bm);
//}
SPI_HandleTypeDef lcd_spi = hspi1;


void lcd_com(uint16_t comm){
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port,LCD_CS_Pin,GPIO_PIN_RESET);
	HAL_Delay(1);
	uint8_t TxBuf[3]={0x70,(uint8_t)(comm>>8),(uint8_t)(comm & 0xFF)};
	HAL_SPI_Transmit(&lcd_spi,TxBuf,3,100);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port,LCD_CS_Pin,GPIO_PIN_SET);
}
void lcd_data(uint16_t data){
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port,LCD_CS_Pin,GPIO_PIN_RESET);
	HAL_Delay(1);
	uint8_t TxBuf[3]={0x72,(uint8_t)(data>>8),(uint8_t)(data & 0xFF)};
	HAL_SPI_Transmit(&lcd_spi,TxBuf,3,100);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port,LCD_CS_Pin,GPIO_PIN_SET);
}
void lcd_ctrl(uint16_t com,uint16_t data){
	lcd_com(com);
	lcd_data(data);
}
void init_lcd2(void){
	HAL_GPIO_WritePin(LCD_RST_GPIO_Port,LCD_RST_Pin,GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(LCD_RST_GPIO_Port,LCD_RST_Pin,GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(LCD_RST_GPIO_Port,LCD_RST_Pin,GPIO_PIN_SET);
	HAL_Delay(50);

	lcd_ctrl(0x00E5, 0x78F0);     // set SRAM internal timing
	lcd_ctrl(0x0001, 0x0000);     // set SS and SM bit
	lcd_ctrl(0x0002, 0x0400);     // set 1 line inversion
	lcd_ctrl(0x0003, 0x1090);     // set GRAM write direction and BGR=1.
	lcd_ctrl(0x0004, 0x0000);     // Resize register
	lcd_ctrl(0x0008, 0x0202);     // set the back porch and front porch
	lcd_ctrl(0x0009, 0x0000);     // set non-display area refresh cycle ISC[3:0]
	lcd_ctrl(0x000A, 0x0000);     // FMARK function
	lcd_ctrl(0x000C, 0x0000);     // RGB interface setting
	lcd_ctrl(0x000D, 0x0000);     // Frame marker Position
	lcd_ctrl(0x000F, 0x0000);     // RGB interface polarity
	//
	 //lcd_ctrl(0x0001, 0x0100);     // set SS and SM bit
	 //lcd_ctrl(0x0002, 0x0700);     // set 1 line inversion
	 //lcd_ctrl(0x0003, 0x1030);     // set GRAM write direction and BGR=1.
	 //lcd_ctrl(0x0004, 0x0000);     // Resize register
	 //lcd_ctrl(0x0008, 0x0207);     // set the back porch and front porch
	 //lcd_ctrl(0x0009, 0x0000);     // set non-display area refresh cycle ISC[3:0]
	 //lcd_ctrl(0x000A, 0x0000);     // FMARK function
	 //lcd_ctrl(0x000C, 0x0000);     // RGB interface setting
	 //lcd_ctrl(0x000D, 0x0000);     // Frame marker Position
	 //lcd_ctrl(0x000F, 0x0000);      // RGB interface polarity

	//*************Power On sequence **************** //

	lcd_ctrl(0x0010, 0x0000);       // SAP, BT[3:0], AP, DSTB, SLP, STB
	lcd_ctrl(0x0011, 0x0007);       // DC1[2:0], DC0[2:0], VC[2:0]
	lcd_ctrl(0x0012, 0x0000);       // VREG1OUT voltage
	lcd_ctrl(0x0013, 0x0000);       // VDV[4:0] for VCOM amplitude
	lcd_ctrl(0x0007, 0x0001);
	HAL_Delay(200);                             // Dis-charge capacitor power voltage
	lcd_ctrl(0x0010, 0x1690);       // SAP, BT[3:0], AP, DSTB, SLP, STB
	lcd_ctrl(0x0010,0x1490);
	lcd_ctrl(0x0011, 0x0227);       // Set DC1[2:0], DC0[2:0], VC[2:0]
	HAL_Delay(50);                       // delay 50ms
	lcd_ctrl(0x0012, 0x008C);       // External reference voltage= Vci;
	HAL_Delay(50);                       // delay 50ms
	lcd_ctrl(0x0013, 0x1500);       //VDV[4:0] for VCOM amplitude  0X1000
	lcd_ctrl(0x0029, 0x0004);       // VCM[5:0] for VCOMH      0X0009
	lcd_ctrl(0x002B, 0x000D);       //Set Frame Rate
	HAL_Delay(50);                               // delay 50ms
	lcd_ctrl(0x0020, 0x0000);       // GRAM horizontal Address
	lcd_ctrl(0x0021, 0x0000);       // GRAM Vertical Address

	//lcd_ctrl(0x0010, 0x0000);      // SAP, BT[3:0], AP, DSTB, SLP, STB
	//lcd_ctrl(0x0011, 0x0007);      // DC1[2:0], DC0[2:0], VC[2:0]
	//lcd_ctrl(0x0012, 0x0000);      // VREG1OUT voltage
	//lcd_ctrl(0x0013, 0x0000);      // VDV[4:0] for VCOM amplitude
	//lcd_ctrl(0x0007, 0x0001);
	//HAL_Delay(200);                            // Dis-charge capacitor power voltage
	//lcd_ctrl(0x0010, 0x1490);      // SAP, BT[3:0], AP, DSTB, SLP, STB
	   //
	//lcd_ctrl(0x0011, 0x0227);      // DC1[2:0], DC0[2:0], VC[2:0]
	//HAL_Delay(50);                       // Delay 50ms
	//lcd_ctrl(0x0012, 0x001C);      // Internal reference voltage= Vci;
	//HAL_Delay(50);                       // Delay 50ms
	//lcd_ctrl(0x0013, 0x1A00);      // Set VDV[4:0] for VCOM amplitude
	//lcd_ctrl(0x0029, 0x0025);      // Set VCM[5:0] for VCOMH
	//lcd_ctrl(0x002B, 0x000F);      // Set Frame Rate        0x000C<0x000f
	//HAL_Delay(50);                              // Delay 50ms
	//lcd_ctrl(0x0020, 0x0000);       // GRAM horizontal Address
	//lcd_ctrl(0x0021, 0x0000);       // GRAM Vertical Address
	// ----------- Adjust the Gamma   Curve ----------//
	//lcd_ctrl(0x0030, 0x0000);
	//lcd_ctrl(0x0031, 0x0607);
	//lcd_ctrl(0x0032, 0x0305);
	//lcd_ctrl(0x0035, 0x0000);
	//lcd_ctrl(0x0036, 0x1604);
	//lcd_ctrl(0x0037, 0x0204);
	//lcd_ctrl(0x0038, 0x0001);
	//lcd_ctrl(0x0039, 0x0707);
	//lcd_ctrl(0x003C, 0x0000);
	//lcd_ctrl(0x003D, 0x000F);


	lcd_ctrl(0x0030, 0x0000);
	lcd_ctrl(0x0031, 0x0506);
	lcd_ctrl(0x0032, 0x0104);
	lcd_ctrl(0x0035, 0x0207);
	lcd_ctrl(0x0036, 0x000F);
	lcd_ctrl(0x0037, 0x0306);
	lcd_ctrl(0x0038, 0x0102);
	lcd_ctrl(0x0039, 0x0707);
	lcd_ctrl(0x003C, 0x0702);
	lcd_ctrl(0x003D, 0x1604);

	//------------------ Set GRAM area ---------------//

	lcd_ctrl(0x0050, 0x0000);      // Horizontal GRAM Start Address
	lcd_ctrl(0x0051, 0x00EF);      // Horizontal GRAM End Address
	lcd_ctrl(0x0052, 0x0000);      // Vertical GRAM Start Address
	lcd_ctrl(0x0053, 0x013F);      // Vertical GRAM Start Address
	lcd_ctrl(0x0060, 0xA700);      // Gate Scan Line
	lcd_ctrl(0x0061, 0x0001);      // NDL,VLE, REV
	lcd_ctrl(0x006A, 0x0000);      //  set scrolling line

	//-------------- Partial Display Control ---------//
	lcd_ctrl(0x0080, 0x0000);
	lcd_ctrl(0x0081, 0x0000);
	lcd_ctrl(0x0082, 0x0000);
	lcd_ctrl(0x0083, 0x0000);
	lcd_ctrl(0x0084, 0x0000);
	lcd_ctrl(0x0085, 0x0000);

	//-------------- Panel Control -------------------//
	lcd_ctrl(0x0090, 0x0010);
	lcd_ctrl(0x0092, 0x0600);
	lcd_ctrl(0x0007, 0x0133);      // 262K color and display ON
}
void lcd_block(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend) //範囲指定で塗りつぶす
{
	lcd_com(0x0050);
	lcd_data(Xstart);
	lcd_com(0x0051);
	lcd_data(Xend);
	lcd_com(0x0052);
	lcd_data(Ystart);
	lcd_com(0x0053);
	lcd_data(Yend);
	lcd_com(0x0020);
	lcd_data(Xstart);
	lcd_com(0x0021);
	lcd_data(Ystart);
	lcd_com(0x0022);
}

void lcd_color(unsigned int color1)
{
	unsigned int i,j;
	////CS(0);
	lcd_block(0,COL-1,0,ROW-1);
	for(i=0;i<ROW;i++){
		for(j=0;j<COL;j++){
			lcd_data(color1);
		}
	}
	//CS(1);
}
void lcd_dot(uint16_t x,uint16_t y,uint16_t color){
	lcd_com(0x0050);
	lcd_data(x);
	lcd_com(0x0051);
	lcd_data(x);
	lcd_com(0x0052);
	lcd_data(y);
	lcd_com(0x0053);
	lcd_data(y);
	lcd_com(0x0020);
	lcd_data(x);
	lcd_com(0x0021);
	lcd_data(y);
	lcd_com(0x0022);
	lcd_data(color);
}

void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
	int16_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		_swap_int16_t(x0, y0);
		_swap_int16_t(x1, y1);
	}

	if (x0 > x1) {
		_swap_int16_t(x0, x1);
		_swap_int16_t(y0, y1);
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1) {
		ystep = 1;
		} else {
		ystep = -1;
	}

	for (; x0<=x1; x0++) {
		if (steep) {
			lcd_dot(x0, y0, color);
			} else {
			lcd_dot(x0, y0, color);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}
void drawPixel(int x, int y, unsigned int color)
{
	//  digitalWrite(pin_CS, 0);

	lcd_com(0x0050);lcd_data(x);
	lcd_com(0x0051);lcd_data(x);
	lcd_com(0x0052);lcd_data(y);
	lcd_com(0x0053);lcd_data(y);
	//writeRegister16(0x0020, x);
	lcd_com(0x0020);lcd_data(x);
	//writeRegister16(0x0021, y);
	lcd_com(0x0021);lcd_data(y);
	//writeRegister16(0x0022, color);
	lcd_com(0x0022);lcd_data(color);

	//  digitalWrite(pin_CS, 1);
}

void BlockWrite(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend)
{
	lcd_com(0x0050);lcd_data(Xstart);
	lcd_com(0x0051);lcd_data(Xend);
	lcd_com(0x0052);lcd_data(Ystart);
	lcd_com(0x0053);lcd_data(Yend);
	lcd_com(0x0020);
	lcd_data(Xstart);
	lcd_com(0x0021);
	lcd_data(Ystart);
	lcd_com(0x0022);
}


void BoxFill(int x, int y, uint16_t width, uint16_t height, unsigned int color1)
{
	unsigned int i,j;
	//  digitalWrite(pin_CS, 0);

	BlockWrite(x,x+width-1,y,y+height-1);

	for(i=0;i<width;i++){
		for(j=0;j<height;j++){
			lcd_data(color1);
		}
	}
	//  digitalWrite(pin_CS, 1);
}

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	// Update in subclasses if desired!
	//  for (int16_t i=x; i<x+w; i++) {
	//    drawFastVLine(i, y, h, color);
	//  }
	BoxFill(x, y, w, h, color);
}


void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	drawPixel(x0  , y0+r, color);
	drawPixel(x0  , y0-r, color);
	drawPixel(x0+r, y0  , color);
	drawPixel(x0-r, y0  , color);

	while (x<y) {
		if (f >= 0) {
		y--;
		ddF_y += 2;
		f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

	drawPixel(x0 + x, y0 + y, color);
		drawPixel(x0 - x, y0 + y, color);
		drawPixel(x0 + x, y0 - y, color);
		drawPixel(x0 - x, y0 - y, color);
		drawPixel(x0 + y, y0 + x, color);
		drawPixel(x0 - y, y0 + x, color);
		drawPixel(x0 + y, y0 - x, color);
		drawPixel(x0 - y, y0 - x, color);
	}
	}
void drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) {
   int16_t f     = 1 - r;
   int16_t ddF_x = 1;
   int16_t ddF_y = -2 * r;
   int16_t x     = 0;
   int16_t y     = r;

  while (x<y) {
     if (f >= 0) {
       y--;
       ddF_y += 2;
       f     += ddF_y;
     }
     x++;
     ddF_x += 2;
     f     += ddF_x;
     if (cornername & 0x4) {
       drawPixel(x0 + x, y0 + y, color);
       drawPixel(x0 + y, y0 + x, color);
     }
     if (cornername & 0x2) {
       drawPixel(x0 + x, y0 - y, color);
       drawPixel(x0 + y, y0 - x, color);
     }
     if (cornername & 0x8) {
       drawPixel(x0 - y, y0 + x, color);
       drawPixel(x0 - x, y0 + y, color);
     }
     if (cornername & 0x1) {
       drawPixel(x0 - y, y0 - x, color);
       drawPixel(x0 - x, y0 - y, color);
     }
   }
 }

void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
	//drawLine(x, y, x, y+h-1, color);
	BoxFill(x, y, 1, h, color);

}

void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
 {
   //drawLine(x, y, x+w-1, y, color);
  BoxFill(x, y, w, 1, color);
 }

void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
 {
   drawFastHLine(x, y, w, color);
   drawFastHLine(x, y+h-1, w, color);
   drawFastVLine(x, y, h, color);
   drawFastVLine(x+w-1, y, h, color);
 }

void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color)
 {
   int16_t f     = 1 - r;
   int16_t ddF_x = 1;
   int16_t ddF_y = -2 * r;
   int16_t x     = 0;
   int16_t y     = r;

  while (x<y) {
     if (f >= 0) {
       y--;
       ddF_y += 2;
       f     += ddF_y;
     }
     x++;
     ddF_x += 2;
     f     += ddF_x;

    if (cornername & 0x1) {
       drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
       drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
     }
     if (cornername & 0x2) {
       drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
       drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
     }
   }
 }

void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
 {
   drawFastVLine(x0, y0-r, 2*r+1, color);
   fillCircleHelper(x0, y0, r, 3, 0, color);
 }

void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
 {
   // smarter version
   drawFastHLine(x+r  , y    , w-2*r, color); // Top
   drawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
   drawFastVLine(x    , y+r  , h-2*r, color); // Left
   drawFastVLine(x+w-1, y+r  , h-2*r, color); // Right
   // draw four corners
   drawCircleHelper(x+r    , y+r    , r, 1, color);
   drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
   drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
   drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
 }

void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
 {
   // smarter version
   fillRect(x+r, y, w-2*r, h, color);

  // draw four corners
   fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
   fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
 }

void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
 {
   int16_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
   if (y0 > y1) {
     _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
   }
   if (y1 > y2) {
     _swap_int16_t(y2, y1); _swap_int16_t(x2, x1);
   }
   if (y0 > y1) {
     _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
   }

  if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
     a = b = x0;
     if(x1 < a)      a = x1;
     else if(x1 > b) b = x1;
     if(x2 < a)      a = x2;
     else if(x2 > b) b = x2;
     drawFastHLine(a, y0, b-a+1, color);
     return;
   }

  int16_t
     dx01 = x1 - x0,
     dy01 = y1 - y0,
     dx02 = x2 - x0,
     dy02 = y2 - y0,
     dx12 = x2 - x1,
     dy12 = y2 - y1;
   int32_t
     sa   = 0,
     sb   = 0;

  // For upper part of triangle, find scanline crossings for segments
   // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
   // is included here (and second loop will be skipped, avoiding a /0
   // error there), otherwise scanline y1 is skipped here and handled
   // in the second loop...which also avoids a /0 error here if y0=y1
   // (flat-topped triangle).
   if(y1 == y2) last = y1;   // Include y1 scanline
   else         last = y1-1; // Skip it

  for(y=y0; y<=last; y++) {
     a   = x0 + sa / dy01;
     b   = x0 + sb / dy02;
     sa += dx01;
     sb += dx02;
     /* longhand:
     a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
     b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
     */
     if(a > b) _swap_int16_t(a,b);
     drawFastHLine(a, y, b-a+1, color);
   }

  // For lower part of triangle, find scanline crossings for segments
   // 0-2 and 1-2.  This loop is skipped if y1=y2.
   sa = dx12 * (y - y1);
   sb = dx02 * (y - y0);
   for(; y<=y2; y++) {
     a   = x1 + sa / dy12;
     b   = x0 + sb / dy02;
     sa += dx12;
     sb += dx02;
     /* longhand:
     a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
     b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
     */
     if(a > b) _swap_int16_t(a,b);
     drawFastHLine(a, y, b-a+1, color);
   }
 }


 int MakeColorGray(int Kosa)
 {
   int red = Kosa << 11;
   int green = Kosa << 6;
   int blue = Kosa;
   return red+green+blue;
 }

void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
 {
   drawLine(x0, y0, x1, y1, color);
   drawLine(x1, y1, x2, y2, color);
   drawLine(x2, y2, x0, y0, color);
 }
