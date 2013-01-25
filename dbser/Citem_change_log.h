/*
 * =========================================================================
 *
 *        Filename: Citem_change_log.h
 *
 *        Version:  1.0
 *        Created:  2011-06-21 13:27:16
 *        Description:  
 *
 *        Author:  jim (jim@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */



#ifndef  CITEM_CHANGE_LOG_INC
#define  CITEM_CHANGE_LOG_INC

#include "CtableRoute.h"


enum item_change_log_opt_type {

	//消耗物品
	item_change_log_opt_type_del = 0 ,
	//得到物品
	item_change_log_opt_type_add = 1,
	//通过米币购买得到的
	item_change_log_opt_type_vip_add =  2,
};

class Citem_change_log : public CtableRoute
{

protected:
	int insert(uint32_t logtime, uint32_t opt_type, uint32_t userid,
			uint32_t  itemid, uint32_t count ) ;
	int create_table( uint32_t userid,uint32_t logtime);
	
public:
	Citem_change_log(mysql_interface * db,const char * db_name ,
			const char * table_name="t_item_change_log" );

	// is_vip_opt:是不是vip 渠道
	// count :可以为+/- 
	int add(uint32_t logtime, bool is_vip_opt, uint32_t userid,
			uint32_t  itemid, int count ) ;

	int add(uint32_t logtime, item_change_log_opt_type opt_type, uint32_t userid,
			uint32_t  itemid, uint32_t count ) ;


	char *  get_table_name(uint32_t userid ,uint32_t logtime);
};

#endif /* ----- #ifndef ----- */

