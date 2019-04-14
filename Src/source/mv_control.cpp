/*
 * mv_control.cpp
 *
 * Created: 2018/09/18 16:51:55
 *  Author: shun2
 */ 

/*
PD2:MV1
PD3:MV2
PD4:MV3  --SIG

PJ5:MVS1
PJ6:MVS2
PJ7:MVS3  --SS

MV
   2
1  ?ｿｽ?ｿｽ  3 

*/

#include "mv_control.hpp"
#include "../peripheral.hpp"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f429xx.h"
#include "stm32f4xx_hal_gpio.h"
#include "spi_control.hpp"
#include "uart_control.hpp"
#include "core.hpp"
#include "action.hpp"
#include "ui_control.hpp"
#include "lcd_control.hpp"
#include "motor_control.hpp"

spi mv_spi(&hspi2);
extern uart xbee;
uart mv_serial = xbee;

void mv_cap(mv_ch_t di,bool st){
	switch(di){
		case MV_LEFT:
			if(st==true){
				HAL_GPIO_WritePin ((GPIO_TypeDef*)MVSIG1_GPIO_Port ,(uint16_t)MVSIG1_Pin , GPIO_PIN_RESET);
			}else{
				HAL_GPIO_WritePin(MVSIG1_GPIO_Port,MVSIG1_Pin,GPIO_PIN_SET);
			}
			break;
		case MV_FRONT:
			if(st==true){
				HAL_GPIO_WritePin ((GPIO_TypeDef*)MVSIG2_GPIO_Port ,(uint16_t)MVSIG2_Pin , GPIO_PIN_RESET);
			}else{
				HAL_GPIO_WritePin(MVSIG2_GPIO_Port,MVSIG2_Pin,GPIO_PIN_SET);
			}
			break;
		case MV_RIGHT:
			if(st==true){
				HAL_GPIO_WritePin ((GPIO_TypeDef*)MVSIG3_GPIO_Port ,(uint16_t)MVSIG3_Pin , GPIO_PIN_RESET);
			}else{
				HAL_GPIO_WritePin(MVSIG3_GPIO_Port,MVSIG3_Pin,GPIO_PIN_SET);
			}
			break;
		default:
			break;
	}
	return;
}

