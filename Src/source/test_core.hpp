
/*
 * test_core.hpp
 *
 * Created: 2018/11/30 16:07:46
 *  Author: TOMOKI
 */ 
  /**
 *  @file test_core.hpp 
 *  @auther Emile
 *  this is functions for mapping,algorithms,and so on. It's core of this robot.
 *  Author: TOMOKI
 */ 
#ifndef TEST_CORE_HPP_
#define TEST_CORE_HPP_

#include "data_structure.hpp"

/**in (class)core
*
* NEW     : (private)(int) tmp --for dfs
* CHANGE  : counter :: Don't use this now.
* REPLACE : countter -> tmp
* 
*/

class core{
    node* now;
    node* start;//it may be unnecessary.But now, it is used in dfs.
    int dir;//0::<,1::^,2::>,3::v //int :OK uint:NG<-overflow(dir can be -1.)
    //for status
    long long unsigned int counter;//dfs's counter.->(Don't use this now.) [TEST]
    uint8_t tmp; //(lluint)countter -> (int)tmp  [TEST]
    node* ans;//dfs's answer.
    node* begin;//•à”map‚Ìbegin
	int num_vertex;
    //for dfs
	queue q;
	//data structure
public:
    core(){
        now = mall.make();
        start = now;
        start->x=100;start->y=100;start->z=1;start->back[0]=np;start->type=v::start;start->depth=0;
        //dir = v::left;
        dir = 0;
        counter=0;
		num_vertex = 0;
        ans = np;
        begin = np;
        tmp=0;
		q.clear();
    };
    void turn_r(){
        dir++; 
		if(dir>3){ dir-=4; }
    };
    void turn_l(){
        dir--; 
		if(dir<0){ dir+=4; }
    };

    void dfs(node* t,int x,int y,int z,int depth){
        if(t!=np){
            if(t->depth>depth+1)t->depth=depth+1;
            if(t->flag!=tmp){ //<-[TEST]
				num_vertex++;
                if(t->x==x && t->y==y && t->z==z){ ans=t; }
                t->flag=tmp; //<-[TEST]
                dfs(t->next[0],x,y,z,t->depth);
                dfs(t->next[1],x,y,z,t->depth);
                dfs(t->next[2],x,y,z,t->depth);
                dfs(t->next[3],x,y,z,t->depth);
            }
        }
    };

    void cl_hs(node* t){
	    if(t!=np && (long long)(t->flag)!=tmp){//[TEST]
		    t->flag=tmp;//[TEST]
		    t->hosu=1000;
		    cl_hs(t->next[0]);
		    cl_hs(t->next[1]);
		    cl_hs(t->next[2]);
		    cl_hs(t->next[3]);
	    }
    }
    
    void clear_hosu(){
	    cl_hs(start);
	    tmp=(tmp+1)%2;//[TEST]
    }

	void into_queue(node* t){
		for(int i=0;i<4;i++){
			if(t->next[i]!=np && (int)t->next[i]->flag != tmp){//<-[TEST]
				q.push(t->next[i]);
                t->next[i]->flag = tmp; //<-[TEST]
                if(t->next[i]->hosu > t->hosu+1)t->next[i]->hosu=t->hosu+1;
				if(t->next[i]->type==v::black)t->next[i]->hosu = 300;
			}
		}
	}

	node* bfs_type(node* t,int typ){
		clear_hosu();
		t->flag = tmp;//<-[TEST]
		t->hosu=0;
		q.push(t);
		node* x=np;
		while(q.size()>0){
			x = q.front();
			q.pop();
			if(x->type==typ){break;}
			into_queue(x);
			//q.pop();
		}
		tmp=(tmp+1)%2;//[test]
		if(q.size()>0){
			q.clear();
            return x;
		}else{
            q.clear();
			return np;
		}
	}

    node* find(int x,int y,int z){
		num_vertex=0;
        ans=np;
        dfs(start,x,y,z,start->depth);
        tmp=(tmp+1)%2;//[TEST]
        return ans;
    };

