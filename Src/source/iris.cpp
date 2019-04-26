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
#define serial_send_node_suru true //suru ?

extern uart xbee;
uart iris_serial = xbee;

void IRIS_string(const char * s){ if(serial_send_node_suru)iris_serial.string(s); }

void serial_send_node(node* n){
    if(serial_send_node_suru){
        IRIS_string("\n\r--------------------\n\r");
        //IRIS_string("*[adress] : ");
        //iris_serial.putint((int)n);
        //IRIS_string("\n\r");
        IRIS_string("*[x,y,z] = ");
        iris_serial.putint((int)n->x);
        IRIS_string(",");
        iris_serial.putint((int)n->y);
        IRIS_string(",");
        iris_serial.putint((int)n->z);
        IRIS_string("\n\r");
        IRIS_string("*[type] ");
        iris_serial.putint((int)n->type);
        IRIS_string("\n\r");
        IRIS_string("*[flag] ");
        iris_serial.putint((int)n->flag);
        IRIS_string("\n\r");
        IRIS_string("*[color] ");
        iris_serial.putint((int)n->color);
        IRIS_string("\n\r--------------------\n\r");
    }
}

void make_nodes(){
    IRIS_string("\n\rmake_nodes\n\r");
    if(!ta.r_now()->ac){
        rep(i,4){
            if(check_ping(i)>1){
                if(ta.ac_next(i,1)==np){ 
                    ta.ap_node(ta.r_now(),i);
                    IRIS_string("\n\rmake!\n\r");
                    ta.hamilton=false;
                }else{ ta.cn_graph(ta.r_now(),ta.ac_next(i,1)); }

                if(check_ping(i)>2 && ping_flag){//2マス先も見る.
                    if(ta.ac_next(i,2)==np){ 
                        ta.ap_node(ta.ac_next(i,1),i);
                        IRIS_string("\n\rmake!\n\r");
                        ta.hamilton = false;
                    }else{ ta.cn_graph(ta.ac_next(i,1),ta.ac_next(i,2)); }
                }
            }
        }
        ta.r_now()->ac=true;
    }
}

