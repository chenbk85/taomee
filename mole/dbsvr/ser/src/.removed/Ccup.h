/*
 * =====================================================================================
 * 
 *       Filename:  Ccup.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CCUP_INCL
#define  CCUP_INCL
#include "Ctable.h"
#include "proto.h"

typedef struct  group_cup_list{
    uint32_t date;  
    uint32_t groupid;  
    uint32_t cup1;  
}__attribute__((packed)) GROUP_CUP_LIST;





class Ccup : public Ctable{
	protected:
//		GROUP_CUP_HISTORY group_history;							 
		int insert( stru_group_cup *p_item );
	public:
		int add_cup_db(int groupid, int cup_type,int value );
		Ccup(mysql_interface * db );
//		int get_history_list_db( uint32_t cache_date ,GROUP_CUP_LIST**pp_list ,uint32_t *p_count );
//		int cache_history_list(uint32_t cache_date );
//		int get_history_list(GROUP_CUP_HISTORY * p_history);
		int get_group_list( uint32_t date ,uint32_t *p_count ,stru_group_cup **pp_list);
};

#endif   /* ----- #ifndef CCUP_INCL  ----- */

