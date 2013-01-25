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
extern "C" {
#include <libtaomee/log.h>
}

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
			memset(&(this->v_cmdmap[0]),0,sizeof (this->v_cmdmap[0])*65536 );
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
				DEBUG_LOG("[%d][%04X]",i,this->v_cmdmap[i].cmd_id );
			}
		}
};
#endif   /* ----- #ifndef CCMDMAPLIST_INC  ----- */

