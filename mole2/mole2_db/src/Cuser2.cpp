/*
 * =====================================================================================
 *
 *       Filename:  Cuser.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分56秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *  common.h
 * =====================================================================================
 */

#include "Cuser2.h"
#include <time.h>
#include "proto.h"
#include "benchapi.h"
#include "db_error.h"
#include <algorithm>
#include "Ccommon.h"

Cuser2::Cuser2(mysql_interface * db ) 
	:CtableRoute100x10(db, "MOLE2_USER" , "user2" , "userid")
{

}

int Cuser2::set_vip_level(userid_t userid, uint32_t vip_level)
{
	return this->set_int_value(userid, "vip_level", vip_level);
}

int Cuser2:: change_last_activity(userid_t userid, int32_t change_val, uint32_t *rt_val)
{
    return this->change_int_value(userid, "vip_activity_4", change_val, 0x7FFFFFFF, rt_val);
}

int Cuser2:: change_last_activity3(userid_t userid, int32_t change_val, uint32_t *rt_val)
{   
    return this->change_int_value(userid, "vip_activity_3", change_val, 0x7FFFFFFF, rt_val);
}

int Cuser2::add_vip_ex_val(userid_t userid, uint32_t vip_ex_val)
{
	uint32_t cur_val_ex = 0;
	return this->change_int_value(userid, "vip_ex_val", vip_ex_val, 0x7FFFFFFF, &cur_val_ex);
}

int Cuser2::set_vip_base_val(userid_t userid, uint32_t vip_base_val)
{
	GEN_SQLSTR(this->sqlstr, "insert into %s values(%u,0,0,0,%u,0,0,0,0,0,0) ON DUPLICATE KEY UPDATE vip_base_val=%u",
		this->get_table_name(userid),userid,vip_base_val,vip_base_val);
	return this->exec_insert_sql(this->sqlstr, SUCC);
}

