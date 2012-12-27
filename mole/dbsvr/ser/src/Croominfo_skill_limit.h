/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_skill_limit.h
 *
 *    Description:  设置每天技能使用上限，获取物品的上限
 *
 *        Version:  1.0
 *        Created:  02/03/2010 11:33:55 AM
 *       Revision:  none
 *       Compiler:  gcc -Wall -Wextra Croominfo_skill_limit.h
 *
 *         Author:  easyeagel (LiuGuangBao), easyeagel@gmx.com
 *        Company:  淘米网络-taomee.com
 *
 * =====================================================================================
 */

#ifndef  CROOMINFO_SKILL_LIMIT_INCL
#define  CROOMINFO_SKILL_LIMIT_INCL

#include <algorithm>
#include <cstring>
#include "CtableRoute10x10.h"
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"
#include "serlib/user.hpp"
#include "serlib/db_utility.hpp"


class Croominfo_skill_limit : public CtableRoute10x10
{
public:
	enum { skill_max_limit_in_count=10, skill_max_limit_in_byte=skill_max_limit_in_count*4*2+4};

	Croominfo_skill_limit (mysql_interface *db)
		:CtableRoute10x10(db, "ROOMINFO", "t_roominfo_skill_limit", "userid")
	{
	
	}

	void userid(const userid_t uid)
	{
		_userid=uid;
	}

	void skillid(const userid_t sid)
	{
		_skillid=sid;
	}

	void key_init(const userid_t uid, const userid_t sid)
	{
		_userid=uid;
		_skillid=sid;
	}

	struct attire_list_t
	{
		attire_list_t(const std::string& s)
		{
			size_t n=skill_max_limit_in_byte;
			if(s.size()!=n) //不满足条件，设置为初始值
			{
				_str.append(n, '\0');
			} else {
				_str=s;
			}

			_count=(uint32_t*)_str.data();
			_attire=(uint32_t*)(_str.data()+sizeof(*_count));
		}

		size_t count(const uint32_t attireid)
		{
			int n=find(attireid);
			if(-1!=n)
			{
				return _attire[2*n+1];
			}

			return 0;
		}

		void  add_count(const uint32_t aid, const size_t m=1)
		{
			int n=find(aid);
			if(-1==n) return;

			_attire[2*n+1] += m;
		}

		const char* data() const
		{
			return _str.data();
		}

	private:
		int find(const uint32_t aid)
		{
			for(size_t i=0; i<*_count; ++i)
			{
				if(_attire[2*i]==aid)
				{
					return i;
				}
			}

			if(*_count<skill_max_limit_in_count)
			{
				_attire[2 * (*_count)]=aid;
				return (*_count)++;
			}

			return -1;

		}

		uint32_t* _count;
		uint32_t* _attire;
		std::string _str;
	};

	DB_MACRO_define_value_type(filed_userid_t, userid, userid_t)
	DB_MACRO_define_value_type(filed_skillid_t, skillid, uint32_t)
	DB_MACRO_define_buf_value_type(filed_string_t, attire_list, skill_max_limit_in_byte)

	DB_MACRO_define_template_update_1_by_userid_id(userid, _userid, skillid, _skillid, ID_NOT_EXIST_ERR)
	DB_MACRO_define_template_buf_update_1_by_userid_id(userid, _userid, skillid, _skillid, ID_NOT_EXIST_ERR)
	DB_MACRO_define_template_select_1_by_userid_id(userid, _userid, skillid, _skillid, ID_NOT_EXIST_ERR)

	DB_MACRO_define_template_insert_by_userid_id(userid, _userid, skillid, _skillid, ID_NOT_EXIST_ERR)

private:
	userid_t _userid;
	uint32_t _skillid;

};

#endif //CROOMINFO_SKILL_LIMIT_INCL

