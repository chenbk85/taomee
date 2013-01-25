/*
 * =====================================================================================
 *
 *       Filename:  Cpet.cpp
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

#include "Citem.h"
#include <time.h> 
#include "proto.h"
#include "benchapi.h"
#include "db_error.h"
#include "Ccommon.h"
#include "util.h"

Citem::Citem(mysql_interface * db ,Citem_change_log * p_log ) 
	:CtableRoute(db , "MOLE2_USER" , "t_item" , "userid","itemid"),
	str_uid("userid"),
	str_itemid("itemid"),
	str_gid("gridid"),
	str_cnt("count"),
	str_storagecnt("storage_cnt")
{ 

	this->p_item_change_log=p_log;

}

int Citem::insert(userid_t userid, uint32_t itemid, uint32_t gid, uint32_t count, uint32_t storage_cnt)
{
	GEN_SQLSTR(this->sqlstr, "insert into %s values(%u, %u, %u, %u, %u) ",
			this->get_table_name(userid),
			userid,
			itemid,
			gid,
			count,
			storage_cnt);
	return this->exec_insert_sql( this->sqlstr, MOLE2_ITEM_ID_EXISTED_ERR);
}



int Citem::storage_cnt_set(userid_t userid, uint32_t itemid, uint32_t cnt, uint32_t storage_cnt)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s = %u, %s = %u where %s=%u and %s=%u",
			this->get_table_name(userid),
			this->str_cnt,			cnt,
			this->str_storagecnt,	storage_cnt,
			this->str_uid,		userid,
			this->str_itemid,	itemid);
	return this->exec_update_sql(this->sqlstr, MOLE2_ITEM_ID_NOFIND_ERR);
}


int Citem::add_item(userid_t userid, uint32_t itemid, int32_t count, uint32_t * p_left_count, bool is_vip_opt)
{
	if (count == 0) {
		return SUCC;
	}
	uint32_t	db_cnt = 0;
	uint32_t	db_storage_cnt = 0;

	int ret = this->get_all_count(userid, itemid, &db_cnt, &db_storage_cnt);
	if (ret != SUCC && ret != MOLE2_ITEM_ID_NOFIND_ERR) {
		return ret;
	} else if (ret == MOLE2_ITEM_ID_NOFIND_ERR) {
		db_cnt = 0;
	}

	int32_t tmp_cnt = (int32_t)db_cnt;
	LIMIT_ADD(tmp_cnt, count, MOLE2_ITEM_COUNT_MIN, MOLE2_ITEM_COUNT_MAX, MOLE2_ITEM_NOT_ENOUGH_ERR, MOLE2_ITEM_COUNT_MAX_ERR);

	if (p_left_count!=NULL ){
		*p_left_count=tmp_cnt;
	}

	if (ret == MOLE2_ITEM_ID_NOFIND_ERR) {
		ret = this->insert(userid, itemid, MOLE2_ITEM_ON_BODY, tmp_cnt, 0);
	} else if (ret == SUCC) {
		if (tmp_cnt == 0 && db_storage_cnt == 0) {
			ret = this->del_item(userid, itemid);
		} else {
			ret = this->set_int_value(userid, itemid, this->str_cnt, tmp_cnt);
		}
	}
	if (ret!=SUCC) return ret;

	int change_count=tmp_cnt - (int)db_cnt;


	if (itemid<=1000000000){
		//记录日志
		stru_game_change_add log_item;
		log_item.userid= userid; 
		log_item.opt_type=1001 ;//修改物品
		log_item.ex_v1=itemid ;
		log_item.ex_v2=change_count ; 
		game_change_add(log_item );
		ret=this->p_item_change_log->add(time(NULL),is_vip_opt ,userid,itemid,change_count );
	}
	
	return ret;
}

int Citem::add_item_in_storage(userid_t userid, uint32_t itemid, int32_t count, bool is_vip_opt)
{
	if (count == 0) {
		return SUCC;
	}
	uint32_t	db_cnt = 0;
	uint32_t	db_storage_cnt = 0;
	int ret = this->get_all_count(userid, itemid, &db_cnt, &db_storage_cnt);
	if (ret != SUCC && ret != MOLE2_ITEM_ID_NOFIND_ERR) {
		return ret;
	} else if (ret == MOLE2_ITEM_ID_NOFIND_ERR) {
		db_cnt = 0;
	}
	int32_t tmp_storage_cnt = (int32_t)db_storage_cnt;
	//int32_t tmp_cnt = (int32_t)db_cnt;
	LIMIT_ADD(tmp_storage_cnt, count, MOLE2_ITEM_COUNT_MIN, MOLE2_ITEM_COUNT_MAX, MOLE2_ITEM_NOT_ENOUGH_ERR, MOLE2_ITEM_COUNT_MAX_ERR);
	
	if (ret == MOLE2_ITEM_ID_NOFIND_ERR) {
		ret = this->insert(userid, itemid, MOLE2_ITEM_ON_BODY, 0, tmp_storage_cnt);
	} else if (ret == SUCC) {
		if (tmp_storage_cnt == 0 && db_cnt == 0) {
			ret = this->del_item(userid, itemid);
		} else {
			ret = this->set_int_value(userid, itemid, this->str_storagecnt, tmp_storage_cnt); 
		}
	}
	if (ret!=SUCC ) return ret;
	//记录日志
   	stru_game_change_add log_item;
    log_item.userid= userid; 
    log_item.opt_type=1001 ;//修改物品
    log_item.ex_v1=itemid ;
    log_item.ex_v2= tmp_storage_cnt - (int)db_storage_cnt ;
       game_change_add(log_item );

       ret=this->p_item_change_log->add(time(NULL),is_vip_opt ,userid,itemid,
                       tmp_storage_cnt - (int)db_storage_cnt );


       return ret;
}

int Citem::get_item(userid_t userid, uint32_t itemid, uint32_t* p_cnt)
{
       GEN_SQLSTR(this->sqlstr, "select %s from %s where %s=%u and %s=%u",
                       this->str_cnt, this->get_table_name(userid),
                       this->str_uid, userid, this->str_itemid, itemid);
       STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_ITEM_ID_NOFIND_ERR);
               INT_CPY_NEXT_FIELD(*p_cnt);
       STD_QUERY_ONE_END();
}

int Citem::get_flag2(userid_t userid, uint32_t* flag)
{
       GEN_SQLSTR(this->sqlstr, "select %s from %s where %s=%u and %s = %u",
                       this->str_cnt, this->get_table_name(userid),
                       this->str_uid, userid, this->str_itemid, FLAG_TYPE);
       STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_ITEM_ID_NOFIND_ERR);
              INT_CPY_NEXT_FIELD(*flag);
       STD_QUERY_ONE_END();
}

int Citem::set_flag2(userid_t userid, uint32_t flag)
{
    GEN_SQLSTR(this->sqlstr, "update  %s set %s=%u where %s=%u and %s=%u ",
                       this->get_table_name(userid),this->str_cnt,flag,
                       this->str_uid, userid, this->str_itemid, FLAG_TYPE);
	return this->exec_update_sql(this->sqlstr, MOLE2_ITEM_ID_NOFIND_ERR);
}

int Citem::get_item_list(userid_t userid, stru_mole2_user_item_info** pp_out_item,
               uint32_t* p_out_count)
{
       GEN_SQLSTR(this->sqlstr, "select %s, %s from %s where %s=%u and %s<>%u", 
                       this->str_itemid, this->str_cnt,
                       this->get_table_name(userid),
                       this->str_uid,  userid,
                       this->str_cnt,  0);
       STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out_item, p_out_count);
               INT_CPY_NEXT_FIELD((*pp_out_item + i)->itemid);
               //INT_CPY_NEXT_FIELD((*pp_out_item + i)->gid);
               INT_CPY_NEXT_FIELD((*pp_out_item + i)->count);
       STD_QUERY_WHILE_END();
}

int Citem::storage_list_get(userid_t userid, stru_mole2_user_item_info** pp_out_item,
               uint32_t* p_out_count)
{
       GEN_SQLSTR(this->sqlstr, "select %s, %s from %s where %s=%u and %s<>%u", 
                       this->str_itemid,
                       this->str_storagecnt,
                       this->get_table_name(userid),
                       this->str_uid,          userid,
                       this->str_storagecnt,   0);
       STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out_item, p_out_count);
               INT_CPY_NEXT_FIELD((*pp_out_item + i)->itemid);
               //INT_CPY_NEXT_FIELD((*pp_out_item + i)->gid);
               INT_CPY_NEXT_FIELD((*pp_out_item + i)->count);
       STD_QUERY_WHILE_END();
}

int Citem::get_item_list_ranged(userid_t userid, mole2_user_item_ranged_get_list_in* p_in, 
               mole2_user_item_ranged_get_list_out_item** pp_out_item, uint32_t* p_out_count)
{
       GEN_SQLSTR(this->sqlstr, "select %s, %s, %s from %s where %s=%u and %s>=%u and %s<=%u", 
                       this->str_itemid, this->str_cnt,this->str_storagecnt,this->get_table_name(userid), 
                       this->str_uid, userid, this->str_itemid, p_in->start_id, this->str_itemid, p_in->end_id);
       STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out_item, p_out_count);
               INT_CPY_NEXT_FIELD((*pp_out_item + i)->itemid);
               INT_CPY_NEXT_FIELD((*pp_out_item + i)->bag_count);
               INT_CPY_NEXT_FIELD((*pp_out_item + i)->store_count);
       STD_QUERY_WHILE_END();
}

int Citem::get_grids_used(userid_t userid, uint32_t** pp_grid_item, uint32_t* p_count)
{
       GEN_SQLSTR(this->sqlstr, "select %s from %s where %s=%u order by %s",
                       this->str_gid, this->get_table_name(userid), 
                       this->str_uid, userid, this->str_gid);
       STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_grid_item, p_count);
               INT_CPY_NEXT_FIELD(*(*pp_grid_item + i));
       STD_QUERY_WHILE_END();
}

int Citem::del_item(userid_t userid, uint32_t itemid)
{
	GEN_SQLSTR(this->sqlstr, "delete from %s where %s=%u and %s=%u",
			this->get_table_name(userid), this->str_uid, userid, this->str_itemid, itemid);
	return this->exec_update_sql(this->sqlstr, MOLE2_ITEM_ID_NOFIND_ERR);
}

/* 
int Citem::update_grid(userid_t userid, mole2_user_item_update_grid_in* p_in)
{
	uint32_t gid_old, itemid_old;
	if (p_in->gid > MOLE2_MAX_ITEM_BAG_NUM)
		return MOLE2_ITEM_BAG_FULL_ERR;
	int ret = this->get_item_grid(userid, p_in->itemid, &gid_old);
	if (ret != SUCC)
		return ret;

	if (this->get_item_in_grid(userid, &itemid_old, p_in->gid) == SUCC)
	{
		ret = this->set_int_value(userid, itemid_old, this->str_gid, gid_old);
		if (ret != SUCC)
			return ret;

		ret = this->set_int_value(userid, p_in->itemid, this->str_gid, p_in->gid);
		if (ret != SUCC)
			STD_ROLLBACK();
		return ret;
	}
	else
		return this->set_int_value(userid, p_in->itemid, this->str_gid, p_in->gid);
}
*/

