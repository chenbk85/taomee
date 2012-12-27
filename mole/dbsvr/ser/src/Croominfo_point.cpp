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

extern "C" {
#include <math.h>
}
#include "Croominfo_point.h"


Croominfo_point :: Croominfo_point(mysql_interface *db)
	              : CtableRoute10x10(db, "ROOMINFO", "t_roominfo_point", "userid")
{

}


int Croominfo_point :: update(userid_t userid, uint32_t value, const char *field_str)
{
	sprintf(this->sqlstr, "update %s set %s = %u where userid = %u",
			this->get_table_name(userid),
			field_str,
			value,
			userid
			);
	STD_SET_RETURN_EX (this->sqlstr, DB_ERR);
}


int Croominfo_point :: insert(userid_t userid)
{
	sprintf(this->sqlstr, "insert into %s values(%u, 0, 0, 0, 0x0000)",
			this->get_table_name(userid),
			userid
		   );
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);
}


int Croominfo_point :: set_value(userid_t userid, int32_t value, const char *field_str)
{
	uint32_t data = 0;
	int ret = this->get_value_ex(userid, &data, field_str);
	if (ret != SUCC) {
		return ret;
	}
	switch (value) {
		case 1:
			data |= 0x01;
			break;
		case 2:
			data |= 0x02;
			break;
		case 3:
			data |= 0x04;
			break;
		case 4:
			data |= 0x08;
			break;
		case 5:
			data |= 0x10;
			break;
		default:
			return VALUE_OUT_OF_RANGE_ERR;
	}
	ret = this->update(userid, data, field_str);
	return ret;
}

int Croominfo_point :: get_value_ex(userid_t userid, uint32_t *p_value, const char *field_str) 
{
	*p_value = 0;
	int ret = this->get(userid, p_value, field_str);
	if (ret != SUCC) {
		ret = this->insert(userid);
		if (ret != SUCC) {
			return ret;
		}
	}
	return SUCC;
}


int Croominfo_point :: get_value(userid_t userid, uint8_t *p_value, const char *field_str) 
{
	uint32_t value = 0;
	int ret = this->get(userid, &value, field_str);
	if (ret != SUCC) {
		ret = this->insert(userid);
		if (ret != SUCC) {
			return ret;
		}
	}

	if ((value & 0x01) == 1) {

		p_value[0] = 1;
	}
	if ((value & 0x02) == 0x02) {
		p_value[1] = 1;
	}
	if ((value & 0x04) == 0x04) {
		p_value[2] = 1;
	}
	if ((value & 0x08) == 0x08) {
		p_value[3] = 1;
	}
	if ((value & 0x10) == 0x10) {
		p_value[4] = 1;
	}
	return SUCC;
}

int Croominfo_point :: get(userid_t userid, uint32_t *p_value, const char *field_str)
{
	int ret = this->get_task(userid, p_value, field_str);
	if (ret != SUCC) {
		ret = this->insert(userid);
		if (ret != SUCC) {
			return ret;
		}
		*p_value = 0;
	}
	return SUCC;
}


int Croominfo_point :: get_task(userid_t userid, uint32_t *p_value, const char *field_str)
{
	sprintf(this->sqlstr, "select %s from %s where userid = %u",
			field_str,
			this->get_table_name(userid),
			userid
			);
	 STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_value);
	 STD_QUERY_ONE_END();
}

int Croominfo_point :: set_task(userid_t userid, uint32_t task_id, uint32_t opt)
{
	uint32_t value = 0;
	int ret = this->get(userid, &value, "task");
	if (ret != SUCC) {
		ret = this->insert(userid);
		if (ret != SUCC) {
			return ret;
		}
	}
	uint32_t temp =value;
	task_id -= 1;
	if (opt == 1) {
		temp &= (3 << (2 * task_id));
		temp >>= (2 * task_id);
		if (temp == 1) {
			return YOU_HAVE_GOT_THIS_TASK_ERR;
		}
		if (temp ==2) {
			return YOU_HAVE_DONE_TASK_ERR;
		}
		value &= (~(1 << ((2 * task_id) + 1)));
		value |= (1 << ((2 * task_id)));
	}
	if (opt == 2) {
		temp &= (3 << (2 * task_id));
		temp >>= (2 * task_id);
		if (temp == 0) {
			return YOU_HAVE_NOT_GOT_TASK_ERR;
		}
		if (temp == 2) {
			return YOU_HAVE_DONE_TASK_ERR;
		}
		value |= (1 << (( 2 * task_id) + 1));
		value &= (~(1 <<(2 * task_id)));
	}
	ret = update(userid, value, "task");
	return ret;
}


int Croominfo_point :: get_water_list( userid_t userid, mms_water_list *p_out)
{
	sprintf( this->sqlstr, "select water_list from  %s where userid = %u ", 
			 this->get_table_name(userid),
			 userid
		   );
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(p_out, sizeof (*p_out));
	STD_QUERY_ONE_END();
}

int Croominfo_point :: update_water_list( userid_t userid, mms_water_list *p_list)
{
	char mysql_list[mysql_str_len(sizeof (*p_list))];
	set_mysql_string(mysql_list,(char*)p_list,
			            4 + sizeof(p_list->userid[0])* p_list->count);

	sprintf( this->sqlstr, "update %s set water_list = '%s' where userid=%d ", 
			 this->get_table_name(userid),
			 mysql_list,
			 userid
		   );
	 STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );
}


int Croominfo_point :: set_water_list(userid_t userid, userid_t other_id)
{
	mms_water_list temp = {	};
	int ret = this->get_water_list(userid, &temp);
	if (ret != SUCC) {
		ret = this->insert(userid);
		if (ret != SUCC) {
			return ret;
		}
	}
	if (temp.count >= 5) {
		return SUCC;
	}
	for (uint32_t i = 0; i < temp.count; i++) {
		if(temp.userid[i] == other_id) {
			return SUCC;
		}
	}
	temp.userid[temp.count] = other_id;
	temp.count++;
	ret = update_water_list(userid, &temp);
	return ret;
}
