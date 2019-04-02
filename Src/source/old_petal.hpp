/*
 * petal.cpp
 *
 * Created: 2018/08/25 0:59:49
 *  Author: TOMOKI
 */ 
 #ifndef PETAL_HPP_
 #define PETAL_HPP_

 #include "avr/io.h"
 //#include "core.hpp"
 #include "test_core.hpp"
 #include "initializing.hpp"
 #include "ui_control.hpp"
 #include "lcd_control.hpp"
 #include "motor_control.hpp"
 #include "action.hpp"
 #include "color_control.hpp"
 #include "sermo_control.hpp"
 #include "serial.hpp" //荳逡ｪ譛蠕後↓・・ｼ・#include <avr/eeprom.h>
 #include "mv_control.hpp"
 #include "util/delay.h"
 //usart serial(&USARTC0,&PORTC);

core ta;

void for_w_w(int direc){
	switch(check_ping(direc)){
		case 1: //横に壁
			ta.w_wall(direc,v::wall);
			break;
		case 2: //隣のマスに壁がある
			ta.append_node(direc,1);
			ta.w_wall(direc,v::nowall);
			//ta.w_wall(ta.ac_next(direc,1),direc,v::wall);//隣のマスに壁をwrite
			break;
		case 3: //2つ隣のマスに壁がある
			ta.append_node(direc,1);
			ta.append_node(ta.ac_next(direc,1),direc,ta.r_dir(),1);//隣の隣のマス作成
// 			ta.w_wall(direc,v::nowall);
// 			ta.w_wall(ta.ac_next(direc,1),direc,v::nowall);
			break;
		case -4: //3つ隣のマスに壁がある
			ta.append_node(direc,1);
			ta.append_node(ta.ac_next(direc,1),direc,ta.r_dir(),1);//隣の隣のマス作成
			ta.append_node(ta.ac_next(direc,2),direc,ta.r_dir(),1);//隣の隣の隣のマス作成
			ta.w_wall(direc,v::nowall);
			ta.w_wall(ta.ac_next(direc,1),direc,v::nowall);
			ta.w_wall(ta.ac_next(direc,2),direc,v::nowall);
			ta.w_wall(ta.ac_next(direc,3),direc,v::wall);
			break;
		case -5: //4つ隣のマスに壁がある,実はcase 2:と全く同じ。
			ta.append_node(direc,1);
			ta.w_wall(direc,v::nowall);
			ta.w_wall(ta.ac_next(direc,1),direc,v::wall);//隣のマスに壁をwrite
			break;
		case 0:
		//case 2:
		//case 3:
		case 4:
		case 5:
			ta.append_node(direc,1);
			ta.append_node(ta.ac_next(direc,1),direc,ta.r_dir(),1);//隣の隣のマス作成
			ta.w_wall(direc,v::nowall);
			break;
		
	}
}

void write_walls(){
	for_w_w(v::left);
	for_w_w(v::front);
	for_w_w(v::right);
	for_w_w(v::back);
}

void blacktile(){
	if(color_check()==1){
		write_walls();
		ta.r_now()->type=v::black;
		motor::move(v::back);
	}else{/*no action*/}	
}

void move(int num){//num::0:turn_l(90deg),1:go_st,2:turn_r(90deg),3:back(turn),4:back(usiro)
// 		serial.string("x,y,n :");
// 		serial.putdec(ta.r_now()->x);
// 		serial.string("\n\r");
// 		serial.putdec(ta.r_now()->y);
// 		serial.string("\n\r");
// 		serial.putdec((uint16_t)ta.r_now());
// 		serial.string("\n\r");
// 		serial.string("\n\r");
	switch(num){
		case 0:
			mv_cap(1,true);
			mv_cap(2,true);
			mv_cap(3,true);
			motor::move(9);
			ta.turn_l();
			break;
		case 1:
			mv_cap(1,true);
			mv_cap(2,true);
			mv_cap(3,true);
			motor::move(0);
			ta.go_st();
			break;
		case 4:
			mv_cap(1,true);
			mv_cap(2,true);
			mv_cap(3,true);
			motor::move(4);
			ta.turn_l();
			ta.turn_l();
			ta.go_st();
			ta.turn_l();
			ta.turn_l();
			break;
		case 2:
			mv_cap(1,true);
			mv_cap(2,true);
			mv_cap(3,true);
			motor::move(8);
			ta.turn_r();
			break;
		case 3:
			mv_cap(1,true);
			mv_cap(2,true);
			mv_cap(3,true);
			motor::move(9);
			motor::fix_position();
			motor::move(9);
			motor::fix_position();
			motor::move(0);
			ta.turn_r();
			ta.turn_r();
			ta.go_st();
			break;
		default:
			break;
	}
	write_walls();
	if(ta.r_now()->type==v::unknown){ta.r_now()->type = v::normal;}
	if(ta.r_now()->type==v::hisai){mv_cap(1,false);mv_cap(2,false);mv_cap(3,false);}
	if(k_r_read()==6 && ta.r_now()->type==v::normal){ ta.r_now()->type=v::hisai; }//ondo
	k_r_write(0);
	lcd_putdec(LCD1_TWI,ta.r_now()->x);
	_delay_ms(500);
	lcd_putdec(LCD1_TWI,ta.r_now()->y);
	if(color_check()==1){//kuro
		ta.r_now()->type=v::black;
		write_walls();
		if(ta.ac_next(v::front,1)==ta.r_now()->back[0]){
			motor::move(0);		
			ta.go_st();
		}else if(ta.ac_next(v::back,1)==ta.r_now()->back[0]){
			motor::move(4);
			ta.turn_l();
			ta.turn_l();
			ta.go_st();	
			ta.turn_l();
			ta.turn_l();
		}else{

		}		
	}
	led(Greenled,1);
	motor::fix_position();
	led(Blueled,1);
	lcd_clear();
}

