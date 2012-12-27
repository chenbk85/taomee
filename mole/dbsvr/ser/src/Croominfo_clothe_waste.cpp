/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_gen_vip.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/08/2009 09:21:10 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Croominfo_clothe_waste.h"

int32_t clothe_init[] = {30, 40, 50, 50, 20};

#define ARRAY_NUM sizeof(clothe_init) / sizeof(int32_t)

Croominfo_clothe_waste :: Croominfo_clothe_waste(mysql_interface *db)
	              : CtableRoute10x10(db, "ROOMINFO", "t_roominfo_clothe_waste", "userid")
{

}


int Croominfo_clothe_waste :: update(userid_t userid, uint32_t clothe_type, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set clothe_waste = %u where userid = %u and clothe_type = %u",
			this->get_table_name(userid),
			value,
			userid,
			clothe_type
			);
	STD_SET_RETURN_EX (this->sqlstr, DB_ERR);
}


int Croominfo_clothe_waste :: insert(userid_t userid, uint32_t type, int32_t init_value)
{
	sprintf(this->sqlstr, "insert into %s values(%u, %u, %u)",
			this->get_table_name(userid),
			userid,
			type,
			init_value
			);
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);
}

int Croominfo_clothe_waste :: set_value_ex(userid_t userid, roominfo_set_clothe_waste_in *p_in,
	roominfo_set_clothe_waste_out *p_out)
{
	int ret = 0;
	uint32_t temp = 0;
	for (uint32_t i = 1; i <= p_in->clothe_count; i++) {
		ret = this->set_value(userid, (p_in->clothe_waste[i -1]).clothe_type,
				(p_in->clothe_waste[i -1]).clothe_value, &temp);
		if (ret != SUCC) {
			return ret;
		}
	}
	ret = this->get_value_all(userid, p_out->clothe_waste, &p_out->clothe_count);
	return ret;
}

int Croominfo_clothe_waste :: set_value(userid_t userid, uint32_t clothe_type, int32_t value, uint32_t *p_back)
{
	*p_back = 0;
	int32_t db_value = 0;
	if (clothe_type > ARRAY_NUM) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	int ret = this->get_value(userid, clothe_type, (uint32_t *)&db_value);
	if (ret != SUCC) {
		return ret;
	}
	db_value = value;
	if (db_value < 0) {
		db_value = 0;
	}
	if (db_value > clothe_init[clothe_type - 1]) {
		db_value = clothe_init[clothe_type -1];
	}
	*p_back = db_value;
	ret = this->update(userid, clothe_type, *p_back);
	return ret;
}

int Croominfo_clothe_waste :: get_value_all(userid_t userid, pair_clothe *p_array, uint32_t *p_count)
{
	*p_count = 5;
	int ret = 0;
	for (uint32_t i = 1; i <= 5; i++) {
		ret = get_value(userid, i, &((p_array + i -1)->clothe_value));
		if (ret != SUCC) {
			return ret;
		}
		(p_array + i -1)->clothe_type = i;
	}
	return ret;
}

int Croominfo_clothe_waste :: get_value(userid_t userid, uint32_t clothe_type, uint32_t *p_value) 
{
	if (clothe_type > ARRAY_NUM) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	*p_value = 0;
	int ret = this->get(userid, clothe_type, p_value);
	if (ret != SUCC) {
		ret = this->insert(userid, clothe_type, clothe_init[clothe_type - 1]);
		if (ret != SUCC) {
			return ret;
		}
		*p_value = clothe_init[clothe_type - 1];
	}
	return ret;
}

int Croominfo_clothe_waste :: get(userid_t userid, uint32_t clothe_type, uint32_t *p_value)
{
	sprintf(this->sqlstr, "select clothe_waste from %s where userid = %u and clothe_type = %u",
			this->get_table_name(userid),
			userid,
			clothe_type
			);
	 STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_value);
	 STD_QUERY_ONE_END();
}

