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

#include <math.h>
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
#include "ping_control.hpp"

spi mv_spi(&hspi2);
extern uart xbee;
uart mv_serial = xbee;
;

uint8_t MV_RECIEVED_DATA[3]={0,0,0};

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
void int_task_check_mv(uint16_t GPIO_Pin){
	uint8_t res=0,dir=0;
	xbee.string("VictimFind! >>");
	switch(GPIO_Pin){
		case MVS1_Pin:
			MV_RECIEVED_DATA[MV_DATA_DIR]=MV_LEFT;
			res=mv_spi_send(MV_LEFT,0);
			dir=1;
			xbee.string("LEFT>");
			break;
		case MVS2_Pin:
			MV_RECIEVED_DATA[MV_DATA_DIR]=MV_FRONT;
			res=mv_spi_send(MV_FRONT,0);
			dir=2;
			xbee.string("FRONT>");
			break;
		case MVS3_Pin:
			MV_RECIEVED_DATA[MV_DATA_DIR]=MV_RIGHT;
			res=mv_spi_send(MV_RIGHT,0);
			dir=3;
			xbee.string("RIGHT>");
			break;
	}
	xbee.putint(res);
	if(res==0){
		return;
	}
	if(!(ping(FRONT)<Sikiti)&&MV_RECIEVED_DATA[MV_DATA_DIR]==MV_FRONT){
		if(motor::Task_Before==motor::LEFT_TURN||motor::Task_Before==motor::RIGHT_TURN){
			MV_RECIEVED_DATA[MV_DATA_PING]=NOT_FIND_FRONT_WALL;
			xbee.string(">NOT_WALL\n\r");
		}
		else if(MV_RECIEVED_DATA[MV_DATA_DIR]==MV_FRONT){
			return;
		}
	}
	else if(MV_RECIEVED_DATA[MV_DATA_DIR]==MV_FRONT){
		MV_RECIEVED_DATA[MV_DATA_PING]=FIND_FRONT_WALL;
		xbee.string(">WALL\n\r");
	}
	MV_RECIEVED_DATA[MV_DATA_TYPE]=dir;/*
	HAL_NVIC_DisableIRQ(MVS1_EXTI_IRQn);
	HAL_NVIC_DisableIRQ(MVS2_EXTI_IRQn);
	HAL_NVIC_DisableIRQ(MVS3_EXTI_IRQn);*/
	mv_cap(MV_LEFT,false);
	mv_cap(MV_FRONT,false);
	mv_cap(MV_RIGHT,false);
	return;
}