void black_tile(){
    if(color_check()==1 || ta.r_now()->type==v::black){
        IRIS_string("\n\r \x1b[30m \n\r");
        IRIS_string("BLACK");
        IRIS_string("\x1b[37m \n\r");
        ta.r_now()->type=v::black;
        ta.r_now()->color=v::black;
        motor::move(motor::ONE_BACK);
        motor::fix_position(v::back);
        ta.turn_l();
        ta.turn_l();
        ta.go_st();
        ta.turn_l();
        ta.turn_l();
        ta.hamilton=false;
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
    IRIS_string("\n\r NAC[START] \n\r");
    ta.hamilton=false;
    if(ta.r_now()->type==v::slope){
        IRIS_string("NAC[__]\n\r");
        node* t = ta.r_now();/*空間計算量を抑える為に使いまわす*/
        rep(i,4){ if(t->next[i]->type==v::slope){t=t->next[i];break;} }/*node_aの探索*/
        rep(i,4){ if(t->next[i]->type!=v::slope){t=t->next[i];break;} }/*node_bの探索*/
        ta.w_now(t);
    }else{/*make_nodesよりも後だと、こちらが面倒*/
        IRIS_string("NAC[*_]\n\r");
        ta.r_now()->type=v::slope;
        node* t = ta.ac_next(v::back,1);/*t=nowの一つ前のnode*/
        uint8_t zz = ta.r_now()->z;
        if(flag == 2){zz++;}else{zz--;}
        rep(i,4){ 
            if(ta.r_now()->next[i]==np){
                ta.r_now()->next[i]=ta.mall.make(t->x,t->y,zz,(ta.r_flg()+1)%2); 
                t=ta.r_now()->next[i];/*t=node_a*/
                t->next[0]=ta.r_now();
                IRIS_string("NAC[**]\n\r");
                break;
            }
        }
        ta.w_now(t);t->type=v::slope;
        ta.go_st();/*node_b*/
        IRIS_string("\n\rNAC2[END]\n\r");
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
    switch(num){
        case 0:
            ta.turn_l();
            motor::move(motor::LEFT_TURN);
            motor::fix_position();
            ta.go_st();
            motor::move(motor::ONE_ADVANCE);
            break;
        case 1:
            ta.go_st();
            motor::move(motor::ONE_ADVANCE);
            break;
        case 2:
            ta.turn_r();
            motor::move(motor::RIGHT_TURN);
            motor::fix_position();
            ta.go_st();
            motor::move(motor::ONE_ADVANCE);
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
            break;
        case 3:
            ta.turn_l(); ta.turn_l();
            ta.go_st();
            ta.turn_l(); ta.turn_l();
            motor::move(motor::ONE_BACK);
            break;
        default:
            break;
    }
    IRIS_string("\n\r::<now>:: \n\r");
    serial_send_node(ta.r_now());
    if(ta.r_now()!=ta.r_start())ta.r_now()->color=color::black;
    black_tile();
    motor::fix_position();
    if(num==v::back){ nachylenie2(v::back); }else{ nachylenie2(v::front); }
    make_nodes();
    if(ta.r_now()->type==v::unknown){ta.r_now()->type = v::normal;}
    motor::wait();
}

void move_n(node* n){//move to neighborhood((node*)n)
    if(n!=np){
        IRIS_string("\n\rm_n[start]\n\r");
        rep(i,4)if(ta.ac_next(i,1)==n && ta.ck_conect(ta.r_now(),ta.ac_next(i,1)) && ta.ac_next(i,1)->type!=v::black){
            move(i);
            lcd_clear();
            lcd_putstr("m_n");
            IRIS_string("m_n[*]\n\r");
            break;
        }
        IRIS_string("m_n[end]\n\r");
    }
}

void move_toa(node* a){//move to (node*)a
    lcd_clear();
    ta.clear_dist();
    ta.bfs(a,ta.r_now());
    bl fg;
    IRIS_string("\n\r M_a[START]\n\r");
    //IRIS_string("= (node*)a = ");
    serial_send_node(ta.r_now());
    while(ta.r_now()!=a && a->type!=v::black && a->type!=v::slope){
        fg = false;
        rep(i,4){
            IRIS_string("M_a[*_] \n\r");
            iris_serial.putint((int)a);
            if(!fg && ta.ac_next(i,1)!=np && ta.ck_conect(ta.r_now(),ta.ac_next(i,1)) && ta.ac_next(i,1)->dist<ta.r_now()->dist && ta.ac_next(i,1)->type!=v::black){ move_n(ta.ac_next(i,1)); fg=true; }
            if(ta.find(a->x,a->y,a->z)->type==v::slope)fg=true;
        }
    }
    IRIS_string("M_a[**]\n\r");
    ta.clear_dist();
    lcd_clear();
    IRIS_string("M_a[END]\n\r");
    lcd_putstr("end");
}

void stack_dfs(){	
	IRIS_string("dfs [START]\n\r");
    serial_send_node(ta.r_now());
	ta.stk.push(ta.r_start());
	ta.r_start()->color=color::gray;
	make_nodes();
	bl fg;/*for ??*/ 
	while(!ta.stk.empty()){
        IRIS_string("dfs::[___]\n\r");
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
				IRIS_string("*dfs : stk_top ===");
				iris_serial.putint((int)ta.stk.top());
				IRIS_string("\n\r");
				IRIS_string("dfs::[*__]\n\r");
                move_toa(ta.stk.top());
				lcd_clear(); lcd_putstr("dfs");
                IRIS_string("\n\r");
                IRIS_string("dfs::[**_]\n\r");
				fg=true;
			}else{ ta.stk.pop(); }
		}//sub loop
        IRIS_string("dfs::[***]\n\r");
	}//main loop
	IRIS_string("dfs [END] \n\r");
	lcd_clear(); lcd_putstr("end_dfs");
}

