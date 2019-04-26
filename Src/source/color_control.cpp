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
uint16_t red=0, green=0, blue=0;	//�l
uint16_t high=800, low=500;	//�������l ��>800  700<��<=800

void color_read(void){	//�Z���T�[�̒l��gree,red,blue�ɑ�����Ă����
	red=0, green=0, blue=0;
	HAL_GPIO_WritePin(GATE_GPIO_Port,GATE_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(CK_GPIO_Port,CK_Pin|RANGE_Pin,GPIO_PIN_RESET);
	HAL_Delay(2);
	HAL_GPIO_WritePin(DOUT_GPIO_Port,GATE_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(CK_GPIO_Port,RANGE_Pin,GPIO_PIN_RESET);
	HAL_Delay(20);	//�����odelay
	HAL_GPIO_WritePin(DOUT_GPIO_Port,GATE_Pin,GPIO_PIN_RESET);
	_delay_us(4);
	red=color_shift();
	green=color_shift();
	blue=color_shift();
	HAL_GPIO_WritePin(DOUT_GPIO_Port,GATE_Pin,GPIO_PIN_SET);
}

uint16_t color_shift(void){//12�r�b�g���̃p���X���M�Ɠǂݍ��ݏ���
	uint16_t result=0;//���o���ʗp�̕ϐ���p�Ӂi0�F�������j
	for(uint8_t i=0;i<12;i++){//12�r�b�g���̌J��Ԃ�����
		HAL_GPIO_WritePin(CK_GPIO_Port,CK_Pin|RANGE_Pin,GPIO_PIN_SET);
		_delay_us(1);
		if(HAL_GPIO_ReadPin(DOUT_GPIO_Port,DOUT_Pin)!=0){//Dout�[�q����̏o�͂�High�̏ꍇ
			result+=(1<<i);//12�r�b�g��i���ڂ�1�����ii�����������ɃV�t�g�j
		}
		HAL_GPIO_WritePin(CK_GPIO_Port,CK_Pin|RANGE_Pin,GPIO_PIN_RESET);//1�r�b�g���̃N���b�N�p���X�o�́iLOW�j
		_delay_us(1);
	}
	_delay_us(3);//3�}�C�N���b�ҋ@
	return result;//���ʂ��o��
}

uint8_t color_check(void){	//�J���[�`�F�b�N 0:�� 1:�N�� 2:��i�`�F�b�N�|�C���g�j�@�@return�ŋA���Ă��܂��B  //�܂��l��܂�Ȃ��I
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
