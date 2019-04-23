#ifndef CORE_HPP_
#define CORE_HPP_
/*
 * core.hpp
 * Created: 2018/12/20 12:00:00
 *  Author: Emile
 */ 
#include "data_structure.hpp"
#define M_V_SIZE 9 //max vertex size

class core{
    node* now;
    node* start;//it may be unnecessary.But now, it is used in dfs.
    int dir;//0::<,1::^,2::>,3::v
    //for status
    node* ans;//dfs's answer.
    int flg;//0 or 1
    //for dfs
    queue q;//for bfs
public:
	AVLtree at;
	nodes mall;
	stack stk;
	//range_set set;
    core();
    void turn_r();
    void turn_l();
	void w_now(node* u);
	int r_flg();
    node* r_now();
	node* r_start();
    void dfs(node* t,int x,int y,int z,int depth);
    node* find(int x,int y,int z);
	void ins_node(node* x);
    void cn_graph(node* v, node* u);
    void cn_tree(node* par,node* v);
    void ap_node(node* t,int dire);
    node* ac_next(node* t,int now_dir,int dire,int dist);
    node* ac_next(int dire,int dist);
	bl ck_conect(node* s,node* t);
    void go_st();
    void cl_dist(node* t,int d);
    void clear_dist();
    void bfs(node* s,node* t);
    /*test ver*/
    node* ans_v[M_V_SIZE+2];
    node* vertex[M_V_SIZE+2];//vertexs
    int8_t vertex_size;
    //v[i]->v[i-1]->...(i=|v|,|v|-1,,1,0)
    uint8_t dist[M_V_SIZE+2][M_V_SIZE+2];
    //dist[s][t]:=distance(s->t)
    uint16_t dp[1<<(M_V_SIZE+2)][M_V_SIZE+2];
    //集合(set)のiで終わる最短ハミルトン路の長さを算出
    //ただし、始点v[0]と終点v[vertex_size]は固定
    //dp[(set)][i] :=ans(dp_calc)
    
    int8_t prev[1<<(M_V_SIZE+2)][M_V_SIZE+2];
    //prev[(set)][i]:~dp[(Set)][i]の解をとる時の,前の頂点
    node* vertex_calc(int set,int i,int v_size);
    bool dp_init();
    bool dp_calc();
};

extern core ta;

#endif
