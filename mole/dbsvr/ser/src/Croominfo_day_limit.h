/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_day_limit.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/01/2010 04:40:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  tommychen, tommychen@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef  CROOMINFO_DAY_LIMIT_INCL
#define  CROOMINFO_DAY_LIMIT_INCL

#include "CtableRoute10x10.h"
#include "proto.h"
#include "benchapi.h"

/**
 * @brief 对ROOMINFO数据库中的t_roominfo_day_limit表进行操作。包括插入更新记录，
 * 插入记录，得到记录的信息
 */

class Croominfo_day_limit : public CtableRoute10x10 {
	public:
		Croominfo_day_limit(mysql_interface *db);

		int set(const userid_t userid, const uint32_t petid, const int change_value);
		
		int update_num_inc(const userid_t userid, const uint32_t petid, const int num);

		int get(const userid_t userid, const uint32_t petid, uint32_t &change_value);
		
		int get_num(const userid_t userid, const uint32_t petid, uint32_t &count);
		
		int get_user_change_value_list(const userid_t userid, uint32_t *p_count, user_su_get_change_value_out_item **pp_item);

		int insert(const userid_t userid, const uint32_t petid);
		
};


#endif //CROOMINFO_DAY_LIMIT_INCL