    void move_to(node* t,int x,int y,int z){//connect (t,node(x,y,z)):(old)->(test)move to point(x,y,z)
        node* k = find(x,y,z);
            //cout<<"t,x,y,z,k :"<<t<<" , "<<x<<" , "<<y<<" , "<<z<<" , "<<k<<endl;
        if(k==np){
            node* n = mall.make();
            n->x=x;n->y=y;n->z=z;
            for(int i=0;i<4;i++){ if(t->next[i]==np){t->next[i]=n; break;}}
            n->depth=t->depth +1;
            n->back[0]=t;
            n->next[0]=t;
        }else{//(test) if (node)point(x,y,z) is not in DFS-Tree, add node to DFS-Tree.
            if(t->next[0]!=k && t->next[1]!=k && t->next[2]!= k && t->next[3]!= k){
                for(int i=0;i<4;i++){ 
                    if(t->next[i]==np){t->next[i]=k; break;}
                }//conection of nodes.
                if(k->next[0]!=t && k->next[1]!=t && k->next[2]!= t && k->next[3]!=t){ 
                    for(int i=0;i<4;i++){ 
                        if(k->next[i]==np){k->next[i]=t; break; } 
                    }//conection of nodes.
                }
				//num_vertex = 0;
                find(x,y,z);//depth?¿½ÌX?¿½V
            }
            if(k->back[0]==np)k->back[0]=t; //
        }
    };
    void move_to(int x,int y,int z){
        move_to(now,x,y,z);
        now = find(x,y,z);
    }

    node* r_start(){ return start; }
	int r_vnum(){ return num_vertex; }
    node* r_now(){ return now; }
    void w_now(int x,int y,int z){ now = find(x,y,z); }
    void w_now(node* ty){ now = ty; }

    int r_dir(){ return dir; }
    void w_wall(node* t,int direction,int num){ t->wall[(dir+direction+3)%4]=num; }
    void w_wall(int direction,int num){ now->wall[(dir+direction+3)%4]=num; }

    int r_wall(node* t,int direction){ return t->wall[(dir+direction+3)%4]; }
    int r_wall(int direction){ return now->wall[(dir+direction+3)%4]; }

    void go_st(){
        int x=now->x;
        int y=now->y;
        int z=now->z;
        switch(dir){
            case 0:
                x--;
                break;
            case 1:
                y++;
                break;
            case 2:
                x++;
                break;
            case 3:
                y--;
                break;
            default:
                break;
        }
        move_to(x,y,z);
    }

    node* ac_next(node* t,int dire,int now_dir,int dist){//(node*,int,int,int)->node* | nullptr(error)
        int direction = (dire+now_dir+3)%4;
        switch(direction){
            case v::left:
                return find(t->x - dist,t->y,t->z);
                break;
            case v::front:
                return find(t->x,t->y + dist,t->z);
                break;
            case v::right:
                return find(t->x + dist,t->y,t->z);
                break;
            case v::back:
                return find(t->x,t->y - dist,t->z);
                break;
            default:
                return np;
                break;
        }
    }
   
   /*
    * NEW:: (void) connect_node(node* t,int x,int y,int z) 
    * CHANGE:: (void) move_to --to add node(x,y,z) to DFS-Tree.(In order to make DFS-Tree.)
    * REPLACE:: (void) move_to (in append_node) -> (void) conect_node()
    *  ?(For making DFS-Tree)
    */
    void connect_node(node* t,int x,int y,int z){//(test) This function does not add (node)point(x,y,z) to DFS-Tree.
        node* k = find(x,y,z);
        if(k==np){
            node* n = mall.make();
            n->x=x;n->y=y;n->z=z;n->type==v::unknown;
            for(int i=0;i<4;i++)if(t->next[i]==np){t->next[i]=n;break;}
            n->next[0]=t;
        }else{
            for(int i=0;i<4;i++){
                if(t->next[i]==k)break;
                if(t->next[i]==np){ t->next[i]=k; break; }
            }
            for(int i=0;i<4;i++){
                if(k->next[i]==t)break;
                if(k->next[i]==np){ k->next[i]=k; break; }
            }
        }
    }//conect each other(only (node*)n->next[i])
   
    void append_node(node* t,int dire,int now_dir,int dist){//t->dir->now_dor->distance
        if(t==np){
            //no action
        }else{
            int direction = (dire+now_dir+3)%4;
            switch(direction){
                case v::left :
                    connect_node(t,t->x - dist,t->y,t->z);
                    break;
                case v::front :
                    connect_node(t,t->x,t->y + dist,t->z);
                    break;
                case v::right :
                    connect_node(t,t->x + dist,t->y,t->z);
                    break;
                case v::back :
                    connect_node(t,t->x,t->y - dist,t->z);
                    break;
                default:
                    //np;
                    break;
            }
        }
    }

    void append_node(int dire,int dis){ append_node(now,dire,dir,dis); }

    node* ac_next(int dire,int dist){ return ac_next(now,dire,dir,dist); }
   ///////
/////////end of hosu_map// := test -> Approval   ////////////////////////
};

#endif