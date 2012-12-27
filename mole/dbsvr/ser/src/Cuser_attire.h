/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_attire.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 13时47分01秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CUSER_ATTIRE_INCL
#define    CUSER_ATTIRE_INCL

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"
#include "serlib/user.hpp"
#include "Citem_change_log.h"
bool attire_id_in_pic_list(const uint32_t id);


class Cuser_attire:public CtableRoute
{
private:
	userid_t _userid;
	
	char *msglog_file;

	Citem_change_log *p_item_change_log;

public:
	Cuser_attire(mysql_interface * db, Citem_change_log * p_log); 
	void userid(const userid_t uid)
	{
		_userid=uid;
	}

	int get (userid_t userid, uint32_t attireid,
			uint32_t * usedcount,uint32_t * count, uint32_t *p_chest, uint32_t *p_mode);

	int get_count (userid_t userid, uint32_t attireid, uint32_t &count);

	int get_used_count(userid_t userid, uint32_t attireid,uint32_t &usedcount);
	int get_list_by_attireid_interval(userid_t userid, 
			uint32_t attireid_start, uint32_t attireid_end,uint8_t usedflag,
			uint32_t *count, attire_count ** list	);
	int insert(userid_t userid,uint32_t attireid, uint32_t  usedcount,uint32_t count  );
	int remove(userid_t userid, uint32_t attireid);

	int update( userid_t userid, uint32_t attireid,
			uint32_t  usedcount,uint32_t count  );
	int update_used_count_inc(userid_t userid, uint32_t attireid, uint32_t count);
	int update_used_count(userid_t userid, uint32_t attireid, uint32_t usedcount);
	int update_usedcount_count(userid_t userid, uint32_t attireid, uint32_t inc_cnt, uint32_t inc_used,
			uint32_t is_vip_opt_type = 0);
	int swaplist( userid_t userid,
			uint32_t oldcount,attire_count* p_oldlist,
			uint32_t newcount,attire_count_with_max * p_newlist, uint32_t is_vip_opt_type = 0);
	int check_list( userid_t userid,
			uint32_t oldcount,attire_count* p_oldlist,
			uint32_t newcount,attire_count_with_max * p_newlist,
			user_change_user_value_in *p_uci);
	int update_count( userid_t userid,uint32_t attireid,int addcount);

	int take_off(userid_t userid, uint32_t itemid, uint32_t cnt);
	int put_on(userid_t userid, uint32_t itemid, uint32_t cnt);
	int get_noused_count(userid_t userid, uint32_t itemid, uint32_t &count);

	int set_no_vip( userid_t userid );

	int update_common(userid_t userid,user_set_attire_in *item,
			user_set_attire_out *p_out, uint32_t is_vip_opt_type = 0);

	int update_max_limit(userid_t userid, user_set_attire_max_limit_in *item, user_set_attire_max_limit_out *p_out,
			uint32_t is_vip_opt_type = 0);
	/**
	 * @brief 对一个指定的列表，取得相应的计数
	 */
	int   get_list_by_idlist(userid_t userid, id_list * p_attireidlist, uint32_t *count, attire_count ** list);

	int  get_nousedcount (userid_t userid, uint32_t attireid,uint32_t * p_noused_count  );

	int put_chest(userid_t userid, uint32_t attireid);

	int get_chest(userid_t userid, uint32_t attireid);

	int get_chest_list(userid_t userid, uint32_t *p_count, user_attire_get_chest_list_out_item **pp_list);

	int get_chestcount(userid_t userid, uint32_t *p_chest_count);

	int put_attire_chest(userid_t userid, uint32_t attireid);

	int   get_list_ex(userid_t userid, uint32_t start, uint32_t count,
		uint32_t *p_count,  user_attire_get_list_ex_out_item ** pp_list   )	;

	int  get_all_list_by_attireid_interval(userid_t userid,
					uint32_t attireid_start, uint32_t attireid_end,uint8_t usedflag,
							uint32_t *count, attire_count ** list   );
	int update_ex_db(userid_t userid, uint32_t attireid,uint32_t  usedcount,
		uint32_t chestcount , uint32_t  count  );

	int  update_ex( userid_t userid,uint32_t attireid,
			uint32_t  usedcount,uint32_t chestcount ,uint32_t count, uint32_t is_vip_opt_type = 0);

	int get_discontinuous_attire(userid_t userid, user_attire_get_some_attire_count_in_item *p_in, uint32_t count,
		user_attire_get_some_attire_count_out_item **pp_out, uint32_t *p_out_count);

	int set_used_attire(userid_t userid, user_attire_set_used_in_item *p_in, uint32_t count);

	int update_set_all_noused(userid_t userid);

	int set_mode_attire(userid_t userid, user_attire_set_mode_attire_in_item *p_in, uint32_t count, uint32_t index);

	int update_set_mode_noused(userid_t userid, uint32_t index);

	int get_mode_clothe(userid_t userid, user_attire_mode_clothe	**pp_list, uint32_t *p_count);

	int get_mode_attire(userid_t userid, user_attire_all_mode_clothe *p_out);

