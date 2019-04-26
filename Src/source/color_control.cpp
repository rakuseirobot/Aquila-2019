/*
 * color_control.cpp
 *
 * Created: 2018/08/23 18:28:32
 *  Author: shun2
 */ 
/*
PF0:RANGE
PF1:CK
PF2:GATE
PF3:DOUT
*/
#include "color_control.hpp"
#include <stdint.h>
#include "delay.hpp"
#include "main.h"
#include "uart_control.hpp"
extern uart xbee;
uart color_serial = xbee;
uint16_t red=0, green=0, blue=0;	//値
uint16_t high=800, low=500;	//しきい値 白>800  700<銀<=800

void color_read(void){	//センサーの値をgree,red,blueに代入してくれる
	red=0, green=0, blue=0;
	HAL_GPIO_WritePin(GATE_GPIO_Port,GATE_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(CK_GPIO_Port,CK_Pin|RANGE_Pin,GPIO_PIN_RESET);
	HAL_Delay(2);
	HAL_GPIO_WritePin(DOUT_GPIO_Port,GATE_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(CK_GPIO_Port,RANGE_Pin,GPIO_PIN_RESET);
	HAL_Delay(20);	//光検出delay
	HAL_GPIO_WritePin(DOUT_GPIO_Port,GATE_Pin,GPIO_PIN_RESET);
	_delay_us(4);
	red=color_shift();
	green=color_shift();
	blue=color_shift();
	HAL_GPIO_WritePin(DOUT_GPIO_Port,GATE_Pin,GPIO_PIN_SET);
}

uint16_t color_shift(void){//12ビット分のパルス送信と読み込み処理
	uint16_t result=0;//検出結果用の変数を用意（0：初期化）
	for(uint8_t i=0;i<12;i++){//12ビット分の繰り返し処理
		HAL_GPIO_WritePin(CK_GPIO_Port,CK_Pin|RANGE_Pin,GPIO_PIN_SET);
		_delay_us(1);
		if(HAL_GPIO_ReadPin(DOUT_GPIO_Port,DOUT_Pin)!=0){//Dout端子からの出力がHighの場合
			result+=(1<<i);//12ビットのi桁目に1を代入（i桁分だけ左にシフト）
		}
		HAL_GPIO_WritePin(CK_GPIO_Port,CK_Pin|RANGE_Pin,GPIO_PIN_RESET);//1ビット分のクロックパルス出力（LOW）
		_delay_us(1);
	}
	_delay_us(3);//3マイクロ秒待機
	return result;//結果を出力
}

uint8_t color_check(void){	//カラーチェック 0:白 1:クロ 2:銀（チェックポイント）　　returnで帰ってきます。  //まだ値定まらない！
	color_read();
	//if(blue+green+red>700){
		//return 2;
	//}
	if(blue+green+red>=80){//??
		return 0;
	}
	else{
		color_serial.string("\x1b[31mBlack!!\x1b[39m\n\r");
		return 1;
	}
}