void mv_sig(uint8_t i,bool ud){
	if (ud){
		if (i==1){
			HAL_GPIO_WritePin(MVSIG1_GPIO_Port,MVSIG1_Pin,GPIO_PIN_SET);
		}
		else if (i==2){	
			HAL_GPIO_WritePin(MVSIG2_GPIO_Port,MVSIG2_Pin,GPIO_PIN_SET);
		}
		else if(i==3){
			HAL_GPIO_WritePin(MVSIG3_GPIO_Port,MVSIG3_Pin,GPIO_PIN_SET);
		}
		else{
			return;
		}
	}
	else{
		if (i==1){
			HAL_GPIO_WritePin(MVSIG1_GPIO_Port,MVSIG1_Pin,GPIO_PIN_RESET);
		}
		else if (i==2){
			HAL_GPIO_WritePin(MVSIG2_GPIO_Port,MVSIG2_Pin,GPIO_PIN_RESET);
		}
		else if(i==3){
			HAL_GPIO_WritePin(MVSIG3_GPIO_Port,MVSIG3_Pin,GPIO_PIN_RESET);
		}
		else{
			return;
		}
	}
	return;
}
uint8_t mv_spi_send(mv_ch_t i,uint8_t val){
	if (i==MV_LEFT){
		HAL_GPIO_WritePin(MVSIG1_GPIO_Port,MVSIG1_Pin,GPIO_PIN_SET);
	}
	else if (i==MV_FRONT){
		HAL_GPIO_WritePin(MVSIG2_GPIO_Port,MVSIG2_Pin,GPIO_PIN_SET);
	}
	else if(i==MV_RIGHT){
		HAL_GPIO_WritePin(MVSIG3_GPIO_Port,MVSIG3_Pin,GPIO_PIN_SET);
	}
	else{
		return 0;
	}
	HAL_Delay(10);
	if (i==MV_LEFT){
		HAL_GPIO_WritePin(MVSIG1_GPIO_Port,MVSIG1_Pin,GPIO_PIN_RESET);
	}
	else if (i==MV_FRONT){
		HAL_GPIO_WritePin(MVSIG2_GPIO_Port,MVSIG2_Pin,GPIO_PIN_RESET);
	}
	else if(i==MV_RIGHT){
		HAL_GPIO_WritePin(MVSIG3_GPIO_Port,MVSIG3_Pin,GPIO_PIN_RESET);
	}
	else{
		return 0;
	}
	uint8_t dat[1];
	uint8_t tx[1]={val};
	HAL_Delay(10);
	mv_spi.send(tx,dat);
	if (i==MV_LEFT){
		HAL_GPIO_WritePin(MVSIG1_GPIO_Port,MVSIG1_Pin,GPIO_PIN_SET);
	}
	else if (i==MV_FRONT){
		HAL_GPIO_WritePin(MVSIG2_GPIO_Port,MVSIG2_Pin,GPIO_PIN_SET);
	}
	else if(i==MV_RIGHT){
		HAL_GPIO_WritePin(MVSIG3_GPIO_Port,MVSIG3_Pin,GPIO_PIN_SET);
	}
	else{
		return 0;
	}
	HAL_Delay(10);
	HAL_GPIO_WritePin(MVSIG1_GPIO_Port,MVSIG1_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(MVSIG2_GPIO_Port,MVSIG2_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(MVSIG3_GPIO_Port,MVSIG3_Pin,GPIO_PIN_SET);
	//while((PORTJ.IN & PIN5_bm)==0||(PORTJ.IN & PIN6_bm)==0||(PORTJ.IN & PIN7_bm)==0);
	return dat[0];
}

bool kit_chk(uint8_t num=0){//num = type of victim. //unknown(1/17)
	//if(ta.ac_next(v::front,1)==np)return false;
// 	int key = 10;
// 	key = ta.r_now()->type;
	node* t=ta.r_now();
	if(t)if(t->type==num||t->type==v::black)return false;
	if(!num)return true;
	bool ans  = true;
	for(uint8_t i = 0;i<4;i++){
		if(t->next[i])if(t->next[i]->type==num){
			ans = false;
		}
	}
	//return (key==v::normal||key==v::unknown);
	return ans;
	//return (hhh.key==0 || hhh.key==1 );//normal or unknown
}

bool check_sig(bool check){
	if(HAL_GPIO_ReadPin(MVS1_GPIO_Port,MVS1_Pin)==0 && check==true){
		mv_cap(MV_LEFT,false);
		mv_cap(MV_FRONT,false);
		mv_cap(MV_RIGHT,false);
		check_mv(MV_LEFT);
		return false;
	}
	else if(HAL_GPIO_ReadPin(MVS2_GPIO_Port,MVS2_Pin)==0 && check==true){
		mv_cap(MV_LEFT,false);
		mv_cap(MV_FRONT,false);
		mv_cap(MV_RIGHT,false);
		check_mv(MV_FRONT);
		return false;
	}
	else if(HAL_GPIO_ReadPin(MVS3_GPIO_Port,MVS3_Pin)==0 && check==true){
		mv_cap(MV_LEFT,false);
		mv_cap(MV_FRONT,false);
		mv_cap(MV_RIGHT,false);
		check_mv(MV_RIGHT);
		return false;
	}
	else{}
	return true;
}
uint8_t check_mv(mv_ch_t dir){ //0:return???,1:????,2:??????
	//mv_sig(dir,false);
	HAL_Delay(2);
	led(Blueled,1);
	uint8_t res = mv_spi_send(dir,0);
	mv_serial.putint((int)ta.r_now());
	mv_serial.string(" , ");
	mv_serial.putint(ta.r_now()->x);
	mv_serial.string(" , ");
	mv_serial.putint(ta.r_now()->y);
	mv_serial.string(" , ");
	mv_serial.putint(ta.r_now()->z);
	mv_serial.string(" ::VictimNotify!\n\r");
	mv_serial.string("ch");
	mv_serial.putint(res);
	mv_serial.string("\n\r");/*
	if(kit_chk()==false){
		//モーター再開
		lcd_clear();
		led(Blueled,0);
		lcd_putstr("return");
		buzzer(400);
		return 0;
	}*/
	motor::set_Status(motor::PAUSE);
	//hhh.key=1;
	mv_cap(dir,false);
	//if (dir==3&&res!=0){
	//	return 2;
	//}
	mv_cap(MV_LEFT,false);
	mv_cap(MV_FRONT,false);
	mv_cap(MV_RIGHT,false);
	led(Blueled,0);
	switch(res){
		case 3://H  2kits
			lcd_clear();
			lcd_putstr("Find H!");
			if(kit_chk(v::H))finded_victim(2,dir);
			ta.r_now()->type=v::H;
			break;
		case 4://S  1kits
			lcd_clear();
			lcd_putstr("Find S!");
			if(kit_chk(v::S))finded_victim(1,dir);
			ta.r_now()->type=v::S;
			break;
		case 5://U 0kits
			lcd_clear();
			lcd_putstr("Find U!");
			if(kit_chk(v::U))finded_victim(0,dir);
			ta.r_now()->type=v::U;
			break;
		case 6:
			lcd_clear();
			lcd_putstr("Find Sermo");
			if(kit_chk(v::sermo))finded_victim(1,dir);
			ta.r_now()->type=v::sermo;
			break;
		case 7:
		case 8:
			lcd_clear();
			lcd_putstr("Find Error");
			finded_victim(1,dir);
			//ta.r_now()->type=v::r_kit;
			break;
		default:
			break;
	}
	lcd_clear();
	mv_cap(MV_LEFT,true);
	mv_cap(MV_FRONT,true);
	mv_cap(MV_RIGHT,true);
	mv_cap(dir,false);

	HAL_Delay(2000);

	led(Blueled,0);
	HAL_Delay(2);
	motor::set_Status(motor::RESTART);
	return 1;
}