void mv_after_stop_task_check(void){//終了後にキット投下が求められるタスク用
	uint8_t kit_need=0;
	switch(MV_RECIEVED_DATA[MV_DATA_TYPE]){
			case 3://H  2kits
				kit_need=2;
				lcd_clear();
				lcd_putstr("Find H!");
				break;
			case 4://S  1kits
				kit_need=1;
				lcd_clear();
				lcd_putstr("Find S!");
				break;
			case 5://U 0kits
				kit_need=0;
				lcd_clear();
				lcd_putstr("Find U!");
				break;
			case 6:
				kit_need=1;
				lcd_clear();
				lcd_putstr("Find Sermo!");
				break;
			case 7:
			case 8:
				kit_need=0;
				break;
			default:
				break;
		}
	if(MV_RECIEVED_DATA[MV_DATA_DIR]==MV_FRONT){
		#warning Nakao should write here >> Save mapping  >>　マッピングされていたらフラグを初期化してreturn!  >>4こ
		switch(motor::Task_Before){
			case motor::ONE_ADVANCE:
			case motor::TWO_ADVANCE:
			case motor::HALF_ADVANCE:
				//現在地地点記録
				MV_RECIEVED_DATA[MV_DATA_DIR]=MV_LEFT;
				break;
			case motor::ONE_BACK:
			case motor::TWO_BACK:
			case motor::HALF_BACK:
				//一個前の地点記録
				motor::set_Status(motor::BACK);
				if(MV_RECIEVED_DATA[MV_DATA_PING]==FIND_FRONT_WALL){//本来正常動作じゃない
					MV_RECIEVED_DATA[MV_DATA_DIR]=MV_LEFT;
					motor::set_Status(motor::BACK);//ジョブ巻き戻し
					while(motor::check_task()!=motor::FREE||abs(motor::Right_count)>=Motor_thre||abs(motor::Left_count)>=Motor_thre);
					motor::move(motor::RIGHT_TURN);
					led(Redled,1);
					led(Blueled,1);
					led(Greenled,1);
					if(MV_RECIEVED_DATA[MV_DATA_DIR]==MV_LEFT){
						Drop_kit(DROP_LEFT,kit_need);
					}
					else if(MV_RECIEVED_DATA[MV_DATA_DIR]==MV_RIGHT){
						Drop_kit(DROP_RIGHT,kit_need);
					}
					else{
						return;
					}
					motor::set_Status(motor::PAUSE);//モーター止める
					led(Redled,0);
					led(Blueled,0);
					led(Greenled,0);
					error_led(1,1);
					while(KIT_DROP_Status!=FREE){
						error_led(1,3);
						error_led(2,4);
						buzzer();
						HAL_Delay(300);
						xbee.putint(KIT_DROP_COUNT);
						xbee.string("\n\r");
					}
					error_led(1,0);
					error_led(2,0);
					MV_RECIEVED_DATA[MV_DATA_PING]=NOT_FIND_FRONT_WALL;
					MV_RECIEVED_DATA[MV_DATA_TYPE]=FIND_NOTHING;//フラグ初期化
					motor::set_Status(motor::RETURN);
					return;
				}
				else{//本来あり得ない
					MV_RECIEVED_DATA[MV_DATA_TYPE]=FIND_NOTHING;
					lcd_clear();
					return;
				}
				break;
			case motor::RIGHT_TURN:
				MV_RECIEVED_DATA[MV_DATA_DIR]=MV_RIGHT;
				//現在地記録
				break;
			case motor::LEFT_TURN:
				MV_RECIEVED_DATA[MV_DATA_DIR]=MV_LEFT;
				//現在地記録
				break;
			case motor::BRAKE:
			case motor::RIGHT_TURN_NO_GYRO:
			case motor::LEFT_TURN_NO_GYRO:
				MV_RECIEVED_DATA[MV_DATA_TYPE]=FIND_NOTHING;
				lcd_clear();
				return;
				break;
			default:
				break;
		}
		if(motor::Task_Before==motor::ONE_ADVANCE||motor::Task_Before==motor::TWO_ADVANCE||motor::Task_Before==motor::HALF_ADVANCE){
			motor::move(motor::RIGHT_TURN);
		}
		led(Redled,1);
		led(Blueled,1);
		led(Greenled,1);
		if(MV_RECIEVED_DATA[MV_DATA_DIR]==MV_LEFT){
			Drop_kit(DROP_LEFT,kit_need);
		}
		else if(MV_RECIEVED_DATA[MV_DATA_DIR]==MV_RIGHT){
			Drop_kit(DROP_RIGHT,kit_need);
		}
		else{
			lcd_clear();
			return;
		}
		motor::set_Status(motor::PAUSE);//モーター止める
		led(Redled,0);
		led(Blueled,0);
		led(Greenled,0);
		error_led(1,1);
		while(KIT_DROP_Status!=FREE){
			error_led(1,3);
			error_led(2,4);
			buzzer();
			HAL_Delay(300);
			xbee.putint(KIT_DROP_COUNT);
			xbee.string("\n\r");
		}
		error_led(1,0);
		error_led(2,0);
		MV_RECIEVED_DATA[MV_DATA_PING]=NOT_FIND_FRONT_WALL;
		MV_RECIEVED_DATA[MV_DATA_TYPE]=FIND_NOTHING;//フラグ初期化
		if(motor::Task_Before==motor::ONE_ADVANCE||motor::Task_Before==motor::TWO_ADVANCE||motor::Task_Before==motor::HALF_ADVANCE){
			motor::move(motor::LEFT_TURN);
		}
	}
	else{//前以外のタスク(本来あり得ない)
		mv_task_check();
	}
	
}

