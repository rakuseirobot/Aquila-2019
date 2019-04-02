/*
 * iris.cpp
 * Created: 2018/12/31 12:39:15
 *  Author: emile
 */ 
#include "iris.hpp"
#include "initializing.hpp"
#include "color_control.hpp"
#include "motor_control.hpp"
#include "lcd_control.hpp"
#include "ping_control.hpp"
#include "mv_control.hpp"
#include "gyro_control.hpp"
#include "action.hpp"
//using namespace motor;
#define ping_flag true //trueなら2マス先も見る

void make_nodes(){
	serial.string("make_nodes\n");
    if(!ta.r_now()->ac){
		rep(i,4){
			if(check_ping(i)>1){
				if(ta.ac_next(i,1)==np){ ta.ap_node(ta.r_now(),i); }else{ ta.cn_graph(ta.r_now(),ta.ac_next(i,1)); }
				if(check_ping(i)>2 && ping_flag){//2マス先も見る.
					if(ta.ac_next(i,2)==np){ ta.ap_node(ta.ac_next(i,1),i); }else{ ta.cn_graph(ta.ac_next(i,1),ta.ac_next(i,2)); }
				}
			}
		}
		ta.r_now()->ac=true;
    }
}

void black_tile(){
	if(color_check()==1 || ta.r_now()->type==v::black){
		ta.r_now()->type=v::black;
		ta.r_now()->color=v::black;
		motor::move(motor::ONE_ADVANCE);
		motor::fix_position(v::back);
		ta.turn_l();
		ta.turn_l();
		ta.go_st();
		ta.turn_l();
		ta.turn_l();
	}
}

void nachylenie(uint8_t x){
	if(ta.r_now()->type==v::slope){
		motor::notify_long_acc(x);
		rep(i,4){
			if(ta.r_now()->next[i]->type==v::slope){
				node* t=ta.r_now()->next[i];
				rep(j,4){
					if(t->next[j]->type==v::normal){
						ta.w_now(t->next[j]);
					}
				}
			}
		}
	}else if(motor::notify_long_acc(x)==2){//昇り
		node* x = ta.mall.make(100,100,ta.r_now()->z+1,(ta.r_flg()+1)%2);x->dist=1000;ta.ins_node(x);
		ta.r_now()->type=v::slope;ta.r_now()->color=color::black;
		ta.cn_graph(ta.r_now(),x);
		x->type=v::slope;x->color=color::black;
		ta.w_now(x);
		ta.ap_node(ta.r_now(),v::front);
		ta.go_st();ta.r_now()->color=color::black;
		ta.stk.push(ta.r_now());
		make_nodes();
	}else if(motor::notify_long_acc(x)==1){//下り
		node* x = ta.mall.make(100,100,ta.r_now()->z-1,(ta.r_flg()+1)%2);x->dist=1000;ta.ins_node(x);
		ta.r_now()->type=v::slope;ta.r_now()->color=color::black;
		ta.cn_graph(ta.r_now(),x);
		x->type=v::slope;x->color=color::black;
		ta.w_now(x);
		ta.ap_node(ta.r_now(),v::front);
		ta.go_st();ta.r_now()->color=color::black;
		ta.stk.push(ta.r_now());
		make_nodes();
	}else{
		
	}
}

