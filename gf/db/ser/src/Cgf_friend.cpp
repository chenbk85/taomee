#include <time.h> 
#include <algorithm>
#include "db_error.h"
#include "benchapi.h"
#include "Cgf_friend.h"



//xhx_tool
//public:
Cgf_friend::Cgf_friend(mysql_interface * db)
	:CtableRoute(db,"GF" ,"t_gf_friend","userid") 
{ 
}

uint32_t Cgf_friend::get_friend_max(uint32_t vip_type)
{
	if ((vip_type & 0x00000001) == 1) {
		return VIP_FRIENDLIST_MAX_NUM;
	} else {
		return NOR_FRIENDLIST_MAX_NUM;
	}
}

uint32_t Cgf_friend::get_black_max(uint32_t vip_type)
{
	return BLACKLIST_MAX_NUM;
}


int Cgf_friend::is_friend_id_exist(userid_t userid, uint32_t fid, FRIEND_TYPE friend_type, bool* p_existed)
{
	sprintf( this->sqlstr, "select count(*) from %s where userid=%u and friend_id=%u and friend_type=%u",
		this->get_table_name(userid), userid, fid, friend_type);
	*p_existed = false;
	int count = 0;
	STD_QUERY_ONE_BEGIN(this->sqlstr ,USER_ID_NOFIND_ERR );
	  INT_CPY_NEXT_FIELD(count);
	  if (count>0)
	  	*p_existed = true;
	STD_QUERY_ONE_END();
}


int Cgf_friend::add(userid_t userid, uint32_t fid, uint32_t vip_type, FRIEND_TYPE friend_type)
{
	uint32_t num = 0;
	uint32_t friend_max = 1;
	bool is_id_exist_othertype=false;
	bool is_id_exist_thistype=false;
	FRIEND_TYPE from_type = TYPE_FRIEND;

	if (userid == fid)
	{
		return GF_FRIEND_SELF_ERR;
	}


	if (friend_type == TYPE_FRIEND)
	{
		friend_max = this->get_friend_max(vip_type);
	}
	else if (friend_type == TYPE_BLACK)
	{
		friend_max = this->get_black_max(vip_type);
	}
	else
	{
		friend_max = 1;
	}
		
	
	ret=this->get_friend_count(userid,friend_type,&num);
	if(DB_SUCC == ret)
	{
	    if (num < friend_max)
    	{
    		if (friend_type == TYPE_FRIEND)
			{
				from_type = TYPE_BLACK;
			}
			else if (friend_type == TYPE_BLACK)
			{
				from_type = TYPE_FRIEND;
			}
			
			if (this->is_friend_id_exist(userid, fid,from_type,&is_id_exist_othertype) != DB_SUCC)
			{
				return DB_ERR;
			}
			if (this->is_friend_id_exist(userid, fid,friend_type,&is_id_exist_thistype) != DB_SUCC)
			{
				return DB_ERR;
			}

			if (is_id_exist_thistype)
			{
				if (is_id_exist_othertype)
					this->del(userid,fid,from_type);
				return GF_FRIENDID_EXISTED_ERR;
			}
			else
			{
				if (is_id_exist_othertype)
				{
					return this->update_friend_type(userid, fid, from_type, friend_type);
				}
				else
				{
					return this->insert(userid, fid, friend_type);
				}
			}
    	}
		else
		{
			return GF_FRIEND_MAX_ERR;
		}
	}
	else
	{
		return ret;
	}
}


int Cgf_friend::del( userid_t userid, uint32_t fid, FRIEND_TYPE friend_type )
{
	GEN_SQLSTR( this->sqlstr, "delete from %s where userid=%u and friend_id=%u and friend_type=%u",
		this->get_table_name(userid), userid, fid, friend_type);
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR );	
}

int Cgf_friend::del( userid_t userid, uint32_t fid )
{
	GEN_SQLSTR( this->sqlstr, "delete from %s where userid=%u and friend_id=%u ",
		this->get_table_name(userid), userid, fid);
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR );	
}


int Cgf_friend::get_friendid_list(userid_t userid, FRIEND_TYPE friend_type, uint32_t* p_count,stru_friedid_list** pp_list)
{
	if (friend_type==TYPE_FRIENDALL)
	{
		GEN_SQLSTR(this->sqlstr, "select friend_id from %s \
		where userid=%u;",
		this->get_table_name(userid), userid);	
	}
	else
	{
		GEN_SQLSTR(this->sqlstr, "select friend_id from %s \
		where userid=%u and friend_type=%u ;",
		this->get_table_name(userid), userid, friend_type);	

	}
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->id );
	STD_QUERY_WHILE_END();

}


int Cgf_friend::get_friendid_list_type(userid_t userid, FRIEND_TYPE friend_type, uint32_t* p_count
	,gf_get_friendlist_type_out_item** pp_list)
{
	if (friend_type==TYPE_FRIENDALL)
	{
		GEN_SQLSTR(this->sqlstr,"select friend_id,friend_type from %s \
		where userid=%u ;",
		this->get_table_name(userid), userid);	
	}
	else
	{
		GEN_SQLSTR(this->sqlstr,"select friend_id,friend_type from %s \
		where userid=%u and friend_type=%u ;",
		this->get_table_name(userid), userid, friend_type);	

	}
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->friend_id );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->friend_type );
	STD_QUERY_WHILE_END();

}


int Cgf_friend::get_friend_count(userid_t userid, FRIEND_TYPE friend_type, uint32_t* p_count)
{
	*p_count = 0;

	sprintf( this->sqlstr, "select count(1) from %s where userid=%u and friend_type=%u",
		this->get_table_name(userid), userid, friend_type);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR );
	INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}


int Cgf_friend::insert(userid_t userid, uint32_t fid, FRIEND_TYPE friend_type)
{
	GEN_SQLSTR(this->sqlstr,"insert into %s (userid,friend_id,friend_type) values (%u,%u,%u)",
		this->get_table_name(userid), userid, fid, friend_type);
	return this->exec_insert_sql (this->sqlstr, GF_FRIENDID_EXISTED_ERR);
}

int Cgf_friend::update_friend_type( userid_t userid, uint32_t fid,  FRIEND_TYPE from_type, FRIEND_TYPE to_type)
{
	GEN_SQLSTR( this->sqlstr, "update %s set friend_type=%u \
		where userid=%u and friend_id=%u and friend_type=%u" ,
		this->get_table_name(userid), to_type, userid, fid, from_type);
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR );
}

int Cgf_friend::clear_role_friend(userid_t userid)
{
	GEN_SQLSTR(this->sqlstr,"delete from %s where userid=%u;",
		this->get_table_name(userid), userid);
	
	return this->exec_update_list_sql(this->sqlstr, SUCC);
}



