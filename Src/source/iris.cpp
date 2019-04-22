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
#define ping_flag true //true�Ȃ�2�}�X�������

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
                if(check_ping(i)>2 && ping_flag){//2�}�X�������.
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
    }else if(motor::notify_long_acc(x)==2){//����
        node* x = ta.mall.make(100,100,ta.r_now()->z+1,(ta.r_flg()+1)%2);x->dist=255;ta.ins_node(x);
        ta.r_now()->type=v::slope;ta.r_now()->color=color::black;
        ta.cn_graph(ta.r_now(),x);
        x->type=v::slope;x->color=color::black;
        ta.w_now(x);
        ta.ap_node(ta.r_now(),v::front);
        ta.go_st();ta.r_now()->color=color::black;
        ta.stk.push(ta.r_now());
        make_nodes();
    }else if(motor::notify_long_acc(x)==1){//����
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

bool nachylenie2(uint8_t x){/*make_nodes�����O�Ɏg��*/
    if(x!=v::back){x=v::front;};
    uint8_t flag = motor::notify_long_ang(x);/*flag=1:����A=2:���A=0:����*/
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
        node* t = ta.r_now();/*��Ԍv�Z�ʂ�}����ׂɎg���܂킷*/
        rep(i,4){ if(t->next[i]->type==v::slope){t=t->next[i];break;} }/*node_a�̒T��*/
        rep(i,4){ if(t->next[i]->type!=v::slope){t=t->next[i];break;} }/*node_b�̒T��*/
        ta.w_now(t);
    }else{/*make_nodes�����ゾ�ƁA�����炪�ʓ|*/
        iris_serial.string("b");
        ta.r_now()->type=v::slope;
        node* t = ta.ac_next(v::back,1);/*t=now�̈�O��node*/
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
        //ta.stk.pop();//????its test.
    }
    return true;
}
/*
 * ������Ȃ�A
 * (�����ɒ��_�����݂��Ȃ���)
 * �E���
 * �E���݂̃m�[�h(now)��status����ɁA
 * �Enow(x,y,z)(�K�i��)->node_a(node_b����o�b�N����C���[�W�̏ꏊ,2�K)->node_b(x,y,z+1)�̂悤�ɐڑ��B
 * �E���݂̈ʒu(�������)��node_b�ɂ���B
 * (���݂��鎞)
 * �E���
 * �Enow(�K�i��)->node_a(2�K�̊K�i��)->node_b�̏��ɐi�ށB(�����ɒ萔���ԂȂ̂Ń_�u�����O�͕K�v�Ȃ�)
 * �E���ݒn��node_b�ɕύX
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
        iris_serial.string("m_n");
        rep(i,4)if(ta.ac_next(i,1)==n && ta.ck_conect(ta.r_now(),ta.ac_next(i,1)) && ta.ac_next(i,1)->type!=v::black){
            move(i);
            lcd_clear();
            lcd_putstr("m_n");
            iris_serial.string("-a");
            break;
        }
        iris_serial.string("mn-end\n\r");
    }
}

void move_toa(node* a){//move to (node*)a
    lcd_clear();
    ta.clear_dist();
    ta.bfs(a,ta.r_now());
    bl fg;
    iris_serial.string("m_a:a=");
    serial_send_node(ta.r_now());
    while(ta.r_now()!=a && a->type!=v::black && a->type!=v::slope){
        fg = false;
        rep(i,4){
            iris_serial.string("-aa");
            iris_serial.putint((int)a);
            if(!fg && ta.ac_next(i,1)!=np && ta.ck_conect(ta.r_now(),ta.ac_next(i,1)) && ta.ac_next(i,1)->dist<ta.r_now()->dist && ta.ac_next(i,1)->type!=v::black){ move_n(ta.ac_next(i,1)); fg=true; }
            if(ta.find(a->x,a->y,a->z)->type==v::slope)fg=true;
        }
    }
    iris_serial.string("-b");
    ta.clear_dist();
    lcd_clear();
    iris_serial.string("ma-end\n\r");
    lcd_putstr("end");
}

/*
void stack_dfs(){	
    iris_serial.string("start: ");
    serial_send_node(ta.r_now());
    iris_serial.string("-------------\n\r");
    ta.stk.push(ta.r_start());
    ta.r_start()->color=color::gray;
    make_nodes();
    bl fg;/*for ??*/ //float range_tmp[4]={0.0};//for node tuika
    /*
    uint8_t tmp2;uint8_t tmp[4]={0,1,2,3};//for node tuika
    while(!ta.stk.empty()){
        if(ta.r_now()!=ta.r_start())ta.r_now()->color=color::black;
        //node�ǉ�����
        for(int i=3;i>=0;i--){//node�ǉ� normal ver
            if(ta.ac_next(i,1)!=np && ta.ac_next(i,1)->color==color::white && ta.ck_conect(ta.r_now(),ta.ac_next(i,1))){
                ta.stk.push(ta.ac_next(i,1));
                ta.ac_next(i,1)->color=color::gray;
            }
        }
        //node�ǉ�������������
        /*for(int i=3;i>=0;i--){//node�ǉ� new ver
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
        //node�ǉ����������܂�
        *//*
        iris_serial.string("n");
        fg=false;
        while(!fg){
                ta.clear_dist();
                ta.bfs(ta.r_now(),ta.r_start());
            led_count_set((uint8_t)ta.stk.size());
            if(ta.stk.t_top()->color!=color::black && !ta.stk.empty() && ta.stk.t_top()!=np){
                iris_serial.string("dfs2 : ");
                iris_serial.putint((int)ta.stk.t_top());
                iris_serial.string("\n\r");
                //move_toa(ta.stk.top());
                ta.clear_dist();
                ta.bfs(ta.r_now(),ta.r_start());
                move_toa(ta.stk.t_top());
                lcd_clear();
                lcd_putstr("dfs2");
                iris_serial.string("s_dfs\n\r");
                fg=true;
                ta.stk.t_pop();//new
            }else{ ta.stk.t_pop(); }
        }
        iris_serial.string("end");
    }
}
*/

