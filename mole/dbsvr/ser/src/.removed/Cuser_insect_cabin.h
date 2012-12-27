/*
 * =====================================================================================
 *
 *       Filename:  Cuer_insect_cabin.h
 *
 *    Description:  蝴蝶等昆虫的信息追踪，记录一个昆虫的信息，出生，状态，工作等等情况。
 *
 *        Version:  1.0
 *        Created:  2009年12月07日 10时21分31秒
 *       Revision:  none
 *       Compiler:  gcc -Wall -Wextra Cuer_butterfly.h
 *
 *         Author:  easyeagel (LiuGuangBao), easyeagel@gmx.com
 *        Company:  淘米网络-taomee.com
 *
 * =====================================================================================
 */

#ifndef CUSER_INSECT_CABIN_INL
#define CUSER_INSECT_CABIN_INL

#include<vector>

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"


class Cuser_insect_cabin: public CtableRoute100x10
{
public:
typedef user_insect_cabin_get_all_out_item InsectInfo;
struct InsectTotal

	Cuser_insect_cabin(mysql_interface* db)
		:CtableRoute100x10( db , "USER" , "t_user_insect_cabin" , "autoid") 
		{}

	int insect_get_all(const userid_t user_id,
			user_insect_cabin_get_all_out_header* header, user_insect_cabin_get_all_out_item** list);

	int insect_get(const userid_t user_id, const uint32_t inner_id)
	{
		return 0;
	}

private:
	int sql_insert(const userid_t user_id, const uint32_t insect_id, const uint8_t status);
	int sql_select_all(const userid_t user_id, uint32_t* pcount, InsectInfo** list);
	int sql_select(const userid_t user_id, const uint32_t inner_id, InsectInfo& info);
	int sql_update_state(const userid_t user_id, const std::vector<uint32_t>& id_list, const uint32_t state);
	int sql_update_state(const userid_t user_id, const uint32_t inner_id, const uint32_t state)
	{
		std::vector<uint32_t> v;
		v.push_back(inner_id);
		return sql_update_state(user_id, v, state);
	}
};


#endif //CUSER_INSECT_CABIN_INL
