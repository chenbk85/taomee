/*
 * =====================================================================================
 * 
 *       Filename:  Ccup.h
 * 
 *    Description:  保存客户端私有数据
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

#ifndef  CROOMINFO_CLIENT_DATA_INCL
#define  CROOMINFO_CLIENT_DATA_INCL

#include <algorithm>
#include <cstring>
#include "CtableRoute10x10.h"
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"
#include "serlib/user.hpp"
#include "serlib/db_utility.hpp"

class Croominfo_client_data : public CtableRoute10x10
{
public:
	Croominfo_client_data (mysql_interface *db)
		:CtableRoute10x10(db, "ROOMINFO", "t_roominfo_client_data", "userid")
	{
	
	}

	void userid(const userid_t uid)
	{
		_userid=uid;
	}

	void type_id(const uint32_t tid)
	{
		_typeid=tid;
	}

	void key_init(const userid_t uid, const uint32_t tid)
	{
		_userid=uid;
		_typeid=tid;
	}

	DB_MACRO_define_value_type(filed_userid_t, userid, userid_t)
	DB_MACRO_define_value_type(typeid_t, typeid, uint32_t)
	DB_MACRO_define_value_type(data_client_t, data_client, uint32_t)

	DB_MACRO_define_template_insert_by_userid_id(userid, _userid, typeid, _typeid, KEY_EXISTED_ERR)
	DB_MACRO_define_template_select_1_by_userid_id(userid, _userid, typeid, _typeid, ID_NOT_EXIST_ERR)
	DB_MACRO_define_template_update_1_by_userid_id(userid, _userid, typeid, _typeid, ID_NOT_EXIST_ERR)

	DB_MACRO_define_template_select_list_1_by_userid(userid, _userid, ID_NOT_EXIST_ERR)
private:
	userid_t _userid;
	uint32_t _typeid;
};

#endif   /* ----- #ifndef CROOMINFO_INCL  ----- */