void stack_dfs(){	
	iris_serial.string("dfs_start: ");
    serial_send_node(ta.r_now());
    iris_serial.string("-------------\n\r");
	ta.stk.push(ta.r_start());
	ta.r_start()->color=color::gray;
	make_nodes();
	bl fg;/*for ??*/ //float range_tmp[4]={0.0};//for node tuika
	//uint8_t tmp2;//uint8_t tmp[4]={0,1,2,3};//for node tuika
	while(!ta.stk.empty()){
		if(ta.r_now()!=ta.r_start())ta.r_now()->color=color::black;
		//node�ǉ� normal ver
        for(int i=3;i>=0;i--){
			if(ta.ac_next(i,1)!=np && ta.ac_next(i,1)->color==color::white && ta.ck_conect(ta.r_now(),ta.ac_next(i,1))){
				ta.stk.push(ta.ac_next(i,1));
				ta.ac_next(i,1)->color=color::gray;
			}
		}//node�ǉ����������܂�
		lcd_clear(); lcd_putstr("dfs");
		fg=false;
		while(!fg){
			if(ta.stk.top()->color!=color::black && !ta.stk.empty() && ta.stk.top()!=np){
				iris_serial.string("*dfs : stk_top : ");
				iris_serial.putint((int)ta.stk.top());
				iris_serial.string("\n\r");
				move_toa(ta.stk.top());
				lcd_clear(); lcd_putstr("dfs");
                iris_serial.string("\n\r");
                iris_serial.string("__now_in_dfs_loop__\n\r");
				fg=true;
			}else{ ta.stk.pop(); }
		}//sub loop
	}//main loop
	iris_serial.string("dfs_end \n\r");
	lcd_clear(); lcd_putstr("end_dfs");
}