void _stack_dfs(){	
	IRIS_string("dfs[START]\n\r");
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
            IRIS_string("dfs::[___]\n\r");
            ta.clear_dist(); ta.bfs(ta.r_now(),ta.r_start()); /*for t_top()*/
			if(ta.stk.t_top()->color!=color::black && !ta.stk.empty() && ta.stk.t_top()!=np){
				IRIS_string("*dfs : stk_top ===");
				iris_serial.putint((int)ta.stk.top());
				IRIS_string("\n\r");
				IRIS_string("dfs::[*__]\n\r");
				move_toa(ta.stk.t_top());
				lcd_clear(); lcd_putstr("dfs");
                IRIS_string("\n\r");
                IRIS_string("dfs::[**_]\n\r");
				fg=true;
			}else{ ta.stk.t_pop(); }
		}//sub loop
        IRIS_string("dfs::[***]\n\r");
	}//main loop
	IRIS_string("dfs[END]\n\r");
    IRIS_string("END ALL PROCEDURE.\n\r");
    IRIS_string("Thank you!! :) \n\r");
	lcd_clear(); lcd_putstr("end_dfs");
}

void h_stack_dfs(){
	IRIS_string("\n\rdfs[START]\n\r");
    serial_send_node(ta.r_now());
	ta.stk.push(ta.r_start());
	ta.r_start()->color=color::gray;
	make_nodes();
	stack sstk;
    ta.hamilton=false;
    bl fg;/*for ??*/ 
    bool hm_flag = true;//もし、直前にstackにnode突っ込んでたらhamiltonしない.
	while(!ta.stk.empty()){
        IRIS_string("dfs[______]\n\r");
		if(ta.r_now()!=ta.r_start())ta.r_now()->color=color::black;
		//node追加 normal ver
        hm_flag = true;
        for(int i=3;i>=0;i--){
			if(ta.ac_next(i,1)!=np && ta.ac_next(i,1)->color==color::white && ta.ck_conect(ta.r_now(),ta.ac_next(i,1))){
				ta.stk.push(ta.ac_next(i,1));
				ta.ac_next(i,1)->color=color::gray;
                hm_flag = false;
			}
		}//node追加部分ここまで
		IRIS_string("dfs[*_____]\n\r");
        lcd_clear(); lcd_putstr("dfs");
		if(ta.stk.empty()){ta.stk.push(ta.r_start());}
        while(!ta.stk.empty() && ta.stk.top()->type!=v::unknown && ta.stk.top()!=ta.r_start() && ta.r_now()!=ta.r_start()){
            ta.stk.pop();
            ta.hamilton = false;
        }
        IRIS_string("dfs[**____]\n\r");
		fg=false;
        if(!ta.hamilton && hm_flag){
        	ta.dp_calc();
        	IRIS_string("\x1b[35m \n\r");
        	IRIS_string("_h_try_");
        	IRIS_string("\x1b[37m \n\r");
        }
        if(ta.hamilton && hm_flag){ 
            sstk = ta.ans_v;
            IRIS_string("\x1b[36m \n\r");
            IRIS_string("_hami_");
            IRIS_string("\x1b[37m \n\r");
        }else{ sstk = ta.stk; }
        IRIS_string("dfs[***___]\n\r");
		while(!fg){
			if(ta.stk.empty()){//new...
			    move_toa(ta.r_start());
			    break;
			}
            if(sstk.top()==ta.r_now()&&ta.r_now()!=ta.r_start())sstk.pop();
			if(sstk.top()->color!=color::black && !sstk.empty() && sstk.top()!=np){
				IRIS_string("stk_top ");
				serial_send_node(sstk.top());
				IRIS_string("\n\r");
				IRIS_string("dfs[****__]\n\r");
                move_toa(sstk.top());
				lcd_clear(); lcd_putstr("dfs");
                IRIS_string("\n\r");
                IRIS_string("dfs[*****_]\n\r");
				fg=true;
			}else{ sstk.pop(); }
		}//sub loop
        IRIS_string("dfs[******]\n\r");
	}//main loop
	IRIS_string("dfs[END] \n\r");
    IRIS_string("Thank you! :) \n\r");
	lcd_clear(); lcd_putstr("end_dfs");
}
