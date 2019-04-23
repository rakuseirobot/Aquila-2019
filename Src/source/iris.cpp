/*
 * iris.cpp
 * Created: 2018/12/31 12:39:15
 *  Author: emile
 */ 
#include "iris.hpp"
#include "color_control.hpp"
#include "motor_control.hpp"
#include "lcd_control.hpp"
#include "ping_control.hpp"
#include "mv_control.hpp"
#include "JY901.hpp"
#include "action.hpp"
#include "uart_control.hpp"
#include "ui_control.hpp"
#include <stdint.h>
#define ping_flag true //trueなら2マス先も見る

uint16_t box[1025][10];

extern uart xbee;
uart iris_serial = xbee;
extern uart serial;

void serial_send_node(node* n){
    iris_serial.string("--------------------");
    iris_serial.string("\n\r");
    iris_serial.string("*[adress] : ");
    iris_serial.putint((int)n);
    iris_serial.string("\n\r");
    iris_serial.string("*[x,y,z] = ");
    iris_serial.putint((int)n->x);
    iris_serial.string(",");
    iris_serial.putint((int)n->y);
    iris_serial.string(",");
    iris_serial.putint((int)n->z);
    iris_serial.string("\n\r");
    iris_serial.string("*[type] : ");
    iris_serial.putint((int)n->type);
    iris_serial.string("\n\r");
    iris_serial.string("*[flag] : ");
    iris_serial.putint((int)n->flag);
    iris_serial.string("\n\r");
    iris_serial.string("*[color] : ");
    iris_serial.putint((int)n->color);
    iris_serial.string("\n\r");
    iris_serial.string("--------------------");
    iris_serial.string("\n\r");
}

void make_nodes(){
    iris_serial.string("make_nodes\n\r");
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
        motor::move(motor::ONE_BACK);
        motor::fix_position(v::back);
        ta.turn_l();
        ta.turn_l();
        ta.go_st();
        ta.turn_l();
        ta.turn_l();
    }
}