void nachylenie(){//多分モーター回した後に入れるべきやつ。//色々改善の余地がある。
	if(motor::notify_long_ex()==1){ //F():坂判定機,今坂の上にいるかどうかがわかる。
		ta.r_now()->type=v::slope;
		node*t=ta.r_now()->back[0];
		ta.move_to(t->x,t->y,t->z+1);
	}else if(ta.r_now()->type==v::slope){
		node* t=ta.r_now()->back[0];
		ta.w_now(t);
	}else{
		//no action
	}
}

node* check_node(node* x){
	node* tmp= x;
	lcd_clear();
	lcd_putstr(LCD1_TWI,"ee");
	_delay_ms(100);
	while(tmp->type!=v::start){
		for(int i=0;i<4;i++){
			if(tmp->next[i] != np && tmp->next[i]->type == v::unknown){
				tmp=tmp->next[i];
// 				serial.string("c_n: ");
// 				serial.putdec((uint16_t)tmp);
// 				serial.string("\n\r");
				return tmp;
			}
		}
		if(tmp->back[0]!=np){
			tmp = tmp->back[0];
		}else{
			ta.find(1,1,100);
			for(int i=0;i<4;i++){
				if(tmp->next[i]->depth < tmp->depth){
					tmp = tmp->next[i];
					break;
				}
			}
		}
	}
	serial.string("c_n: ");
	serial.putdec((uint16_t)tmp);
	serial.string("\n\r");
	return tmp;
}

bool movetoa(node* a){//move to A. If A is neighbor of now_node , move to A.
	//lcd_putstr(LCD1_TWI,"MtoA");
	if(a==np)return false;
	if(ta.ac_next(v::left,1)==a && ta.r_wall(v::left)!= v::wall){
		lcd_clear();
		move(v::left);
		move(v::front);
		lcd_clear();
		return true;
	}else if(ta.ac_next(v::front,1)==a && ta.r_wall(v::front) != v::wall ){
		move(v::front);
		lcd_clear();
		return true;
	}else if(ta.ac_next(v::right,1)==a && ta.r_wall(v::right) != v::wall ){
		move(v::right);
		move(v::front);
		lcd_clear();
		return true;
	}else if(ta.ac_next(v::back,1)==a && ta.r_wall(v::back) != v::wall ){
		move(4);
		lcd_clear();
		return true;
	}else{
		//error
		return false;
	}
}

void goback(node *saki){
	if(ta.r_now()==saki){
		//end
	}else{
		for(int i=0;i<4;i++){
			if(ta.ac_next(i,1)->hosu +1 == ta.r_now()->hosu){	movetoa(ta.ac_next(i,1)); goback(saki);  break;  }
		}
	}
}

void go_bfs(node* x){
	lcd_clear();
// 	serial.string("bfsx:");
// 	serial.putdec((uint16_t)x);
// 	serial.string("\n\r");
	ta.bfs_type(x,v::damy);
	lcd_putstr(LCD1_TWI,"bfs");
	lcd_putdec(LCD1_TWI,ta.r_now()->hosu);
	node* tmp=np;bool flg=false;
	while(ta.r_now()->hosu!=0){
		tmp = ta.r_now();flg=false;
// 		serial.string("tmp : ");
// 		serial.putdec((uint16_t)tmp);
// 		serial.string("\n\r");
		for(int i=0;i<4;i++){
			if(tmp->next[i]!=np && tmp->next[i]->hosu < tmp->hosu && !flg){
// 				serial.string("m_a :");
// 				serial.putdec((uint16_t)tmp->next[i]);
// 				serial.string("\n\r");
				tmp=tmp->next[i];
				movetoa(tmp);	
				flg=true;
			}
		}
	}
}

void real_dfs(node* t,node* s){
	write_walls();
	lcd_clear();
	lcd_putstr(LCD1_TWI,"dfs");
	node* a;bool flg=false;
	for(int i=0;i<4;i++){
		flg=false;
		for(int j=0;j<4;j++){
			a = ta.ac_next(s,ta.r_dir(),i,1);
			if(!flg && a!=np && a->type==v::unknown){ 
				flg=true;
				if(movetoa(a))real_dfs(s,ta.r_now()); 
			}
		}
	}
	lcd_clear();
	//if(s==ta.r_now() && t!=np)go_bfs(check_node(t));
	go_bfs(check_node(ta.r_now()));
	lcd_clear();
	//if(t!=np)movetoa(t);
	//if(t!=np)gobacktoa(t);
}

void stack_dfs(node* t){
	if(t!=np){
		st.push(t);
		write_walls();
		node* k;node* nw;
		for(int	i=3;i>=0;i--){
			k = find(ta.ac_next(i,1)->x,ta.ac_next(i,1)->y,ta.ac_next(i,1)->z);
			if(k!=np && k->type==v::unknown)st.push(k);
		}
		while(!st.empty()){
			nw = ta.find(st.top()->x,st.top()->y,st.top()->z);
			if(nw->type==v::unknown)go_bfs(nw);
			st.pop();
			for(int	i=3;i>=0;i--){
				k = find(ta.ac_next(i,1)->x,ta.ac_next(i,1)->y,ta.ac_next(i,1)->z);
				if(k!=np && k->type==v::unknown)st.push(k);
			}	
		}
	}
}

#endif