bool nachylenie2(uint8_t x){/*make_nodesよりも前に使う*/
	if(x!=v::back){x=v::front;};
	uint8_t flag = motor::notify_long_ang(x);/*flag=1:下り、=2:上り、=0:無し*/
	if(flag==0)return false;
	motor::brake(motor::MOTOR_LEFT);
	motor::brake(motor::MOTOR_RIGHT);
	serial.putint(x);
	if(x==v::front){
		motor::move(motor::HALF_ADVANCE);
	}
	else{
		motor::move(motor::HALF_BACK);
	}
	motor::gb_fix();
	motor::turn_fix();
	serial.string("nac2");
	if(ta.r_now()->type==v::slope){
		serial.string("a");
		node* t = ta.r_now();/*空間計算量を抑える為に使いまわす*/
		rep(i,4){ if(t->next[i]->type==v::slope){t=t->next[i];break;} }/*node_aの探索*/
		rep(i,4){ if(t->next[i]->type!=v::slope){t=t->next[i];break;} }/*node_bの探索*/
		ta.w_now(t);
	}else{/*make_nodesよりも後だと、こちらが面倒*/
		serial.string("b");
		ta.r_now()->type=v::slope;
		node* t = ta.ac_next(v::back,1);/*t=nowの一つ前のnode*/
		uint8_t zz = ta.r_now()->z;
		if(flag == 2){zz++;}else{zz--;}
		rep(i,4){ 
			if(ta.r_now()->next[i]==np){
				ta.r_now()->next[i]=ta.mall.make(t->x,t->y,zz,(ta.r_flg()+1)%2); 
				t=ta.r_now()->next[i];/*t=node_a*/
				t->next[0]=ta.r_now();
				serial.string("c");
				break;
			}
		}
		ta.w_now(t);t->type=v::slope;
		ta.go_st();/*node_b*/
		serial.string("nac2:end:");
		serial.putint((int)ta.r_now());
		serial.string(",");
		serial.putint((int)ta.r_now()->x);
		serial.string(",");
		serial.putint((int)ta.r_now()->y);
		serial.string(",");
		serial.putint((int)ta.r_now()->z);
		serial.string(",");
		serial.putint((int)ta.r_now()->type);
		serial.string("\n");
		serial.string("find :");
		serial.putint((int)ta.find(ta.r_now()->x,ta.r_now()->y,ta.r_now()->z));
		serial.string("\n");
		//ta.stk.pop();//????its test.
	}
	return true;
}
/*
 * 上向きなら、
 * (そこに頂点が存在しない時)
 * ・上る
 * ・現在のノード(now)のstatusを坂に、
 * ・now(x,y,z)(階段中)->node_a(node_bからバックするイメージの場所,2階)->node_b(x,y,z+1)のように接続。
 * ・現在の位置(上った先)をnode_bにする。
 * (存在する時)
 * ・上る
 * ・now(階段中)->node_a(2階の階段中)->node_bの順に進む。(自明に定数時間なのでダブリングは必要ない)
 * ・現在地をnode_bに変更
 */

bool blind_alley(int x){
	if(x!=v::back)return false;
	node* t = ta.r_now();
	if(t->type==v::black||t->type==v::normal)return false;
	uint8_t ct = 0;
	rep(i,4){ if(t->next[i]!=np)ct++; }
	if(ct==1)return true;
	else return false;
}

void move(int num){//num::0:turn_l(90deg)+go_st,1:go_st,2:turn_r(90deg)+go_st,4:back(turn),3:back(usiro)
	motor::wait();
	switch(num){
		case 0:
			ta.turn_l();
			motor::move(motor::LEFT_TURN);
			motor::fix_position();
			ta.go_st();
			motor::move(motor::ONE_ADVANCE);
			motor::fix_position();
			break;
		case 1:
			ta.go_st();
			motor::move(motor::ONE_ADVANCE);
			motor::fix_position();
			break;
		case 2:
			ta.turn_r();
			motor::move(motor::RIGHT_TURN);
			motor::fix_position();
			ta.go_st();
			motor::move(motor::ONE_ADVANCE);
			motor::fix_position();
			break;
		case 4:
			ta.turn_r();
			motor::move(motor::LEFT_TURN);
			motor::fix_position();
			ta.turn_r();
			motor::move(motor::LEFT_TURN);
			motor::fix_position();
			ta.go_st();
			motor::move(motor::ONE_ADVANCE);
			motor::fix_position();
			break;
        case 3:
			ta.turn_l();
			ta.turn_l();
			ta.go_st();
			ta.turn_l();
			ta.turn_l();
			motor::move(motor::ONE_BACK);
			motor::fix_position(v::back);
			break;
		default:
			break;
	}
	serial.string("now:: ");
	serial.putint((int)ta.r_now());
	serial.string(" -> ");
	serial.putint(ta.r_now()->x);
	serial.string(" , ");
	serial.putint(ta.r_now()->y);
	serial.string(" , ");
	serial.putint(ta.r_now()->z);
	serial.string(" : ");
	serial.putint(ta.r_now()->type);
	serial.string(" : ");
	serial.putint(ta.r_now()->color);
	serial.string("\n");
	if(ta.r_now()!=ta.r_start())ta.r_now()->color=color::black;
	black_tile();
	if(num==v::back){ nachylenie2(v::back); }else{ nachylenie2(v::front); }
	make_nodes();
// 	if(blind_alley(num)){
// 		ta.turn_r();
// 		motor::move(motor::RIGHT_TURN);
// 		motor::fix_position();
// 		ta.turn_l();
// 		motor::move(motor::LEFT_TURN);
// 		motor::fix_position();
// 	}
	if(ta.r_now()->type==v::unknown){ta.r_now()->type = v::normal;}
	motor::wait();
}

void move_n(node* n){//move to neighborhood((node*)n)
	if(n!=np){
		serial.string("m_n");
        rep(i,4)if(ta.ac_next(i,1)==n && ta.ck_conect(ta.r_now(),ta.ac_next(i,1)) && ta.ac_next(i,1)->type!=v::black){
	        move(i);
			lcd_clear();
			lcd_putstr(LCD1_TWI,"m_n");
			serial.string("-a");
			break;
		}
		serial.string("mn-end\n");
    }
}

