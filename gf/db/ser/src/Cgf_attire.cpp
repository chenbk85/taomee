#include <algorithm>
#include "db_error.h"
#include "Cgf_attire.h"

using namespace std;

Cgf_attire::Cgf_attire(mysql_interface * db ) 
	:CtableRoute( db,"GF" ,"t_gf_attire","userid")
{ 

}

/* *
 *@fn取得穿在身上的装备 
 *@brief cmd route interface
 * */
int Cgf_attire::get_used_clothes_list(userid_t userid, uint32_t role_regtime, uint32_t* p_count,
		gf_get_role_list_out_item_2** pp_list)
{
	GEN_SQLSTR(this->sqlstr,"select attireid,id,duration,attirelv from %s \
		where userid=%u and role_regtime=%u and usedflag=1",
		this->get_table_name(userid),userid,role_regtime);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->attireid );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->id );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->duration );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->attirelv );
	STD_QUERY_WHILE_END();
}

/* *
 *@fn取得穿在身上的装备 with duration
 *@brief cmd route interface
 * */
int Cgf_attire::get_used_clothes_list_with_duration(userid_t userid, uint32_t role_regtime, uint32_t* p_count,
		gf_attire_id_duration_list** pp_list)
{
	GEN_SQLSTR(this->sqlstr,"select attireid,id,duration,attirelv,gettime,endtime from %s \
		where userid=%u and role_regtime=%u and usedflag=1",
		this->get_table_name(userid),userid,role_regtime);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->attireid );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->id );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->duration );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->attirelv );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->gettime );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->timelag );
	STD_QUERY_WHILE_END();
}

/* *
 *@fn取得背包的装备 with duration
 *@brief cmd route interface
 * */
int Cgf_attire::get_package_clothes_list(userid_t userid, uint32_t role_regtime, uint32_t* p_count, gf_attire_id_duration_list** pp_list)
{
	GEN_SQLSTR(this->sqlstr,"select attireid,id,duration,attirelv,gettime,endtime from %s \
		where userid=%u and role_regtime=%u and usedflag=0",
		this->get_table_name(userid),userid,role_regtime);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->attireid );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->id );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->duration );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->attirelv );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->gettime );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->timelag );
	STD_QUERY_WHILE_END();
}

int Cgf_attire::get_clothes_list(userid_t userid, uint32_t role_regtime, uint32_t* p_count,
		gf_get_clothes_list_out_item** pp_list)
{
	GEN_SQLSTR(this->sqlstr,"select attireid,id,duration,attirelv,gettime,endtime,usedflag from %s \
		where userid=%u and role_regtime=%u and usedflag < 2",
		this->get_table_name(userid), userid, role_regtime);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->attireid );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->id );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->duration );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->attirelv );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->gettime );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->timelag );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->usedflag );
	STD_QUERY_WHILE_END();
}

int Cgf_attire::get_clothes_list_ex(userid_t userid, uint32_t role_regtime, uint32_t* p_count,
		gf_get_clothes_list_ex_out_item** pp_list)
{
	GEN_SQLSTR(this->sqlstr,"select attireid,id,usedflag,duration from %s \
		where userid=%u and role_regtime=%u",
		this->get_table_name(userid), userid, role_regtime);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->attireid );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->id );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->usedflag );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->duration );
	STD_QUERY_WHILE_END();
}


int Cgf_attire::set_all_clothes_noused(userid_t userid, uint32_t role_regtime)
{
	GEN_SQLSTR(this->sqlstr,"update %s set usedflag = 0 \
			where userid = %u and role_regtime=%u and usedflag = 1",
		this->get_table_name(userid),userid,role_regtime);
	return this->exec_update_list_sql(this->sqlstr, SUCC);
}

