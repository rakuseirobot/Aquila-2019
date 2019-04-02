
/*
 * data_structure.hpp
 *
 * Created: 2018/11/27 23:03:16
 *  Author: TOMOKI NAKAO
 */ 
 #ifndef DATA_STRUCTURE_HPP_
 #define DATA_STRUCTURE_HPP_

 #define np nullptr
 using namespace std;
 using ll = long long int;
 using ci = const int;
 using bl = bool;

 namespace v{
	 //for all ,such as wall,move,and so on...
	 ci left =0;
	 ci front=1;
	 ci back=3;
	 ci right=2;
	 //direction
	 ci start = -1;
	 ci unknown = 0;
	 ci normal = 1;
	 ci black=2;
	 ci r_kit=3;
	 ci c_point=4;
	 ci slope=5;
	 ci hisai=6;
	 //type
	 //ci unknown=0;//type?øΩ?øΩunknown?øΩ∆àÍèè?øΩ?øΩ?øΩ?øΩ?øΩ?øΩA?øΩg?øΩ?øΩ?øΩ‹ÇÌÇ∑?øΩB
	 ci wall=1;
	 ci nowall=-1;
	 ci damy=23;//for bfs_type , damy
	 //wall
 }

 struct node{
	 int x=0,y=0,z=0,depth=1000,type=0;
	 node* next[4] ={nullptr,nullptr,nullptr,nullptr};
	 node* back[1] ={nullptr};
	 uint8_t wall[4]={0};// 0|<=|2 wall[0].,if dir=1 then wall[v::left]=left.??????????????
	 uint8_t flag=0;
	 uint8_t color=0;//0:unknown,1:in stack,2:checked
	 int hosu=1000;
 };

 class nodes{
	 ci max=200;
	 int size,now;
	 node mal[200];
	 bl box[200]={0};
  public:
	 nodes(){size=0;now=0;};
	 node* make(){
		if(now>=max-1){ return np; }else{
		   now++;
		   mal[now].back[0]=np;
		   mal[now].color=0;
		   return &mal[now];
		}
     }
	bl full(){ if(now>=max-1){ return true; }else{ return false; } }
 };

 struct stack{
	 node* box[300]={np};
	 int siz;
	 stack(){siz=0;};
	 node* top(){return box[siz-1];}
	 void init(){siz=0;for(int i=0;i<300;i++)box[i]=np;};
	 void push(node* n){box[siz]=n;siz++;};
	 void pop(){box[siz-1]=np;siz--;};
	 int size(){return siz;};
	 bool empty(){ if(siz==0){ return true; }else{ return false; }; }//if empty return true;
 };

 struct queue{////test
	 node* box[300]={np};
	 int siz;
	 queue(){siz = 0;}
	 void push(node* x){ box[siz]=x; siz++; }
	 node* front(){return box[0];}
	 int size(){return siz;}
	 void pop(){
		 for(int i=0;i<siz;i++){
			 box[i]=box[i+1];
		 }
		 siz--;
	 }
	 void clear(){for(int i=0;i<siz;i++)box[i]=np; siz=0;}
	 bool search(node* x){
		 bool tt = false;
		 for(int i=0;i<siz;i++){if(box[i]==x)tt=true;}
		 return tt;
	 }
 };

 stack st;
 nodes mall;
 
 #endif