int Cuser2::update(userid_t userid,mole2_user_vip_info *info)
{
	GEN_SQLSTR(this->sqlstr, "update %s set vip_auto=%u,vip_end_time=%u,vip_begin_time=%u,vip_activity=%u,"
		"vip_activity_2=%u, vip_activity_3=%u,vip_activity_4=%u  where userid=%u",
		this->get_table_name(userid),info->vip_auto,info->vip_end_time, info->vip_begin_time, info->vip_activity,
		info->vip_activity_2, info->vip_activity_3, info->vip_activity_4, userid);
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser2::get_vip_info(userid_t userid,mole2_user_vip_info *info)
{
	GEN_SQLSTR(this->sqlstr, "select vip_auto, vip_ex_val, vip_base_val, vip_end_time, vip_begin_time,vip_activity, vip_activity_2, vip_activity_3,  vip_activity_4  from %s where userid=%u", 
		this->get_table_name(userid), userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(info->vip_auto);
		INT_CPY_NEXT_FIELD(info->vip_ex_val);
		INT_CPY_NEXT_FIELD(info->vip_base_val);
		INT_CPY_NEXT_FIELD(info->vip_end_time);
		INT_CPY_NEXT_FIELD(info->vip_begin_time);
		INT_CPY_NEXT_FIELD(info->vip_activity);
		INT_CPY_NEXT_FIELD(info->vip_activity_2);
		INT_CPY_NEXT_FIELD(info->vip_activity_3);
		INT_CPY_NEXT_FIELD(info->vip_activity_4);
		if(info->vip_activity_3 > 100000000)
		{
			DEBUG_LOG("vip_activity_3 maybe wrong [id=%u vip_activity_3=%u]",userid,info->vip_activity_3);

		}
	STD_QUERY_ONE_END();
}

int Cuser2::set_vip_info(userid_t userid,mole2_user_set_vip_info_in *p_in, uint32_t * p_vip_activity )
{
	mole2_user_vip_info info={};
	uint32_t now = (uint32_t)time(NULL);

	int ret = this->get_vip_info(userid,&info);
	if(ret == SUCC) {
		DEBUG_LOG("SUCC");
		if(p_in->last_charge_chnl_id != 18 ) {
			if(info.vip_end_time < p_in->vip_begin_time)
				info.vip_end_time = p_in->vip_begin_time;
			if(p_in->vip_end_time > now && p_in->vip_end_time > info.vip_end_time) {
				DEBUG_LOG("SUCC11111");
				//info.vip_activity_4 += p_in->vip_end_time - info.vip_end_time;
				info.vip_activity_3 += p_in->vip_end_time - info.vip_end_time;
				if(p_vip_activity) *p_vip_activity = p_in->vip_end_time - info.vip_end_time;
			}
		}
		info.vip_auto = p_in->vip_auto;
		info.vip_end_time = p_in->vip_end_time;
		info.vip_begin_time = p_in->vip_begin_time;
		return this->update(userid,&info);
	} else {
		DEBUG_LOG("USER_ID_NOFIND_ERR");
		if(ret == USER_ID_NOFIND_ERR) {
			//uint32_t vip_activity_4 = 0;
			uint32_t vip_activity_3 =0; //关系到可翻牌的次数
			if(p_in->last_charge_chnl_id != 18 ) {
				DEBUG_LOG("USER_ID_NOFIND_ERR:111:%u,%u,%u ",p_in->vip_end_time,now,p_in->vip_begin_time );
				if(p_in->vip_end_time > now && p_in->vip_end_time > p_in->vip_begin_time) {
					DEBUG_LOG("USER_ID_NOFIND_ERR:222");
					//vip_activity_4 = p_in->vip_end_time - p_in->vip_begin_time;
					vip_activity_3 = p_in->vip_end_time - p_in->vip_begin_time;
					if(p_vip_activity) *p_vip_activity = p_in->vip_end_time - p_in->vip_begin_time;
				}
			}
			GEN_SQLSTR(this->sqlstr, "insert into %s values(%u,%u,0,0,0,%u,%u,0,0,%u,0) ON DUPLICATE KEY UPDATE vip_auto=%u,vip_end_time=%u,vip_begin_time=%u,vip_activity_3=%u ",
				this->get_table_name(userid), userid, p_in->vip_auto, p_in->vip_end_time, p_in->vip_begin_time,vip_activity_3 ,
				p_in->vip_auto, p_in->vip_end_time, p_in->vip_begin_time,vip_activity_3 );
			return this->exec_insert_sql(this->sqlstr, USER_ID_EXISTED_ERR);
		}
	}
	return ret;
}

int Cuser2::get_online_login(userid_t userid,stru_vip_args &p_out)
{
	GEN_SQLSTR(this->sqlstr, "select vip_auto, vip_ex_val, vip_base_val, vip_end_time, vip_begin_time from %s where userid=%u", 
			this->get_table_name(userid), userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out.type);
		INT_CPY_NEXT_FIELD(p_out.ex_val);
		INT_CPY_NEXT_FIELD(p_out.base_val);
		INT_CPY_NEXT_FIELD(p_out.end_time);
		INT_CPY_NEXT_FIELD(p_out.begin_time);
	STD_QUERY_ONE_END();
}


int Cuser2::set_field_value(userid_t userid, su_mole2_set_field_value_in *p_in)
{
	char mysql_value[mysql_str_len(sizeof(p_in->value))];
	set_mysql_string(mysql_value, p_in->value, sizeof(p_in->value));

	GEN_SQLSTR(this->sqlstr, "update %s set %s = '%s' where userid = %u", 
				this->get_table_name(userid), p_in->field,	mysql_value, 
				userid);
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}


int Cuser2::get_info(userid_t userid, user2_info_t*p_out)
{
	GEN_SQLSTR(this->sqlstr, "select vip_auto,vip_level,vip_ex_val,vip_base_val,vip_end_time,vip_begin_time,vip_activity,vip_activity_2,vip_activity_3,vip_activity_4 from %s where userid=%u", 
			this->get_table_name(userid), userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->vip_auto);
		INT_CPY_NEXT_FIELD(p_out->vip_level);
		INT_CPY_NEXT_FIELD(p_out->vip_ex_val);
		INT_CPY_NEXT_FIELD(p_out->vip_base_val);
		INT_CPY_NEXT_FIELD(p_out->vip_end_time);
		INT_CPY_NEXT_FIELD(p_out->vip_begin_time);
		INT_CPY_NEXT_FIELD(p_out->vip_activity);
		INT_CPY_NEXT_FIELD(p_out->vip_activity_2);
		INT_CPY_NEXT_FIELD(p_out->vip_activity_3);
		INT_CPY_NEXT_FIELD(p_out->vip_activity_4);
		if(p_out->vip_activity_3 > 100000000)
		{
			DEBUG_LOG("vip_activity_3 maybe wrong [id=%u vip_activity_3=%u]",userid,p_out->vip_activity_3);
		}
	STD_QUERY_ONE_END();
}