int Cgf_attire::set_clothes_noused(userid_t userid, uint32_t role_regtime, uint32_t id)
{
	GEN_SQLSTR(this->sqlstr,"update %s set usedflag = 0 \
			where userid = %u and role_regtime=%u and id= %u",
		this->get_table_name(userid),userid,role_regtime, id);
	return this->exec_update_list_sql(this->sqlstr, SUCC);
}

int Cgf_attire::set_clothes_outdated(userid_t userid, uint32_t role_regtime, uint32_t outdate)
{
    GEN_SQLSTR(this->sqlstr,"update %s set usedflag = 2 \
        where userid = %u and (endtime !=0 and endtime < %u)",
        this->get_table_name(userid),userid, outdate);
    return this->exec_update_list_sql(this->sqlstr, SUCC);
}

int Cgf_attire::get_clothes_outdated(userid_t userid, uint32_t role_regtime, uint32_t outdate, 
	uint32_t** pp_out_list, uint32_t* p_out_count)
{
    GEN_SQLSTR(this->sqlstr,"select role_regtime,count(*) from %s \
        where userid = %u and usedflag<>2 and (endtime !=0 and endtime < %u) group by role_regtime;",
        this->get_table_name(userid),userid, outdate);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out_list, p_out_count);
		INT_CPY_NEXT_FIELD( *(*pp_out_list+i) );
	STD_QUERY_WHILE_END();

}

int Cgf_attire::set_clothes_used_by_timelist(userid_t userid, uint32_t role_regtime, 
		gf_wear_clothes_in_item* p_in_item, uint32_t in_count)
{
	char formatstr[3000] = {};
	char valuestr[10];

	if(in_count == 0)
		return SUCC;

	for (uint32_t i = 0; i < in_count; i++) {
		sprintf(valuestr, "%u,", p_in_item[i].id);
		strcat(formatstr, valuestr);
	}
	formatstr[strlen(formatstr)-1] = '\0';

	GEN_SQLSTR(this->sqlstr,"update %s set usedflag = 1 \
			where userid = %u and role_regtime=%u and id in (%s)",
		this->get_table_name(userid), userid, role_regtime, formatstr);
	return 	this->exec_update_list_sql(this->sqlstr, SUCC);
}


int Cgf_attire::set_clothes_flag_equip_part(userid_t userid, uint32_t role_regtime, 
	uint32_t unique_id, uint32_t equip_part)
{
	GEN_SQLSTR(this->sqlstr,"update %s set usedflag = 1, equip_part=%u \
			where userid = %u and role_regtime=%u and id=%u ",
		this->get_table_name(userid), equip_part, userid, role_regtime, unique_id);
	return 	this->exec_update_list_sql(this->sqlstr, SUCC);
}

int Cgf_attire::get_clothes_info_for_web(userid_t userid, uint32_t role_regtime, uint32_t* p_count, 
	gf_get_clothes_info_for_web_out_item** pp_out_list)
{
	GEN_SQLSTR(this->sqlstr,"select attireid,attirelv,duration,equip_part from %s \
			where userid = %u and role_regtime=%u and usedflag>0",
		this->get_table_name(userid), userid, role_regtime);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_out_list+i)->attire_id );
		INT_CPY_NEXT_FIELD( (*pp_out_list+i)->attirelv );
		INT_CPY_NEXT_FIELD( (*pp_out_list+i)->duration );
		INT_CPY_NEXT_FIELD( (*pp_out_list+i)->equip_part );
	STD_QUERY_WHILE_END();
}


