#ifndef CORE_HPP_
#define CORE_HPP_
/*
 * core.hpp
 * Created: 2018/12/20 12:00:00
 *  Author: Emile
 */ 
#include "data_structure.hpp"

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
	range_set set;
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
	int range_size_helper(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t z);//helper for range_size
	float range_size(node* u,int dir);//return num of vertex(unknown)
};
extern core ta;
#endif