	int update_set_mode_mole(userid_t userid, uint32_t index);

	int get_one_mode_clothe(userid_t userid, uint32_t index,
		user_attire_set_mode_to_mole_out_item **pp_list, uint32_t *p_count);

	int update_set_clothe_mole_mode(userid_t userid, uint32_t index);

	int get_discontinuous_attire_except_chest(userid_t userid, user_attire_get_attire_except_chest_in_item *p_in,
	            	uint32_t count, user_attire_get_attire_except_chest_out_item **pp_out, uint32_t *p_out_count);


	int get_mole_clothe(userid_t userid, user_attire_set_used_out_item **pp_list, uint32_t *p_count);

	int  get_list_by_attireid_for_bus(userid_t userid, uint32_t attireid_start, 
			uint32_t attireid_end, uint32_t *count, uint32_t** list); 

	int delete_attirelist(userid_t userid, uint32_t start_index, uint32_t end_index, uint32_t is_vip_opt_type = 0);
	
	int reset_usedcount(userid_t userid, uint32_t start_attireid, uint32_t end_attireid);
	
	int add_attire_msglog(userid_t, uint32_t attireid, uint32_t count);
	
	int get_paradise_attire(userid_t userid, user_get_paradise_attirelist_out_item **p_out_item, 
			uint32_t *count, uint32_t start_id, uint32_t end_id);
	int get_paradise_noused_attirelist(userid_t userid, user_get_paradise_attirelist_out_item **pp_list,
        uint32_t *count);
	int get_expedition_articles(userid_t userid, user_get_expedition_bag_out_item **pp_list, uint32_t *p_count,
			uint32_t start_id, uint32_t end_id, uint32_t flag);
	int update_new_treasue_age(userid_t userid, uint32_t attireid, uint32_t used_count, uint32_t count);
	int update_assign_count(userid_t userid, uint32_t attireid, uint32_t used_count, uint32_t count,
			uint32_t is_vip_opt_type = 0);
	int update_inc_count_and_used(userid_t userid, uint32_t attireid, uint32_t inc_cnt, uint32_t inc_used);
	/**
	 * @brief 把数据读取指定物品列表的物品个数信息
	 * @param buy_map 列表从其中读取
	 * @param have_map 最后生成的包含个数信息的列表
	 */
	int get_count_by_attire_list(const db_utility::UserAttireBuyMap& buy_map, db_utility::UserAttireCountMap& have_map)
	{
		id_list alist;
		alist.count=std::min((size_t)ID_LIST_MAX, buy_map.size());
		to_attire_list(buy_map, ID_LIST_MAX, alist.item);
		attire_count* attire_list;
		uint32_t count=0;
		int ret=get_list_by_idlist(_userid, &alist, &count, &attire_list);
		if(SUCC!=ret)
		{
			return ret;
		}
		to_count_map(count, attire_list, have_map);
		free(attire_list);
		return SUCC;
	}

	/**
	 * @brief 批量更新物品信息，或者插入，或者设置计数
	 * @param buy_map 需要更新的物品信息列表
	 */
	int update_db_by_buy_map(const db_utility::UserAttireBuyMap& buy_map, uint32_t is_vip_opt_type = 0)
	{
		db_utility::UserAttireBuyMap::const_iterator itr=buy_map.begin();
		for(; itr!=buy_map.end(); ++itr)
		{
			int ret;
			const db_utility::UserAttireBuyUnit& u=itr->second;
			if(u.exist)
			{
				ret=update_count(_userid,itr->first,u.count);
				if(SUCC!=ret)
				{
					return ret;
				}
				else{
					uint32_t itemid = itr->first;
					if(itemid == 190654){
						DEBUG_LOG("add== itemid: %u, count: %u", itemid, u.count);
					}
					this->p_item_change_log->add(time(NULL), is_vip_opt_type, this->_userid, itemid, u.count);	
				}
			} else {
				ret=insert(_userid,itr->first, 0,u.count);
				if(SUCC!=ret)
				{
					return ret;
				}
				else{
					if(itr->first == 190654){
						DEBUG_LOG("add== itemid: %u, count: %u", itr->first, u.count);
					}
					this->p_item_change_log->add(time(NULL), is_vip_opt_type, this->_userid, itr->first, u.count);	
				}
			}
		}

		return SUCC;
	}

	/**
	 * @brief 面向具体命令的函数，执行命令逻辑，批量更新物品信息
	 * @param buy_map 需要更新的物品信息列表
	 */
	int user_buy_many_thing_in_one_time(db_utility::UserAttireBuyMap& buy_map)
	{
		db_utility::UserAttireCountMap  have_map;
		int ret=get_count_by_attire_list(buy_map, have_map);
		if(SUCC!=ret)
		{
			return ret;
		}

		if(!completeness_ckeck(buy_map, have_map))
		{
			return BUY_MANY_THING_IN_ONE_TIME_NOT_COMPLETENESS_ERR;
		}

		return update_db_by_buy_map(buy_map);
	}

};

#endif   /* ----- #ifndef CUSER_ATTIRE_INCL  ----- */
