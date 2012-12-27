/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_sbh.h
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

#ifndef  CROOMINFO_SBH_INCL
#define  CROOMINFO_SBH_INCL

#include "CtableRoute10x10.h"
#include "proto.h"
#include "benchapi.h"

/**
 * @brief 对ROOMINFO数据库中的t_roominfo_sbh表进行操作。包括插入更新记录，
 * 插入记录，得到记录的信息
 */

class Croominfo_sbh:public CtableRoute10x10 {
	public:
		Croominfo_sbh(mysql_interface *db);
		int get_items_value(userid_t userid, uint32_t index1, uint32_t index2, uint32_t& value1, uint32_t& value2);
		int get_prove_all(userid_t userid, uint32_t&  prove_all, uint32_t& prove);
		int insert(userid_t userid);
		int update_item_value_inc(userid_t userid, uint32_t index, int value);
		int update_prove_inc(userid_t userid, uint32_t prove);
		int update_prove_dec(userid_t userid, uint32_t prove);
		int update_prove_all_inc(userid_t userid, uint32_t prove);
		int get_prove(userid_t userid, roominfo_user_get_prove_out* p_out );
		
};


#endif //CROOMINFO_SBH_INCL

