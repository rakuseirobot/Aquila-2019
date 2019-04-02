#ifndef DATA_STRUECTURE_HPP_
#define DATA_STRUECTURE_HPP_
/*
 *  core.hpp 
 *  this is functions for mapping,algorithms,and so on. It's core of this robot.
 *  Author: Emile
 */ 
 #include <stdint.h>
#define np nullptr
#define max_size 250
#define rep(i,N) for(int i=0;i<N;i++)
using ll = long long int;
using ci = const int;
using bl = bool;
namespace v{
    //for all ,such as wall,move,and so on...
    ci left =0;
    ci front=1;
    ci right=2;
    ci back=3;
    //direction
    ci unknown = 0;
    ci normal = 1;
    ci black=2;
    //ci r_kit=3;
    ci c_point=4;
    ci slope=5;
    //ci hisai=6;
    ci start = 7;
    ci kaidan = 8;
	ci H =20;
	ci S =21;
	ci U =22;
	ci sermo=23;
	//type
    ci vv[4][2]={{-1,0},{0,1},{1,0},{0,-1}};//vv[dir][0]:=x,[1]:=y
	
}

namespace color{
	ci white = 0;
	ci gray = 1;
	ci black = 2;
}

struct node{
    uint8_t x=100,y=100,z=10;//coordinate
	uint8_t flag,type;//for bfs,search
	uint8_t color;//for real bfs
    uint8_t depth=1000,dist=1000;//
	bl ac=false;//already checked?
    node* next[4]={np};
    node* back=np;
	node* child[2]={np,np};
	uint8_t height=0;
};

struct tuple_four{
	uint8_t x_min,x_max,y_min,y_max;
	//tuple_four(uint8_t x_m,uint8_t x_M,uint8_t y_m,uint8_t y_M);
	void update(uint8_t x,uint8_t y);
	void write(uint8_t x_m,uint8_t x_M,uint8_t y_m,uint8_t y_M);
};

struct range_set{
	tuple_four set[20];
	tuple_four* at(int n);
};

class AVLtree{
	node* root;
	public:
	AVLtree();
	node* find(int x,int y,int z);
	node* find(node* t,int x,int y,int z);
	#define ht(t) (t ? t->height : 0)
	node* rotate(node* t,int l,int r);
	node* barance(node* t);
	void insert(node* x);
	node* insert(node* t,node* x);
	void erase(int x,int y,int z);
	node *erase(node *t,int x,int y,int z);
	node *move_down(node *t, node *rhs);
};

class queue{
    int siz;
    node* box[max_size];
public:
    void init();
    int size();
    node* front();
    void pop();
    void push(node* x);
    bl empty();
};

class stack{
    int siz;
    node* box[max_size];
public:
    void init();
    int size();
    node* top();
    void pop();
    void push(node* x);
    bl empty();
};

class nodes{
    int size,now;
    node mal[max_size];
    bl box[max_size]={0};
public:
    nodes();
    node* make(int x,int y,int z,int flag);
    bl full();
};

int max(int x,int y);
int min(int x,int y);

#endif