int Cgf_attire::get_clothes_by_timelist(userid_t userid, uint32_t role_regtime, 
		gf_wear_clothes_in_item* p_in_item, uint32_t in_count,
		gf_wear_clothes_out_item** pp_out_list, uint32_t* p_out_count)
{
	char formatstr[3000] = {};
	char valuestr[10];
	
	for (uint32_t i = 0; i < in_count; i++) {
		sprintf(valuestr, "%u,", p_in_item[i].id);
		strcat(formatstr, valuestr);
	}
	formatstr[strlen(formatstr)-1] = '\0';

	GEN_SQLSTR(this->sqlstr,"select attireid,id,duration,attirelv,gettime,endtime from %s \
			where userid = %u and role_regtime=%u and id in (%s)",
		this->get_table_name(userid), userid, role_regtime, formatstr);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out_list, p_out_count);
		INT_CPY_NEXT_FIELD( (*pp_out_list+i)->clothes_id );
		INT_CPY_NEXT_FIELD( (*pp_out_list+i)->id );
		INT_CPY_NEXT_FIELD( (*pp_out_list+i)->duration );
		INT_CPY_NEXT_FIELD( (*pp_out_list+i)->attirelv );
		INT_CPY_NEXT_FIELD( (*pp_out_list+i)->gettime );
		INT_CPY_NEXT_FIELD( (*pp_out_list+i)->endtime );
	STD_QUERY_WHILE_END();
}

int Cgf_attire::get_user_attireid_by_usedflg(userid_t userid, uint32_t role_regtime, uint32_t* p_count, 
	gf_get_user_partial_info_out_item** pp_list, uint32_t usedflg)
{
	GEN_SQLSTR(this->sqlstr,"select attireid,gettime,endtime,attirelv from %s \
		where userid=%u and role_regtime=%u and usedflag=%u ",
		this->get_table_name(userid),userid,role_regtime,usedflg);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->id );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->gettime );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->endtime );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->attirelv );
	STD_QUERY_WHILE_END();
}

int Cgf_attire::get_attire_by_usedflg(userid_t userid,uint32_t role_regtime,uint32_t* p_cnt,
    gf_get_outdated_product_out_item** pp_list, uint32_t usedflg)
{
    GEN_SQLSTR(this->sqlstr,"select attireid,id,duration,attirelv from %s \
		where userid=%u and role_regtime=%u and usedflag=%u ",
		this->get_table_name(userid),userid,role_regtime,usedflg);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_cnt);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->attireid );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->uniquekey );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->duration );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->attirelv );
	STD_QUERY_WHILE_END();
}

int Cgf_attire::del_attire(userid_t userid,uint32_t role_regtime,gf_del_attire_in* p_in)
{
	GEN_SQLSTR(this->sqlstr,"delete from %s where userid = %u and role_regtime=%u \
		and attireid = %u and id = %u ",
		this->get_table_name(userid),userid,role_regtime,p_in->attireid,p_in->index_id);
	return this->exec_update_sql(this->sqlstr, GF_ATTIREID_NOFIND_ERR);
}

int Cgf_attire::del_attire_with_index(userid_t userid,uint32_t role_regtime,uint32_t index)
{
	GEN_SQLSTR(this->sqlstr,"delete from %s where userid = %u and role_regtime=%u \
		and id = %u ",
		this->get_table_name(userid),userid,role_regtime,index);
	return this->exec_update_sql(this->sqlstr, GF_ATTIREID_NOFIND_ERR);
}

int Cgf_attire::del_attire_by_attireid(userid_t userid,uint32_t role_regtime,uint32_t attireid)
{
	GEN_SQLSTR(this->sqlstr,"delete from %s where userid = %u and role_regtime=%u \
		and attireid = %u ",
		this->get_table_name(userid),userid,role_regtime,attireid);
	return this->exec_update_sql(this->sqlstr, GF_ATTIREID_NOFIND_ERR);
}

int Cgf_attire::del_timeout_attire_by_attireid(userid_t userid,uint32_t role_regtime,uint32_t attireid)
{
	GEN_SQLSTR(this->sqlstr,"delete from %s where userid = %u and role_regtime=%u \
		and attireid = %u and usedflag = 2",
		this->get_table_name(userid),userid,role_regtime,attireid);
	return this->exec_update_sql(this->sqlstr, GF_ATTIREID_NOFIND_ERR);
}

