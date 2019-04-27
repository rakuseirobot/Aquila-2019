/*
 * data_structure.cpp
 * Created: 2018/12/31 12:34:55
 *  Author: emile
 */
#include "data_structure.hpp"
#include <stdint.h>
extern const int max_size;

#define ht(t) (t ? t->height : 0)
uint8_t lorr(node* t,int x,int y,int z){
	int d[3];
	d[0] = t->x-x;
	d[1] = t->y-y;
	d[2] = t->z-z;
	rep(i,3){
		if(d[i]<0)return 1;
		if(d[i]>0)return 0;
	}
	return 0;
}

AVLtree::AVLtree(){ root = np; }
node* AVLtree::find(int x,int y,int z){return find(root,x,y,z);}
node* AVLtree::find(node* t,int x,int y,int z){
	if(t==np)return np;
	if(t->x==x&&t->y==y&&t->z==z)return t;
	return find(t->child[lorr(t,x,y,z)],x,y,z);
}

node* AVLtree::rotate(node* t,int l,int r){
	node* s = t->child[l];
	t->child[l]=s->child[r];
	s->child[r]=barance(t);
	return barance(s);
}

node* AVLtree::barance(node* t){
	rep(i,2){
		if(ht(t->child[i])-ht(t->child[!i])>1){
			if(ht(t->child[i]->child[!i])-ht(t->child[i]->child[i])>0)t->child[i] = rotate(t->child[i],!i,i);
			return rotate(t,i,!i);
		}
	}
	if(t!=np)t->height=max(ht(t->child[0]),ht(t->child[1]))+1;
	return t;
}

void AVLtree::insert(node* x){ x->height=1;root = insert(root,x); }
node* AVLtree::insert(node* t,node* x){
	if(t==np)return x;
	if(t->x==x->x&&t->y==x->y&&t->z==x->z)return t;
	t->child[lorr(t,x->x,x->y,x->z)] = insert(t->child[lorr(t,x->x,x->y,x->z)],x);
	return barance(t);
}

void AVLtree::erase(int x,int y,int z){ root = erase(root,x,y,z); }
node* AVLtree::erase(node *t,int x,int y,int z){
	if(t==np)return np;
	if(t->x==x&&t->y==y&&t->z==z){
		return move_down(t->child[0], t->child[1]);
		}else{
		t->child[lorr(t,x,y,z)]=erase(t->child[lorr(t,x,y,z)],x,y,z);
		return barance(t);
	}
}

node* AVLtree::move_down(node *t, node *rhs) {
	if(t==np)return rhs;
	t->child[1] = move_down(t->child[1], rhs);
	return barance(t);
}


/////////////////////////////////////////////////////////////////AVLtree///////////////////////

void queue::init(){siz=0;rep(i,max_size)box[i]=np;}
int queue::size(){return siz;}
node* queue::front(){ return box[0]; }
void queue::pop(){
    rep(i,siz){box[i]=box[i+1];}
    siz--;
}
void queue::push(node* x){ if(x!=np){ box[siz]=x; siz++; } }
bl queue::empty(){if(siz==0){return true;}else{return false;}}

////////////////////////////////////////////////////////////////queue/////////////////////

void stack::init(){t_ans = -1;siz=0;rep(i,max_size_stack)box[i]=np;}
int stack::size(){return siz;}
node* stack::top(){
	if(box[siz-1]!=np && box[siz-1]->type==v::black){
		box[siz-1] = np;
		siz--;
	}
	if(siz>0){
		return box[siz-1];
	}else{
		return np;
	}
}
node* stack::t_top(){/*return t { t âˆ? box | min(t->dist) } ,and use ta.clear_bfs(); ta.bfs(ta.r_now(),ta.r_start); */
	//if(t_ans==-1)return np;
	if(siz==1){ t_ans = siz-1; return box[siz-1];}
	node* ans = np;
	uint8_t tmp = 255;
	for(int i=1;i<siz;i++){
		if(tmp>=box[i]->dist){
			ans = box[i]; t_ans = i;
			tmp = ans->dist;
		}
	}
	return ans;
}
void stack::pop(){if(siz>0){box[siz-1]=np;siz--;}}
void stack::pop(int x){
	if(x>=siz)return ;
	for(int i=x;i<siz;i++){ box[i]=box[i+1]; }
	siz--;
}
void stack::t_pop(){
	if(t_ans == -1){
		//no action
	}else{
		for(int i = t_ans;i<siz;i++){
			box[i] = box[i+1];
		}
		siz--;
		t_ans=-1;
	}
}
//void stack::push(node* x){box[siz]=x;siz++;}
void stack::push(node* x){
	int sizz=siz;
	rep(i,sizz){
		if(stack::box[i]!=np && stack::box[i]->type==v::black){
			stack::pop(i);
		}
	}
	while(stack::top()!=np && stack::top()->type!=v::unknown && stack::top()->type!=v::start ){
		stack::pop();
	}
	if(x!=np){
		box[siz]=x;
		siz++;
	}
}
bl stack::empty(){if(siz==0){return true;}else{return false;}}
void stack::clear(){
	t_ans = -1;
	siz=0;
	rep(i,max_size_stack)box[i]=np;
}
/////////////////////////////////////////////////////////////stack//////////////////////////////////

nodes::nodes(){size=0;now=0;};
node* nodes::make(int x,int y,int z,int flag){
    if(now>=max_size-1){ return np; }else{
        now++;mal[now].dist=255;mal[now].depth=255;//init
        mal[now].x=x; mal[now].y=y; mal[now].z=z; mal[now].flag=flag;
        mal[now].color=color::white;
        mal[now].ac=false;
        mal[now].type=v::unknown;//add on kansai open
		return &mal[now];
    }
}
/*
 * (node*)nodes::make(int x,int y,int z,int flag) -> (node*) n
 * n :: (x,y,z) = (x,y,z) (#input)
 *      type = unknown
 *      color = white
 *		flag = flag (#input)
 *		dist,depth = 255
 *      ac = false
 */
bl nodes::full(){ if(now>=max_size-1){ return true; }else{ return false; } }

//////////////////////////////////////////////////////nodes//////////////////////////////////////////////////

int max(int x,int y){if(x>y){return x;}else{return y;}}
int min(int x,int y){if(x<y){return x;}else{return y;}}
