/*
 * legacy.h
 *
 * Created: 2018/11/22 7:23:46
 *  Author: TOMOKI
 */ 


#ifndef LEGACY_H_
#define LEGACY_H_


namespace for_func{
	int check_neighbor(node* t,node* s){//t�̎���ɂ���s�ɂ��āA����dir���猩�Ăǂ��瑤�ɂ��邩��Ԃ��֐��B(node*,node*)->direction
	int x=s->x - t->x;
	int y=s->y - t->y;
	int z=s->z - t->z;
	int ans=-1;//-1==error
	if(x!=0 && z==0 && y==0){//�܂��͊�̕�������l��������dir��Ԃ��B���̌�ɍ���dir�Ɣ�r���đ��Ή��B
		if(x==1){
			ans = v::back;
		}else if(x==-1){
			ans =v::front;
		}else{/*error*/}
	}else if(x==0 && y!=0 && z==0){
		if(y==1){
			ans =v::right;
		}else if(y==-1){
			ans =v::left;
		}else{/*error*/}	
	}else{/*error*/}
	return ans;//��ΓI�ȕ�����Ԃ��B���ΓI�ȕ����ɒ����ĂˁB
}
				
int change_relatively(int now_dir,int di){//di�͐�ΓI�ȕ���,now_dir�͍��̕���
	int num = now_dir - di;
	if(num == 0){
		return v::front;
	}else if(num == 1||num ==-3){
		return v::left;
	}else if(num == 2||num==-2){
		return v::back;
	}else if(num ==3||num == -1){
		return v::right;
	}else{/*error*/}
}
					
int _change_relatively(int now_dir,int dir){/*now_dir::����dir,dir::direction from now_dir,ans::��ΓI��dir,,,,(now_dir,dir)->ans*/}
};//end of for_func

void float_killer(){
	node* a=ta.r_now();
	if(ta.count_walls(a)==3 || ta.count_walls(a)<=1){
		//�I������
	}else{
		if(ta.r_wall(a,v::left)==v::nowall){
			move(v::left);
			move(v::front);
		}else if(ta.r_wall(a,v::front)==v::nowall){
			move(v::front);
		}else if(ta.r_wall(a,v::right)==v::nowall){
			move(v::right);
			move(v::front);
		}else{ /*error*/ }
			float_killer();
		}
		move(for_func::change_relatively(ta.r_dir(),for_func::check_neighbor(a,a->back[0])));
		move(v::right);
	}

bool for_fk2(int n){if(ta.ac_next(v::right,n) ==np || ta.ac_next(v::right,n)->type==v::unknown){return true;}else{return false;}}

void float_killer2(){
	if(for_fk2(1)){
		if(for_fk2(2) && ta.ac_next(v::right,3)->type!=v::unknown){
			//�E2�܂������Ė߂�
			move(v::right);
			move(v::front);
			move(v::front);
			move(v::back);
			move(v::back);
			move(v::left);
		}else{
			if(ta.ac_next(v::right,2)->type!=v::unknown){
				//�E1�}�X�i��ŁA�߂�
				move(v::right);
				move(v::front);
				move(v::back);
				move(v::left);
			}else{
				//���̏�no action
				//�ꉞ�����̏����̏���for_fk2(3)�Ƃ��F�X�����Ă�����3�}�X�E�ɂƂ��ł����B
			}
		}
	}else{
		//no action
	}
}

void hidarite(){
						if(ta.r_wall(v::left)==v::wall || ta.ac_next(v::left,1)->type==v::black){
							if(ta.r_wall(v::front)==v::wall || ta.ac_next(v::left,1)->type==v::black){
								if(ta.r_wall(v::right)==v::wall || ta.ac_next(v::right,1)->type==v::black){
									move(0);
									move(0);
									move(1);
									}else{
									move(2);
									//move(0);
									move(1);
									//�E��
								}
								}else{
								move(1);
								//�O��
							}
							}else{
							move(0);
							move(1);
							//����
						}
						//�Ǐ������݁B
					}

					void go_home(){
						node* t = ta.r_now();
						if(t==ta.r_start()){
							
							}else{
							int num = ta.ac_next(v::left,1)->depth - t->depth;
							int direc = v::left;
							for(int i=1;i<4;i++){
								if( (ta.ac_next(i,1)->depth - t->depth) > num){ num = ta.ac_next(i,1)->depth - t->depth; direc = i; }
							}
							switch(direc){
								case 0:
								//go left block
								move(0);
								move(1);
								go_home();
								break;
								case 1:
								//go st
								move(1);
								go_home();
								break;
								case 2:
								//go right block
								move(2);
								move(0);
								go_home();
								break;
								case 3:
								move(3);
								go_home();
								break;
								default:
								
								break;
		}
	}
}


#endif /* LEGACY_H_ */