void move_toa(node* a){//move to (node*)a
	lcd_clear();
	ta.clear_dist();
	ta.bfs(a,ta.r_now());
	bl fg;
	serial.string("m_a:a=");
	serial.putint((int)a);	
	serial.string("\n");
	serial.putint(a->x);
	serial.string("\n");
	serial.putint(a->y);
	serial.string("\n");
	serial.putint(a->z);
	serial.string("\n");
	serial.string("\n");
	while(ta.r_now()!=a && a->type!=v::black && a->type!=v::slope){
		fg = false;
		rep(i,4){
			serial.string("-aa");
			serial.putint((int)a);
			if(!fg && ta.ac_next(i,1)!=np && ta.ck_conect(ta.r_now(),ta.ac_next(i,1)) && ta.ac_next(i,1)->dist<ta.r_now()->dist && ta.ac_next(i,1)->type!=v::black){ move_n(ta.ac_next(i,1)); fg=true; }
			if(ta.find(a->x,a->y,a->z)->type==v::slope)fg=true;
		}
	}
	serial.string("-b");
	ta.clear_dist();
	lcd_clear();
	serial.string("ma-end\n");
	lcd_putstr(LCD1_TWI,"end");
}

void stack_dfs(){	
	serial.string("start: ");
	serial.putint((int)ta.r_now());
	serial.string("\n");
	serial.putint(ta.r_now()->x);
	serial.string("\n");
	serial.putint(ta.r_now()->y);
	serial.string("\n");
	serial.putint(ta.r_now()->z);
	serial.string("\n");
	serial.string("-------------\n");
	ta.stk.push(ta.r_start());
	ta.r_start()->color=color::gray;
	make_nodes();
	bl fg;/*for ??*/ float range_tmp[4]={0.0};//for node tuika
	uint8_t tmp2;uint8_t tmp[4]={0,1,2,3};//for node tuika
	while(!ta.stk.empty()){
		if(ta.r_now()!=ta.r_start())ta.r_now()->color=color::black;
		/*for(int j=1;j>=0;j--){ //node追加 old ver
			for(int i=3;i>=0;i--){
				if(check_ping(i)>j+1)if(ta.ac_next(i,1)!=np && ta.ac_next(i,1)->color==color::white 
																	&& ta.ck_conect(ta.r_now(),ta.ac_next(i,1))){
					serial.string("m");
					ta.stk.push(ta.ac_next(i,1));
					ta.ac_next(i,1)->color=color::gray;
				}
			}
		}*///node追加部分
		for(int i=3;i>=0;i--){//node追加 normal ver
			if(ta.ac_next(i,1)!=np && ta.ac_next(i,1)->color==color::white && ta.ck_conect(ta.r_now(),ta.ac_next(i,1))){
				ta.stk.push(ta.ac_next(i,1));
				ta.ac_next(i,1)->color=color::gray;
			}
		}
		//node追加部分ここから
		/*for(int i=3;i>=0;i--){//node追加 new ver
			if(ta.ac_next(i,1)!=np && ta.ac_next(i,1)->color==color::white && ta.ck_conect(ta.r_now(),ta.ac_next(i,1))){
				range_tmp[i]=ta.range_size(ta.ac_next(i,1),i);
			}else{ range_tmp[i] = -1.0;}
		}
		rep(i,4)tmp[i]=i;//init
		rep(i,3){
			rep(j,i)if(range_tmp[tmp[j]]<range_tmp[tmp[j+1]]){//sort
				tmp2 = tmp[j+1]; tmp[j+1] = tmp[j]; tmp[j]=tmp2;//swap
			}
			if(range_tmp[tmp[i+1]]>=0.0){//push
				ta.stk.push(ta.ac_next(tmp[i+1],1));
				ta.ac_next(tmp[i+1],1)->color=color::gray;
			}
		}
		if(range_tmp[tmp[0]]>=0.0){
			ta.stk.push(ta.ac_next(tmp[0],1));
			ta.ac_next(tmp[0],1)->color=color::gray;
		}
		//node追加部分ここまで
		*/serial.string("n");
		lcd_clear();
		lcd_putstr(LCD1_TWI,"dfs");
		fg=false;
		while(!fg){
			if(ta.stk.top()->color!=color::black && !ta.stk.empty() && ta.stk.top()!=np){
				serial.string("dfs2 : ");
				serial.putint((int)ta.stk.top());
				serial.string("\n");
				move_toa(ta.stk.top());
				lcd_clear();
				lcd_putstr(LCD1_TWI,"dfs2");
				serial.string("s_dfs\n");
				fg=true;
			}else{ ta.stk.pop(); }
		}
		serial.string("end");
	}
}