void mv_task_check(void){//waitのループ内の停止を求められるキット投下
	uint8_t kit_need=0;
	switch(MV_RECIEVED_DATA[MV_DATA_TYPE]){
			case 3://H  2kits
				kit_need=2;
				lcd_putstr("Find H!");
				break;
			case 4://S  1kits
				kit_need=1;
				lcd_putstr("Find S!");
				break;
			case 5://U 0kits
				kit_need=0;
				lcd_putstr("Find U!");
				break;
			case 6:
				kit_need=1;
				lcd_putstr("Find Sermo!");
				break;
			case 7:
			case 8:
				kit_need=0;
				break;
			default:
				break;
		}
	if(MV_RECIEVED_DATA[MV_DATA_DIR]==MV_FRONT){
		if(MV_RECIEVED_DATA[MV_DATA_PING]==FIND_FRONT_WALL){
			if(motor::Task_Before==motor::ONE_BACK||motor::Task_Before==motor::TWO_BACK||motor::Task_Before==motor::HALF_BACK){
				MV_RECIEVED_DATA[MV_DATA_DIR]=MV_LEFT;
				motor::set_Status(motor::BACK);//ジョブ巻き戻し
				while(motor::check_task()!=motor::FREE||abs(motor::Right_count)>=Motor_thre||abs(motor::Left_count)>=Motor_thre);
			}
			else{//Jobが後進以外なので終了後落とす
				lcd_clear();
				return;
			}
		}
		else{//前に壁ないので、終了後落とす
			lcd_clear();
			return;
		}
	}
#warning Nakao should write here >> Save mapping >>もともと発見されていたらフラグ初期化後、returnして！ >>2こ
	if(MV_RECIEVED_DATA[MV_DATA_DIR]==MV_FRONT){
		//一個前の座標記録
	}
	else{
		//現在地記録
	}
	if(motor::Task_Save!=motor::BRAKE){//StatusがBACKに入った場合はこの条件に入る
		motor::move(motor::RIGHT_TURN);
	}
	led(Redled,1);
	led(Blueled,1);
	led(Greenled,1);
	if(MV_RECIEVED_DATA[MV_DATA_DIR]==MV_LEFT){
		Drop_kit(DROP_LEFT,kit_need);
	}
	else if(MV_RECIEVED_DATA[MV_DATA_DIR]==MV_RIGHT){
		Drop_kit(DROP_RIGHT,kit_need);
	}
	else{
		lcd_clear();
		return;
	}
	motor::set_Status(motor::PAUSE);//モーター止める
	led(Redled,0);
	led(Blueled,0);
	led(Greenled,0);
	error_led(1,1);
	while(KIT_DROP_Status!=FREE){
		error_led(1,3);
		error_led(2,4);
		buzzer();
		HAL_Delay(300);
		xbee.putint(KIT_DROP_COUNT);
		xbee.string("\n\r");
	}
	error_led(1,0);
	error_led(2,0);
	MV_RECIEVED_DATA[MV_DATA_PING]=NOT_FIND_FRONT_WALL;
	MV_RECIEVED_DATA[MV_DATA_TYPE]=FIND_NOTHING;//フラグ初期化
	if(motor::Task_Save!=motor::BRAKE){//BACKに入っていた場合はこの条件に入って元のマスに戻る。
		motor::set_Status(motor::RETURN);
	}
	return;
}
/*
void throw_kit_at_front(uint8_t lr,uint8_t type){
	if(motor::Task_Before==motor::LEFT_TURN||motor::Task_Before==motor::RIGHT_TURN){
		switch(type){
			case 3://H  2kits
				lcd_clear();
				lcd_putstr("Find H!");
				if(kit_chk(v::H))finded_victim(2,lr);
				//ta.r_now()->type=v::H;
				break;
			case 4://S  1kits
				lcd_clear();
				lcd_putstr("Find S!");
				if(kit_chk(v::S))finded_victim(1,lr);
				//ta.r_now()->type=v::S;
				break;
			case 5://U 0kits
				lcd_clear();
				lcd_putstr("Find U!");
				if(kit_chk(v::U))finded_victim(0,lr);
				//ta.r_now()->type=v::U;
				break;
			case 6:
				lcd_clear();
				lcd_putstr("Find Sermo");
				if(kit_chk(v::sermo))finded_victim(1,lr);
				//ta.r_now()->type=v::sermo;
				break;
			case 7:
			case 8:
				lcd_clear();
				lcd_putstr("Find Error");
				finded_victim(1,lr);
				//ta.r_now()->type=v::r_kit;
				break;
			default:
				break;
		}
	}
	else{
		if(lr==MV_RIGHT){
			motor::move(motor::RIGHT_TURN);
		}
		else if(lr==MV_LEFT){
			motor::move(motor::LEFT_TURN);
		}
		switch(type){
			case 3://H  2kits
				lcd_clear();
				lcd_putstr("Find H!");
				if(kit_chk(v::H))finded_victim(2,lr);
				//ta.r_now()->type=v::H;
				break;
			case 4://S  1kits
				lcd_clear();
				lcd_putstr("Find S!");
				if(kit_chk(v::S))finded_victim(1,lr);
				//ta.r_now()->type=v::S;
				break;
			case 5://U 0kits
				lcd_clear();
				lcd_putstr("Find U!");
				if(kit_chk(v::U))finded_victim(0,lr);
				//ta.r_now()->type=v::U;
				break;
			case 6:
				lcd_clear();
				lcd_putstr("Find Sermo");
				if(kit_chk(v::sermo))finded_victim(1,lr);
				//ta.r_now()->type=v::sermo;
				break;
			case 7:
			case 8:
				lcd_clear();
				lcd_putstr("Find Error");
				finded_victim(1,lr);
				//ta.r_now()->type=v::r_kit;
				break;
			default:
				break;
		}

		if(lr==MV_RIGHT){
			motor::move(motor::LEFT_TURN);
		}
		else if(lr==MV_LEFT){
			motor::move(motor::RIGHT_TURN);
		}
		if(motor::Task_Save!=motor::BRAKE){
			motor::set_Status(motor::RETURN);
		}
	}
}
*/
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
	mv_serial.string("\n\r");
	if(kit_chk()==false){
		//モーター再開
		lcd_clear();
		led(Blueled,0);
		lcd_putstr("return");
		buzzer(400);
		return 0;
	}
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
	led(Blueled,0);
	HAL_Delay(2);
	motor::set_Status(motor::RESTART);
	return 1;
}


