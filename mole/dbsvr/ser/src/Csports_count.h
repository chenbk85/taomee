/*
 * =====================================================================================
 * 
 *       Filename:  Csports_count.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2009年02月16日 10时10分20秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), jim@taomee.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CSPORTS_COUNT_INC
#define  CSPORTS_COUNT_INC
#include "Ctable.h"
#include "proto.h"


typedef struct  pet_group_count_list{
    uint32_t date;  
    uint32_t group_allcount[4];  
}__attribute__((packed))   PET_GROUP_COUNT_LIST ;




class Csports_count : public Ctable{
	protected:
		inline int cache_allcount( uint32_t date );
	public:
		Csports_count(mysql_interface * db );
		pet_group_count_list cache_list;
		int add_count_db( uint32_t groupid);
		int add_count( uint32_t groupid);
		int insert(uint32_t logdate );
		int init();
		int get_count_list( uint32_t * p_count, pet_group_item **pp_item  );
		int get_all_count( uint32_t groupid, uint32_t * p_count );
		int get_count( uint32_t groupid,uint32_t  date, uint32_t * p_count );

};

#endif   /* ----- #ifndef CSPORTS_COUNT_INC  ----- */

