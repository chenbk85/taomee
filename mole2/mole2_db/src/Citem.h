
/*
 * =====================================================================================
 * 
 *       Filename:  Cpet.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  g++
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CITEM_INC
#define  CITEM_INC
#include "CtableRoute.h"
#include "Citem_change_log.h"
#include "proto.h"
#include "benchapi.h"

#define MOLE2_MAX_ITEM_BAG_NUM	32
#define MOLE2_ITEM_COUNT_MAX	99999
#define MOLE2_ITEM_COUNT_MIN	0
#define FLAG_TYPE   2000000000
#define	MOLE2_ITEM_ON_BODY		0


class Citem: public CtableRoute
{
private: 
	Citem_change_log *p_item_change_log;
	int del_item(userid_t userid, uint32_t itemid);

public:
	int insert(userid_t userid, uint32_t itemid, uint32_t gid, uint32_t count, uint32_t storage_cnt);

	int storage_cnt_set(userid_t userid, uint32_t itemid, uint32_t cnt, uint32_t storage_cnt);
	
	int get_grids_used(userid_t userid, uint32_t** pp_grid_item, uint32_t* p_count);
	int get_item_grid(userid_t userid, uint32_t itemid, uint32_t* gid);
	int get_item_in_grid(userid_t userid, uint32_t *itemid, uint32_t gid);
	int get_flag2(userid_t userid, uint32_t* flag);
	int set_flag2(userid_t userid, uint32_t flag);
	const char* str_uid;
	const char* str_itemid;
	const char* str_gid;
	const char* str_cnt;
	const char* str_storagecnt;
public:
	Citem(mysql_interface * db ,Citem_change_log * p_log);
	int add_item(userid_t userid, uint32_t itemid, int32_t count ,uint32_t * p_left_count=NULL , bool is_vip_opt=false);

	int add_item_in_storage(userid_t userid, uint32_t itemid, int32_t count ,bool is_vip_opt=false);
	
	int get_item(userid_t userid, uint32_t itemid, uint32_t* p_cnt);
	int get_item_list(userid_t userid, stru_mole2_user_item_info** pp_out_item, uint32_t* p_out_count);

	int storage_list_get(userid_t userid, stru_mole2_user_item_info** pp_out_item, uint32_t* p_out_count);

	int get_item_list_ranged(userid_t userid, mole2_user_item_ranged_get_list_in* p_in, 
			mole2_user_item_ranged_get_list_out_item** pp_out_item, uint32_t* p_out_count);

	int get_all_count(userid_t	userid, uint32_t itemid, uint32_t* p_count, uint32_t* p_storage_cnt);

	int list_get(uint32_t userid, mole2_user_user_login_ex_out_item_5** pp_out, uint32_t* p_count);
	
	int get_items(uint32_t userid, std::vector<stru_item_info> &items);
};

#endif /* ----- #ifndef CPET_INC  ----- */
