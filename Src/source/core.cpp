/*
 * core.cpp
 * Created: 2018/12/30 3:19:32
 *  Author: emile
 */ 
#include "core.hpp"

template<class T,class T2>bool chmax(T &former, const T2 &b) { if (former<=b) { former=b; return true; } return false; }
template<class T,class T2>bool chmin(T &former, const T2 &b) { if (b<former) { former=b; return true; } return false; }

core::core(){
    now=mall.make(100,100,10,1);
	at.insert(now);
    flg=0;
    ans = np;
    start=now;
    dir=0;
    vertex_size=0;
    hamilton = false;
    ans_v.init();
    stk.init();
};
void core::turn_r(){ dir=(dir+1)%4; };
void core::turn_l(){ dir=(dir+4-1)%4; };
void core::w_now(node* u){ now = u; }
int core::r_flg(){ return flg; }
node* core::r_now(){return now;}
node* core::r_start(){return start;}
void core::dfs(node* t,int x,int y,int z,int depth){
    if(t!=np){
        if(t->depth>depth+1)t->depth=depth+1;
        if((long long int)(t->flag)!=flg){
            if(t->x==x && t->y==y && t->z==z){ ans=t; }
            t->flag=flg;
            dfs(t->next[0],x,y,z,t->depth);
            dfs(t->next[1],x,y,z,t->depth);
            dfs(t->next[2],x,y,z,t->depth);
            dfs(t->next[3],x,y,z,t->depth);
        }
    }
};
/*
node* core::find(int x,int y,int z){
    ans=np;
    dfs(start,x,y,z,start->depth);
    flg=(flg+1)%2;
    return ans;
};*/
node* core::find(int x,int y,int z){
    return at.find(x,y,z);
}
void core::ins_node(node* x){at.insert(x);}
void core::cn_graph(node* v, node* u){//Connect Nodes on Graph ::vとuをgraph(next[])に関してつなげる
    at.insert(u);
    if(v!=np && u!=np){
        rep(i,4)if(v->next[i]==np){ v->next[i]=u; break; }else if(v->next[i]==u){break;}
        rep(i,4)if(u->next[i]==np){ u->next[i]=v; break; }else if(u->next[i]==v){break;}
    }
}
void core::cn_tree(node* par,node* v){//connect nodes on Tree ::par(ent)とvをtree(back)に関してつなげる
    at.insert(v);
    if(v!=np && par!=np)if(v->back==np)v->back=par;
}
void core::ap_node(node* t,int dire){//append node (cn_graph)
    dire = (dire+dir+3)%4;
    node* u = find(t->x+v::vv[dire][0],t->y+v::vv[dire][1],t->z);
    if(u==np)u = mall.make(t->x+v::vv[dire][0],t->y+v::vv[dire][1],t->z,(flg+1)%2);
	//set.at(u->z)->update(u->x,t->y);//範囲算出用
    cn_graph(t,u);
}
node* core::ac_next(node* t,int now_dir,int dire,int dist){//(node*,int,int,int)->node* | nullptr(error) 
    int direction = (dire+now_dir+3)%4;
    if(t==np)return np;
    return find(t->x + v::vv[direction][0]*dist , t->y + v::vv[direction][1]*dist , t->z);
}
node* core::ac_next(int dire,int dist){ return ac_next(now,dir,dire,dist); }

bl core::ck_conect(node* s,node* t){
    bl ans = false;
    rep(i,4)if(s->next[i]==t){ans = true;break;}
    return ans;
}

void core::go_st(){
    node* next = ac_next(v::front,1);
    if(next==np)ap_node(now,v::front);
    next = ac_next(v::front,1);
    cn_graph(now,next);
    cn_tree(now,next);
    now = next;
}

void core::cl_dist(node* t,int d){//in clear_dist
    if(t!=np){
        t->dist=d;t->flag=flg;
        rep(i,4){
            if(t->next[i]!=np){
                if(t->next[i]->flag!=flg){cl_dist(t->next[i],d);}
            }else{ break; }
        }
    }
}

