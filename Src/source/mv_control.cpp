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
1  ?¿½?¿½  3 

*/

#include "mv_control.hpp"
#include "../peripheral.hpp"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f429xx.h"
#include "stm32f4xx_hal_gpio.h"
/*
void mv_cap(mv_ch_t di,bool st){
	switch(di){
		case MV_LEFT:
			if(st==true){
				HAL_GPIO_WritePin ((GPIO_TypeDef*)MVSIG1_GPIO_Port ,(uint16_t)MVSIG1_Pin , GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOC,10,RESET);
			}else{
				HAL_GPIO_WritePin(MVSIG1_GPIO_Port,MVSIG1_Pin,GPIO_PIN_SET);
			}
			break;
		case MV_FRONT:
			if(st==true){
				PORTD.OUTCLR=PIN3_bm;
			}else{
				PORTD.OUTSET=PIN3_bm;
			}
			break;
		case MV_RIGHT:
			if(st==true){
				PORTD.OUTCLR=PIN4_bm;
			}else{
				PORTD.OUTSET=PIN4_bm;
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
			PORTD.OUTSET=PIN2_bm;
		}
		else if (i==2){	
			PORTD.OUTSET=PIN3_bm;
		}
		else if(i==3){
			PORTD.OUTSET=PIN4_bm;
		}
		else{
			return;
		}
	}
	else{
		if (i==1){
			PORTD.OUTCLR=PIN2_bm;
		}
		else if (i==2){
			PORTD.OUTCLR=PIN3_bm;
		}
		else if(i==3){
			PORTD.OUTCLR=PIN4_bm;
		}
		else{
			return;
		}
	}
	return;
}
uint8_t mv_spi_send(uint8_t val, uint8_t i){
	if (i==1){
		PORTD.OUTSET=PIN2_bm;
	}
	else if (i==2){
		PORTD.OUTSET=PIN3_bm;
	}
	else if(i==3){
		PORTD.OUTSET=PIN4_bm;
	}
	else{
		return 0;
	}
	_delay_ms(10);
	if (i==1){
		PORTD.OUTCLR=PIN2_bm;
	}
	else if (i==2){
		PORTD.OUTCLR=PIN3_bm;
	}
	else if(i==3){
		PORTD.OUTCLR=PIN4_bm;
	}
	else{
		return 0;
	}
	uint8_t dat = 0;
	_delay_ms(10);
	dat = mv.send(val);
	if (i==1){
		PORTD.OUTSET=PIN2_bm;
	}
	else if (i==2){
		PORTD.OUTSET=PIN3_bm;	
	}
	else if(i==3){
		PORTD.OUTSET=PIN4_bm;
	}
	else{
		return 0;
	}
	_delay_ms(10);
	PORTD.OUTCLR=PIN2_bm|PIN3_bm|PIN4_bm;
	//while((PORTJ.IN & PIN5_bm)==0||(PORTJ.IN & PIN6_bm)==0||(PORTJ.IN & PIN7_bm)==0);
	return dat;
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

uint8_t check_mv(mv_ch_t dir){ //0:return???,1:????,2:??????
	//mv_sig(dir,false);
	_delay_ms(2);
	led(Blueled,1);
	uint8_t res = mv_spi_send(dir,1);
	serial.putint((int)ta.r_now());
	serial.string(" , ");
	serial.putint(ta.r_now()->x);
	serial.string(" , ");
	serial.putint(ta.r_now()->y);
	serial.string(" , ");
	serial.putint(ta.r_now()->z);
	serial.string(" ::sermo\n");
	if(kit_chk()==false){
		PORTB.OUTCLR=PIN0_bm|PIN1_bm;
		lcd_clear();
		led(Blueled,0);
		lcd_putstr(LCD1_TWI,"return");
		buzzer(400);
		return 0;
	}
	PORTB.OUTSET=PIN0_bm|PIN1_bm;
	//hhh.key=1;
	mv_cap(dir,false);
	serial.string("ch");
	serial.putdec(res);
	serial.string("\n\r");
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
			lcd_putstr(LCD1_TWI,"Find H!");
			if(kit_chk(v::H))finded_victim(2,dir);
			ta.r_now()->type=v::H;
			break;
		case 4://S  1kits
			lcd_clear();
			lcd_putstr(LCD1_TWI,"Find S!");
			if(kit_chk(v::S))finded_victim(1,dir);
			ta.r_now()->type=v::S;
			break;
		case 5://U 0kits
			lcd_clear();
			lcd_putstr(LCD1_TWI,"Find U!");
			if(kit_chk(v::U))finded_victim(0,dir);
			ta.r_now()->type=v::U;
			break;
		case 6:
			lcd_clear();
			lcd_putstr(LCD1_TWI,"Find Sermo");
			if(kit_chk(v::sermo))finded_victim(1,dir);
			ta.r_now()->type=v::sermo;
			break;
		case 7:
		case 8:
			lcd_clear();
			lcd_putstr(LCD1_TWI,"Find Error");
			finded_victim(1,dir);
			//ta.r_now()->type=v::r_kit;
			break;
		default:
			break;
	};
	lcd_clear();
	mv_cap(MV_LEFT,true);
	mv_cap(MV_FRONT,true);
	mv_cap(MV_RIGHT,true);
	mv_cap(dir,false);
	led(Blueled,0);
	_delay_ms(2);
	PORTB.OUTCLR=PIN0_bm|PIN1_bm;
	return 1;
}

*/