void nachylenie(uint8_t x){/*old ver. Do not use!*/
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
        node* x = ta.mall.make(100,100,ta.r_now()->z+1,(ta.r_flg()+1)%2);x->dist=255;ta.ins_node(x);
        ta.r_now()->type=v::slope;ta.r_now()->color=color::black;
        ta.cn_graph(ta.r_now(),x);
        x->type=v::slope;x->color=color::black;
        ta.w_now(x);
        ta.ap_node(ta.r_now(),v::front);
        ta.go_st();ta.r_now()->color=color::black;
        ta.stk.push(ta.r_now());
        make_nodes();
    }else if(motor::notify_long_acc(x)==1){//下り
        node* x = ta.mall.make(100,100,ta.r_now()->z-1,(ta.r_flg()+1)%2);x->dist=255;ta.ins_node(x);
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
    iris_serial.putint(x);
    if(x==v::front){
        motor::move(motor::HALF_ADVANCE);
    }
    else{
        motor::move(motor::HALF_BACK);
    }
    motor::gb_fix();
    motor::turn_fix();
    iris_serial.string("nac2");
    if(ta.r_now()->type==v::slope){
        iris_serial.string("a");
        node* t = ta.r_now();/*空間計算量を抑える為に使いまわす*/
        rep(i,4){ if(t->next[i]->type==v::slope){t=t->next[i];break;} }/*node_aの探索*/
        rep(i,4){ if(t->next[i]->type!=v::slope){t=t->next[i];break;} }/*node_bの探索*/
        ta.w_now(t);
    }else{/*make_nodesよりも後だと、こちらが面倒*/
        iris_serial.string("b");
        ta.r_now()->type=v::slope;
        node* t = ta.ac_next(v::back,1);/*t=nowの一つ前のnode*/
        uint8_t zz = ta.r_now()->z;
        if(flag == 2){zz++;}else{zz--;}
        rep(i,4){ 
            if(ta.r_now()->next[i]==np){
                ta.r_now()->next[i]=ta.mall.make(t->x,t->y,zz,(ta.r_flg()+1)%2); 
                t=ta.r_now()->next[i];/*t=node_a*/
                t->next[0]=ta.r_now();
                iris_serial.string("c");
                break;
            }
        }
        ta.w_now(t);t->type=v::slope;
        ta.go_st();/*node_b*/
        iris_serial.string("nac2:end:");
        serial_send_node(ta.r_now());
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

bool blind_alley(int x){ /*what's this?*/
    if(x!=v::back)return false;
    node* t = ta.r_now();
    if(t->type==v::black||t->type==v::normal)return false;
    uint8_t ct = 0;
    rep(i,4){ if(t->next[i]!=np)ct++; }
    if(ct==1)return true;
    else return false;
}

void move(int num){//num::0:turn_l(90deg)+go_st,1:go_st,2:turn_r(90deg)+go_st,4:back(turn),3:back(usiro)
    motor::Right_count=0;
    motor::Left_count=0;
    motor::wait();
    serial.putint(num);
    serial.string("\n\r");
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
            ta.turn_l(); ta.turn_l();
            ta.go_st();
            ta.turn_l(); ta.turn_l();
            motor::move(motor::ONE_BACK);
            motor::fix_position(v::back);
            break;
        default:
            break;
    }
    iris_serial.string("now:: ");
    serial_send_node(ta.r_now());
    if(ta.r_now()!=ta.r_start())ta.r_now()->color=color::black;
    black_tile();
    if(num==v::back){ nachylenie2(v::back); }else{ nachylenie2(v::front); }
    make_nodes();
    if(ta.r_now()->type==v::unknown){ta.r_now()->type = v::normal;}
    motor::wait();
}

void move_n(node* n){//move to neighborhood((node*)n)
    if(n!=np){
        iris_serial.string("m_n [start]");
        rep(i,4)if(ta.ac_next(i,1)==n && ta.ck_conect(ta.r_now(),ta.ac_next(i,1)) && ta.ac_next(i,1)->type!=v::black){
            move(i);
            lcd_clear();
            lcd_putstr("[*]m_n");
            iris_serial.string("in m_n \n\r");
            break;
        }
        iris_serial.string("m_n [end]\n\r");
    }
}

void move_toa(node* a){//move to (node*)a
    lcd_clear();
    ta.clear_dist();
    ta.bfs(a,ta.r_now());
    bl fg;
    iris_serial.string("\n\r__ m_a [START]\n\r");
    iris_serial.string("= (node*)a = ");
    serial_send_node(ta.r_now());
    while(ta.r_now()!=a && a->type!=v::black && a->type!=v::slope){
        fg = false;
        rep(i,4){
            iris_serial.string("[*_] in m_a \n\r");
            iris_serial.putint((int)a);
            if(!fg && ta.ac_next(i,1)!=np && ta.ck_conect(ta.r_now(),ta.ac_next(i,1)) && ta.ac_next(i,1)->dist<ta.r_now()->dist && ta.ac_next(i,1)->type!=v::black){ move_n(ta.ac_next(i,1)); fg=true; }
            if(ta.find(a->x,a->y,a->z)->type==v::slope)fg=true;
        }
    }
    iris_serial.string("[**] in m_a *b\n\r");
    ta.clear_dist();
    lcd_clear();
    iris_serial.string("m_a [END]\n\r");
    lcd_putstr("end");
}

void stack_dfs(){	
	iris_serial.string("dfs [START]\n\r");
    serial_send_node(ta.r_now());
	ta.stk.push(ta.r_start());
	ta.r_start()->color=color::gray;
	make_nodes();
	bl fg;/*for ??*/ 
	while(!ta.stk.empty()){
        iris_serial.string("dfs::[___]\n\r");
		if(ta.r_now()!=ta.r_start())ta.r_now()->color=color::black;
		//node追加 normal ver
        for(int i=3;i>=0;i--){
			if(ta.ac_next(i,1)!=np && ta.ac_next(i,1)->color==color::white && ta.ck_conect(ta.r_now(),ta.ac_next(i,1))){
				ta.stk.push(ta.ac_next(i,1));
				ta.ac_next(i,1)->color=color::gray;
			}
		}//node追加部分ここまで
		lcd_clear(); lcd_putstr("dfs");
		fg=false;
		while(!fg){
			if(ta.stk.top()->color!=color::black && !ta.stk.empty() && ta.stk.top()!=np){
				iris_serial.string("*dfs : stk_top ===");
				iris_serial.putint((int)ta.stk.top());
				iris_serial.string("\n\r");
				iris_serial.string("dfs::[*__]\n\r");
                move_toa(ta.stk.top());
				lcd_clear(); lcd_putstr("dfs");
                iris_serial.string("\n\r");
                iris_serial.string("dfs::[**_]\n\r");
				fg=true;
			}else{ ta.stk.pop(); }
		}//sub loop
        iris_serial.string("dfs::[***]\n\r");
	}//main loop
	iris_serial.string("dfs [END] \n\r");
	lcd_clear(); lcd_putstr("end_dfs");
}

void _stack_dfs(){	
	iris_serial.string("dfs [START]\n\r");
    serial_send_node(ta.r_now());
	ta.stk.push(ta.r_start());
	ta.r_start()->color=color::gray;
	make_nodes();
	bl fg;/*for ??*/ 
	while(!ta.stk.empty()){
		if(ta.r_now()!=ta.r_start())ta.r_now()->color=color::black;
		//node追加 normal ver
        for(int i=3;i>=0;i--){
			if(ta.ac_next(i,1)!=np && ta.ac_next(i,1)->color==color::white && ta.ck_conect(ta.r_now(),ta.ac_next(i,1))){
				ta.stk.push(ta.ac_next(i,1));
				ta.ac_next(i,1)->color=color::gray;
			}
		}//node追加部分ここまで
		lcd_clear(); lcd_putstr("dfs");
		fg=false;
		while(!fg){
            iris_serial.string("dfs::[___]\n\r");
            ta.clear_dist(); ta.bfs(ta.r_now(),ta.r_start()); /*for t_top()*/
			if(ta.stk.t_top()->color!=color::black && !ta.stk.empty() && ta.stk.t_top()!=np){
				iris_serial.string("*dfs : stk_top ===");
				iris_serial.putint((int)ta.stk.top());
				iris_serial.string("\n\r");
				iris_serial.string("dfs::[*__]\n\r");
				move_toa(ta.stk.t_top());
				lcd_clear(); lcd_putstr("dfs");
                iris_serial.string("\n\r");
                iris_serial.string("dfs::[**_]\n\r");
				fg=true;
			}else{ ta.stk.t_pop(); }
		}//sub loop
        iris_serial.string("dfs::[***]\n\r");
	}//main loop
	iris_serial.string("dfs [END]\n\r");
    iris_serial.string("END ALL PROCEDURE.\n\r");
    iris_serial.string("Thank you!! :) \n\r");
	lcd_clear(); lcd_putstr("end_dfs");
}