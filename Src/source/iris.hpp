#ifndef IRIS_HPP_
#define IRIS_HPP_
/*
 * iris.hpp
 * Created: 2018/12/20 12:00:00
 *  Author: Emile
 */ 
#include "core.hpp"

void make_nodes();

void black_tile();

void nachylenie();
bool nachylenie2();

void move(int num);
void move_n(node* n);

void move_toa(node* a);
void stack_dfs();
#endif