void core::clear_dist(){
    cl_dist(now,1000);
    flg=(flg+1)%2;
}


void core::bfs(node* s,node* t){//sを始点にしてtを検索する
    q.push(s);
    s->dist=0;
    while(!q.empty()){
        node* a = q.front();
        if(a==t){ans = t;}
        rep(i,4){
            node* aa=a->next[i];
            if(aa!=np){
                aa->dist=min(aa->dist+0,a->dist+1);
				if(aa->type==v::black)aa->dist=255;
                if(aa->flag!=flg){ q.push(aa); aa->flag=flg; }
            }else{ break; }
        }
        q.pop();
    }
    ans = np;
    flg=(flg+1)%2;
}

/*test*/

bool core::dp_init(){
    if(stk.size()>M_V_SIZE+1)return false;
    if(stk.size()<2)return false;
    int jj;vertex_size=0;
    if(now->z!=start->z){//階がstack内で違う
        jj=0;
        for(int i=0;stk.box[i]!=np;i++){
            if(stk.box[i]->z==now->z){
                vertex[jj]=stk.box[i];jj++;
            }
        }
    }else{//階がstack内で同じ
        rep(i,M_V_SIZE)vertex[i]=np;
        vertex[0]=start;
        jj=1;//jj:=suffix
        for(int i=0;stk.box[i]!=np;i++){/*start と now 以外のstkの中身を書き込む*/
            if(stk.box[i]!=start && stk.box[i]!=now && stk.box[i]!=np){vertex[jj]=stk.box[i];jj++; }
        }
        vertex[jj]=now;jj++;
        /* vertex[0]=start , [1...k-1]=(node*) , [k]=now */
    }/* ここまでvertexのinit */
    //*jjは頂点数*
    vertex_size = jj;
    rep(i,jj){
        clear_dist();
        bfs(vertex[i],start);//startは何でもいいはず
        rep(j,jj){
            dist[i][j] = vertex[j]->dist; dist[j][i] = vertex[j]->dist;
        }
    }//init dist
    rep(i,1<<(M_V_SIZE+2))rep(j,M_V_SIZE+2){
        dp[i][j]=256;
        prev[i][j]=-1;
    }//init dp , init prev
    return true;
}

node* core::vertex_calc(int set,int i,int v_size){
    if(v_size==1){
        ans_v.push(vertex[v_size-1]);
        return vertex[v_size-1];
    }else{
        uint8_t pre_v=prev[set][i];
        vertex_calc(set&~(1<<i) , pre_v , v_size-1);
        ans_v.push(vertex[i]);
        //return ans_v[v_size-1]=vertex[i];
        return vertex[i];
    }
}

bool core::dp_calc(){
    stk.push(np);
    if(now==start)return hamilton = false;
    if(!dp_init())return hamilton = false;
    int mask=(1<<0);//bit set ,(now)mask = {0}
    dp[mask][0]=0;
    //vertex[0]=startとvertex[vertex_size]=nowの要素は固定
    for(mask=1;mask<(1<<(vertex_size-1));mask++)rep(i,vertex_size-1)if(mask&(1<<i))rep(j,vertex_size-1){
        if(!(mask&(1<<0)))continue;
        if(chmin(dp[mask|(1<<j)][j],dp[mask][i]+dist[i][j])){
            prev[mask|(1<<j)][j]=i;
        }
    }
    int anss=260;
    mask--;//need!
    for(int i=1;i<vertex_size-1;i++){
        if(chmin(dp[mask|(1<<(vertex_size-1))][vertex_size-1],dp[mask][i]+dist[i][vertex_size-1])){
            if(chmin(anss,dp[mask|(1<<(vertex_size-1))][vertex_size-1])){
                prev[mask|(1<<(vertex_size-1))][vertex_size-1]=i;
            }
        }
    }
    ans_v.clear();
    vertex_calc(mask|(1<<(vertex_size-1)),vertex_size-1,vertex_size);
    if(ans_v.size()!=vertex_size)return hamilton = false;
    else return hamilton = true;
}

core ta;