int Citem::get_item_grid(userid_t userid, uint32_t itemid, uint32_t* gid)
{

	GEN_SQLSTR(this->sqlstr, "select %s from %s where %s=%u and %s=%u", this->str_gid,
			this->get_table_name(userid), this->str_uid, userid, this->str_itemid, itemid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_ITEM_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*gid);
	STD_QUERY_ONE_END();
}

int Citem::get_item_in_grid(userid_t userid, uint32_t *itemid, uint32_t gid)
{
	GEN_SQLSTR(this->sqlstr, "select %s from %s where %s=%u and %s=%u", this->str_itemid,
			this->get_table_name(userid), this->str_uid, userid, this->str_gid, gid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_ITEM_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*itemid);
	STD_QUERY_ONE_END();
}

int Citem::get_all_count(userid_t	userid, uint32_t itemid, uint32_t* p_count, uint32_t* p_storage_cnt)
{
	GEN_SQLSTR(this->sqlstr, "select %s, %s from %s where %s=%u and %s=%u",
			this->str_cnt,
			this->str_storagecnt,
			this->get_table_name(userid),
			this->str_uid, userid,
			this->str_itemid, itemid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_ITEM_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_count);
		INT_CPY_NEXT_FIELD(*p_storage_cnt);
	STD_QUERY_ONE_END();
}

int Citem::list_get(uint32_t userid, mole2_user_user_login_ex_out_item_5** pp_out, uint32_t* p_count)
{
	sprintf(this->sqlstr, "select itemid, count from %s where userid = %u",
		this->get_table_name(userid),
		userid
	);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out, p_count);
		INT_CPY_NEXT_FIELD((*pp_out + i)->itemid);
		INT_CPY_NEXT_FIELD((*pp_out + i)->count);
	STD_QUERY_WHILE_END();
}

int Citem::get_items(uint32_t userid, std::vector<stru_item_info> &items)
{
	sprintf(this->sqlstr, "select itemid, count, storage_cnt from %s where userid = %u",
		this->get_table_name(userid),userid);
	STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr, items);
		INT_CPY_NEXT_FIELD(item.itemid);
		INT_CPY_NEXT_FIELD(item.count);
		INT_CPY_NEXT_FIELD(item.storage_cnt);
	STD_QUERY_WHILE_END_NEW();
}


