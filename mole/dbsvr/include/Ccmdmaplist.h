/*
 * =====================================================================================
 * 
 *       Filename:  Ccmdmap.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月29日 19时15分29秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CCMDMAPLIST_INC
#define  CCMDMAPLIST_INC
#include <vector>
#include<iostream>
#include <algorithm>
#include <string.h>
#include "benchapi.h" 

using namespace std;

/**
 *Ccmdmap:
 * 命令--私有结构
 */
template <class _Tp > 
class Ccmdmap {
public: 
	typedef _Tp  T_PRI_STRU ;
	//命令编号
	uint16_t cmd_id;	
	//命令所对应私有结构
	_Tp pri_stru;
	inline void set(Ccmdmap & amap ) {
		memcpy( this, &amap,sizeof( Ccmdmap )  );
	}
	inline void set_cmdid(uint16_t cmdid ) {
		this->cmd_id=cmdid;
	}

	inline 	uint16_t get_cmd_id(){
		return this->cmd_id;
	}
	inline 	_Tp* get_pri_stru(){
		return  &(this->pri_stru );
	}	
	inline bool operator ==(const Ccmdmap & other  ) {
		if (this->cmd_id==other.cmd_id )	
			return true;
		else  return false;
	}
	inline 	bool operator <(const Ccmdmap & other  ) const {
		if (cmd_id < other.cmd_id )	
			return true;
		else  return false;
	}
	inline 	bool operator >(const Ccmdmap & other  ) const {
		if (this->cmd_id > other.cmd_id )	
			return true;
		else  return false;
	}
};


/**
 *Ccmdmap:
 * 命令--私有结构
 * 
 */

template <class _Tp > 
class Ccmdmaplist {
	public:
		typedef _Tp  T_ITEM  ;
		vector< T_ITEM > v_cmdmap ;
	private:	
	public:
		typedef typename vector<T_ITEM >::iterator  iterator;
		Ccmdmaplist()
		{
			this->v_cmdmap.reserve(65536);
		}
		inline typename T_ITEM::T_PRI_STRU * getitem(uint16_t cmdid){
			if (this->v_cmdmap[cmdid].cmd_id!=0  ){
				return  this->v_cmdmap[cmdid].get_pri_stru();
			}else{
				return  NULL;
			}
		}

		inline void show_item(){
			typename  Ccmdmaplist::iterator  it;
			for (int i=0;i<65536;i++){
				DEBUG_LOG("[%d][%04X][%s]",i,this->v_cmdmap[i].cmd_id, 
					   	this->v_cmdmap[i].get_pri_stru()->desc_msg );
			}
		}



		/*
		//加入
		inline void additem(T_ITEM  aitem ){
			this->v_cmdmap.push_back( aitem );
		}
		//sort for  equal_range search 
		inline void sort(){
			std::sort(this->v_cmdmap.begin(),this->v_cmdmap.end());
		}
				
		//得到要所指向的ID元素
		// 查找到的iterator 通过 it,返回 
		// 函数返回:是否查找到
		inline typename T_ITEM::T_PRI_STRU * getitem(uint16_t cmdid){
			T_ITEM cp;			
			typename  Ccmdmaplist::iterator  it;
			pair< typename  Ccmdmaplist::iterator,
				typename Ccmdmaplist::iterator > range;
			cp.set_cmdid(cmdid );
			//二分查找
			range=equal_range(this->v_cmdmap.begin(),this->v_cmdmap.end(), cp  );
	
			it=range.first;		
			if (it!=this->v_cmdmap.end()&&it->get_cmd_id()==cp.get_cmd_id()){
				return it->get_pri_stru();
			}else{
				return  NULL;
			}

		}
		inline void show_item(){
			typename  Ccmdmaplist::iterator  it;
			 for (it=v_cmdmap.begin(); it!=v_cmdmap.end(); it++){
				DEBUG_LOG("[%04X][%s]", it->get_cmd_id(),it->get_pri_stru()->desc_msg);
			}
		}
		*/
};
#endif   /* ----- #ifndef CCMDMAPLIST_INC  ----- */

