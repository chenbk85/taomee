/*
 * =====================================================================================
 * 
 *       Filename:  func_route_base.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月02日 16时40分57秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CFUNC_ROUTE_BASE_INC
#define  CFUNC_ROUTE_BASE_INC
#include "Ccmdmaplist.h"
#include "benchapi.h"

/*
 * =====================================================================================
 *        Class:  Cfunc_route_base
 *  Description:  
 * =====================================================================================
 */

/***
 *  _Tp:listitem,  
 *  _Tp_c 
 */
template <class _Tp > 
class Cfunc_route_base
{
protected: 
	Ccmdmaplist<_Tp > cmdmaplist;		

	/**
	 * 功能:从deal_fun 统一初始化
	 * count:数组中元素的个数.
	 */
	inline void initlist(_Tp * deal_fun, int count ){
		for (int i=0;i<count;i++ ){
			this->cmdmaplist.v_cmdmap[ (*(deal_fun+i)).cmd_id]=*(deal_fun+i);
		}
		//this->cmdmaplist.sort();
	}
	
}; /* -----  end of class  Cfunc_route_base  ----- */

#endif   /* ----- #ifndef CFUNC_ROUTE_BASE_INC  ----- */