int Cgf_attire::get_attire_cnt(userid_t userid, uint32_t role_regtime, uint32_t* p_count, uint32_t usedflag)
{
	if (usedflag == 0xffffffff)
	{// all of the attires include used or unused
		GEN_SQLSTR(this->sqlstr,"select count(id) from %s \
		where userid=%u and role_regtime=%u and usedflag < 2",
		this->get_table_name(userid), userid, role_regtime);
	}
	else
	{
		GEN_SQLSTR(this->sqlstr,"select count(id) from %s \
		where userid=%u and role_regtime=%u and usedflag=%u",
		this->get_table_name(userid), userid, role_regtime, usedflag);
	}

	STD_QUERY_ONE_BEGIN(this->sqlstr,GF_ATTIREID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD( *p_count );
	STD_QUERY_ONE_END();

}
int Cgf_attire::get_attire_cnt_by_attireid(userid_t userid, uint32_t role_regtime, uint32_t* p_count, uint32_t id)
{
    GEN_SQLSTR(this->sqlstr,"select count(id) from %s \
        where userid=%u and role_regtime=%u and attireid=%u",
        this->get_table_name(userid), userid, role_regtime, id);

	STD_QUERY_ONE_BEGIN(this->sqlstr,GF_ATTIREID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD( *p_count );
	STD_QUERY_ONE_END();

}
int Cgf_attire::get_attire_cnt_by_unique_id(userid_t userid, uint32_t role_regtime, uint32_t* p_count, uint32_t id)
{
    GEN_SQLSTR(this->sqlstr,"select count(id) from %s \
        where userid=%u and role_regtime=%u and id=%u",
        this->get_table_name(userid), userid, role_regtime, id);

	STD_QUERY_ONE_BEGIN(this->sqlstr,GF_ATTIREID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD( *p_count );
	STD_QUERY_ONE_END();

}


int Cgf_attire::add_attire(userid_t userid, uint32_t role_regtime, add_attire_t* p_in, uint32_t max_attire_bag_count)
{
	uint32_t attire_cnt = 0;
	int ret = get_attire_cnt(userid, role_regtime, &attire_cnt, p_in->usedflag);
	if (ret != SUCC)
	{
		return ret;
	}
	if (p_in->usedflag == 1)
	{
		if (attire_cnt >= MAX_WEAR_CLOTHES_NUM)
		{
			return GF_ATTIRE_MAX_ERR;
		}
	}
	else if (p_in->usedflag == 0)
	{
		//if (attire_cnt >= MAX_ATTIRE_CNT)
		if(attire_cnt >= max_attire_bag_count)	
		{
			return GF_ATTIRE_MAX_ERR;
		}
	}
	else
	{
		if (attire_cnt >= MAX_WEAR_CLOTHES_NUM + max_attire_bag_count )
		{
			return GF_ATTIRE_MAX_ERR;
		}		
	}

	GEN_SQLSTR(this->sqlstr,"insert into %s (userid,role_regtime,gettime,endtime,attireid,attire_rank,\
        usedflag,duration,  attirelv) values (%u,%u,%u,%u,%u,%u,%u,%u, %u)",
		this->get_table_name(userid), userid, role_regtime, p_in->gettime, p_in->endtime, 
		p_in->attireid, p_in->attire_rank, p_in->usedflag, p_in->duration, p_in->attire_lv);
	return this->exec_insert_sql  (this->sqlstr, GF_ATTIREID_EXISTED_ERR);	
}

int Cgf_attire::add_attire(userid_t userid,uint32_t role_regtime, add_attire_t* p_in, uint32_t max_attire_bag_count, uint32_t *last_insert_id)
{
	*last_insert_id = 0;
	uint32_t attire_cnt = 0;
        int ret = get_attire_cnt(userid, role_regtime, &attire_cnt, p_in->usedflag);
        if (ret != SUCC)
        {
                return ret;
        }
        if (p_in->usedflag == 1)
        {
                if (attire_cnt >= MAX_WEAR_CLOTHES_NUM)
                {
                        return GF_ATTIRE_MAX_ERR;
                }
        }
        else if (p_in->usedflag == 0)
        {
                if (attire_cnt >= max_attire_bag_count)
                {
                        return GF_ATTIRE_MAX_ERR;
                }
        }
        else
        {
                if (attire_cnt >= MAX_WEAR_CLOTHES_NUM + max_attire_bag_count)
                {
                        return GF_ATTIRE_MAX_ERR;
                }
        }

        GEN_SQLSTR(this->sqlstr,"insert into %s (userid,role_regtime,gettime,endtime,attireid,attire_rank,\
            usedflag,duration, attirelv) values (%u,%u,%u,%u,%u,%u,%u,%u, %u)",
                this->get_table_name(userid), userid, role_regtime, p_in->gettime, p_in->endtime,
                p_in->attireid, p_in->attire_rank, p_in->usedflag, p_in->duration, p_in->attire_lv);
		return exec_insert_sql_get_auto_increment_id(sqlstr,  GF_ATTIREID_EXISTED_ERR, last_insert_id);
}

int Cgf_attire::get_max_id(userid_t userid,uint32_t role_regtime,uint32_t* p_iter_id)
{
	GEN_SQLSTR(this->sqlstr,"select max(id) from %s \
		where userid=%u and role_regtime=%u",
		this->get_table_name(userid), userid, role_regtime);
	STD_QUERY_ONE_BEGIN(this->sqlstr, GF_ATTIREID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD( *p_iter_id );
	STD_QUERY_ONE_END();
}

int Cgf_attire::clear_role_attire(userid_t userid,uint32_t role_regtime)
{
	GEN_SQLSTR(this->sqlstr,"delete from %s where userid=%u and role_regtime=%u",
		this->get_table_name(userid), userid, role_regtime);
	
	return this->exec_update_list_sql(this->sqlstr, SUCC);
}

int Cgf_attire::get_max_id_list(userid_t userid, uint32_t role_regtime, uint32_t in_count,
		uint32_t* p_out_count, gf_buy_attire_out_item** pp_out_list)
{

	GEN_SQLSTR(this->sqlstr,"select attireid,id,duration from %s \
		where userid=%u and role_regtime=%u order by id desc limit %d ;",
		this->get_table_name(userid), userid, role_regtime,in_count);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out_list, p_out_count);
		INT_CPY_NEXT_FIELD( (*pp_out_list+i)->attireid );
		INT_CPY_NEXT_FIELD( (*pp_out_list+i)->id );
	STD_QUERY_WHILE_END();
}

int Cgf_attire::get_max_id_list(userid_t userid, uint32_t role_regtime, uint32_t in_count,
		uint32_t* p_out_count, gf_task_swap_item_out_item** pp_out_list)
{

	GEN_SQLSTR(this->sqlstr,"select attireid,id,gettime,endtime from %s \
		where userid=%u and role_regtime=%u order by id desc limit %d ;",
		this->get_table_name(userid), userid, role_regtime,in_count);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out_list, p_out_count);
		INT_CPY_NEXT_FIELD( (*pp_out_list+i)->attireid );
		INT_CPY_NEXT_FIELD( (*pp_out_list+i)->index_id );
		INT_CPY_NEXT_FIELD( (*pp_out_list+i)->gettime );
		INT_CPY_NEXT_FIELD( (*pp_out_list+i)->endtime );
	STD_QUERY_WHILE_END();
}

int Cgf_attire::set_attire_list_duration(userid_t userid, uint32_t role_regtime, uint32_t in_count,
		gf_set_role_base_info_in_item* p_item)
{
	int ret = 0;
	for (uint32_t i = 0; i < in_count; ++i)
	{
		ret |= this->update_duration(userid, role_regtime, p_item->attire_index, p_item->duration);
		++p_item;
	}
	return ret;
}

int Cgf_attire::update_duration (userid_t userid, uint32_t role_regtime, uint32_t attire_index, uint32_t duration)
{
	GEN_SQLSTR(this->sqlstr,"update %s set duration=%u where id=%u;", 
		this->get_table_name(userid), duration, attire_index);
	
	return this->exec_update_sql(this->sqlstr, GF_ATTIREID_NOFIND_ERR);
}

int Cgf_attire::get_outdated_attire_by_index(userid_t userid, uint32_t role_regtime, uint32_t key, uint32_t* id)
{
	GEN_SQLSTR(this->sqlstr,"select attireid from %s \
		where userid=%u and role_regtime=%u and id=%u and usedflag=2",
		this->get_table_name(userid), userid, role_regtime, key);
	STD_QUERY_ONE_BEGIN(this->sqlstr, GF_ATTIREID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD( *id );
	STD_QUERY_ONE_END();
}

int Cgf_attire::get_duration_by_index (userid_t userid, uint32_t role_regtime, uint32_t attire_index, uint32_t* p_duration)
{
	GEN_SQLSTR(this->sqlstr,"select duration from %s \
		where userid=%u and role_regtime=%u and id=%u",
		this->get_table_name(userid), userid, role_regtime, attire_index);
	STD_QUERY_ONE_BEGIN(this->sqlstr, GF_ATTIREID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD( *p_duration );
	STD_QUERY_ONE_END();
}

int Cgf_attire::set_attire_int_value(userid_t userid,uint32_t role_regtime, uint32_t indexid ,const char * field_type , uint32_t  value)
{
	sprintf( this->sqlstr, "update %s set %s =%u where %s=%u and role_regtime=%u and id=%u;" ,
		this->get_table_name(userid),field_type,value ,this->id_name, userid, role_regtime, indexid );
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);	
}

int Cgf_attire::get_attire_by_index(userid_t userid, uint32_t role_regtime, uint32_t id, uint32_t item_id,  add_attire_t* p_in)
{
	memset(p_in, 0, sizeof(add_attire_t));

	GEN_SQLSTR(sqlstr, "select attireid, gettime, attire_rank, duration, usedflag, endtime, attirelv from %s where id = %u and userid = %u and role_regtime = %u and attireid = %u and usedflag = 0 ", get_table_name(userid), id, userid, role_regtime, item_id);
	STD_QUERY_ONE_BEGIN(this->sqlstr, GF_ATTIREID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD( p_in->attireid );
		INT_CPY_NEXT_FIELD( p_in->gettime );
		INT_CPY_NEXT_FIELD( p_in->attire_rank);
		INT_CPY_NEXT_FIELD( p_in->duration);
		INT_CPY_NEXT_FIELD( p_in->usedflag);
		INT_CPY_NEXT_FIELD( p_in->endtime);
		INT_CPY_NEXT_FIELD( p_in->attire_lv);
    STD_QUERY_ONE_END();		
}

int Cgf_attire::getback_attire_by_index(userid_t userid,uint32_t role_regtime,uint32_t indexid,uint32_t gettime,uint32_t endtime)
{
	sprintf( this->sqlstr, "update %s set usedflag=%u, gettime=%u, endtime=%u \
        where userid=%u and role_regtime=%u and id=%u;" ,
		this->get_table_name(userid), 0, gettime, endtime, userid, role_regtime, indexid );
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);	
}

int Cgf_attire::get_clothes_list_for_boss(userid_t userid, uint32_t role_regtime, uint32_t* p_count, gf_get_role_base_info_for_boss_out_item** pp_list)
{
    GEN_SQLSTR(this->sqlstr,"select attireid,attirelv from %s \
		where userid=%u and role_regtime=%u and usedflag=%u ",
		this->get_table_name(userid),userid,role_regtime,1);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->clothes_id );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->level );
	STD_QUERY_WHILE_END();

}
