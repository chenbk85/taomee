
/*
 * =====================================================================================
 * 
 *       Filename:  Cuser.h
 * 
 *    Description:  
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

#ifndef  CUSER_INC
#define  CUSER_INC
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include "func_comm.h"
#include "Csend_log.h"
#include "serlib/user.hpp"
#include <map>
#include "Citem_change_log.h"
#include "Cuser_bank.h"

typedef std::map<uint32_t,uint32_t> ATTIRE_MAPITEM;

//用户活动 1:走路
#define  USER_TEMP_USER_FLAG    	  1


#define  USER_FLAG_ENABLED    		0x00000001


//VIP...
//封号
#define  USER_FLAG_UNUSED			0x04000000
//离线24小时
#define  USER_FLAG_OFFLINE24 		0x02000000
//

//封号7天
#define USER_FLAG_UNUSED_SEVEN_DAY 0X01000000

//封号14天
#define USER_FLAG_UNUSED_FOURTEEN_DAY 0X00800000

#define  COMM_FLAG_STR    	  		"flag"
#define  VIP_FLAG_STR    	  		"vip"
#define  FRIEND_LIST_STR 	  		"friendlist"
#define  BLACKUSER_LIST_STR 		"blacklist"

#define  HOME_STR 		"home"
#define  JY_STR 		"jy"

class Cuser:public CtableRoute100x10
{
 
  private:
	Cuser_bank mole_bank;
  	userid_t _userid; ///< 对象的内部暂时存储的当前用户ID
	uint32_t xiamee_a_day_max;
	uint32_t sale_xiamee_a_day_max;
	uint32_t log_xiaomee_flag;
	Csend_log *p_send_log;
	
	Citem_change_log *p_item_change_log;

	char msglog_file[200];

	inline int get_flag (userid_t userid, const char *flag_type, uint32_t * p_flag);
	//此update_xiaomee函数禁止别处使用
	int update_xiaomee(userid_t userid, uint32_t xiaomee);
	int cal_total_xiaomee(userid_t userid, int *change_value, int *p_xiaomee);
	//返回实际删除的个数
	inline int del_homeattire_noused (noused_homeattirelist * p_list, attire_noused_item * p_item);

	//返回实际删除的个数
	inline int del_homeattire_used (home_attirelist * p_list, home_attire_item * p_item);

	int update_used_homeattirelist (const char *type_str, userid_t userid, home_attirelist * p_usedlist);
  public:
	  Cuser (mysql_interface * db, Csend_log * p_send_log,  Citem_change_log * p_log);
	int update_flag (userid_t userid, const char *flag_type, uint32_t flag);
	void userid(const userid_t uid)
	{
		_userid=uid;
	}

	int insert (userid_t userid, user_without_id * p_in);
	int del (userid_t userid);
	int check_existed (userid_t userid);
	int set_flag (userid_t userid, const char *flag_type, uint32_t flag_bit, bool is_true);
	int set_vipflag (userid_t userid, bool isvip);

	int update_onlinetime (userid_t userid, time_t onlinetime, uint32_t lastip);
	int user_check (userid_t userid);

	//attire list
	int get_attirelist (const char *type_str, userid_t userid, home_attirelist * p_home_attirelist);
	int swap_homeattire (const char *type_str, userid_t userid,
						 uint32_t oldcount, attire_count * p_oldlist,
						 uint32_t newcount, attire_count_with_max * p_newlist);
	//
	//msg lits
	inline int get_msglist (userid_t userid, stru_msg_list * msglist);
	inline int update_msglist (userid_t userid, stru_msg_list * msglist);
	int add_offline_msg (userid_t userid, msg_item * item);
	int add_home_attire (const char *type_str, userid_t userid, attire_count_with_max * p_item, uint32_t is_vip_opt_type = 0);


	int update_homeattire_all (const char *type_str, userid_t userid,
							   home_attirelist * p_usedlist, noused_homeattirelist * p_nousedlist);

	int update_noused_homeattirelist(const char *type_str, userid_t userid, noused_homeattirelist * p_list);
	int get_noused_home_attirelist (const char *type_str, userid_t userid, noused_homeattirelist * p_list);

	/**
	 * @brief 从数据库抽取两个列表，使用列表和未使用列表
	 * @param const char* type_str 指定是家园或小屋列表的两个类型 @ref HOME_STR JY_STR
	 * @param userid_t userid 用户米米号
	 * @param home_attirelist* p_usedlist 已经使用列表，用于返回
	 * @param noused_homeattirelist* p_nousedlist 未使用列表，用于返回
	 * @return 数据错误码或成功，或者用户定义错误码
	 */
	int get_homeattirelist (const char *type_str, userid_t userid,
							home_attirelist * p_usedlist, noused_homeattirelist * p_nousedlist);


	int update_homeattirelist (const char *type_str, userid_t userid,
							   home_attirelist * p_usedlist, noused_homeattirelist * p_nousedlist);

	void get_home_attire_total (ATTIRE_MAPITEM * p_itemmap,
								home_attirelist * p_usedlist, noused_homeattirelist * p_nousedlist);


	int set_tasklist (userid_t userid, stru_tasklist * p_tasklist);
	int get_user (userid_t userid, get_user_out * p_out);
	int get_user_ex (userid_t userid, get_user_out * p_out);
	int get_mole_color(userid_t userid, uint32_t &color);
	int update_petcolor (userid_t userid, user_set_petcolor_in * p_in);
	int get_user_all (userid_t userid, user_get_user_all_out * p_out);
	int change_user_value (userid_t userid, user_change_user_value_in * in, user_change_user_value_out * out);
	int del_home_attire_noused (const char *type_str, userid_t userid, attire_count_with_max * p_item,
			uint32_t is_vip_opt_type = 0);

	int get_attire_count (const char *type_str, userid_t userid, uint32_t attireid,
						  uint32_t count_flag, uint32_t * p_count);

	int change_xiaomee (userid_t userid, int change_value, uint32_t reason, uint32_t reason_ex, int *p_xiaomee);
	int home_set_attire_noused (const char *type_str, userid_t userid, uint32_t attireid);



	int get_flags (userid_t userid, user_get_flag_out * p_out);
	int home_attire_change (const char *type_str, userid_t userid, user_home_attire_change_in * p_in);

	int del_home_attire_used (const char *type_str, userid_t userid, uint32_t attireid);

	int update_used_homeattirelist_with_check (const char *type_str, userid_t userid, home_attirelist * p_new_usedlist);
	int set_birthday (userid_t userid, uint32_t birthday);
	int get_birtyday (userid_t userid, uint32_t * p_birthday);
	int isset_birthday (userid_t userid, stru_is_seted * p_out);
	int get_tmp_value (userid_t userid, uint32_t * p_flag, uint32_t * p_value);
	int set_tmp_value (userid_t userid, uint32_t flag, uint32_t value);
	int add_tmp_value (userid_t userid, uint32_t flag, uint32_t * p_value);
	int get_tasklist (userid_t userid, stru_tasklist * p_tasklist);
	int set_task_value (userid_t userid, user_set_task_in * p_in);
	int get_hometypeid (userid_t userid, uint32_t * p_hometypeid);
	int get_history_ip (userid_t userid, history_ip * p_history_ip);
	int update_history_ip (userid_t userid, history_ip * p_history_ip);
	int set_history_ip (userid_t userid, uint32_t lastip);

	int get_xiaomee (userid_t userid, uint32_t * p_xiaomee);
	int add_game (userid_t userid, uint32_t score);
	int update_work (userid_t userid, uint32_t work);
	int set_xiaomee_max_info (userid_t userid, user_set_xiaomee_max_info_in * in);

	int reset_noused_homeattirelist (const char *type_str, userid_t userid, noused_homeattirelist * p_nousedlist);


//-----------------------------------------------------------------------------------------
	int home_get_noused_attirelist (userid_t userid, stru_noused_attirelist * p_list);
	int home_get_used_attirelist (userid_t userid, uint32_t homeid, stru_home_used_attirelist * p_usedlist);

	/**
	 * @brief 一次抓取小屋全部物品列表
	 * @param userid_t userid 指定用户
	 * @param stru_noused_attirelist* plist 未使用列表
	 * @param stru_home_used_attirelist * p_usedlist_1 已使用列表1, 存在多个这样的列表
	 * @param stru_home_used_attirelist * p_usedlist_2 已使用列表2, 存在多个这样的列表
	 * @param stru_home_used_attirelist * p_usedlist_3 已使用列表3, 存在多个这样的列表
	 * @return 数据操作错误码或自定义逻辑错误
	 */
	int home_get_all(userid_t userid, stru_noused_attirelist * p_list,
					  stru_home_used_attirelist * p_usedlist_1,
					  stru_home_used_attirelist * p_usedlist_2,
					  stru_home_used_attirelist * p_usedlist_3);

	int home_update_noused_attirelist (userid_t userid, stru_noused_attirelist * p_list);


	int home_add_attire (userid_t userid, attire_count_with_max * p_item, bool is_vip, uint32_t is_vip_opt_type = 0);
	int home_reduce_attire (userid_t userid, attire_count_with_max * p_item, uint32_t is_vip_opt_type = 0);

	//小屋编辑
	int home_edit (userid_t userid, uint32_t homeid, stru_home_used_attirelist * p_usedlist);

	int home_get_edit_list (userid_t userid, uint32_t homeid,
							stru_noused_attirelist * p_noused_list, stru_home_used_attirelist * p_usedlist);

	int home_edit_update (userid_t userid, uint32_t homeid,
						  stru_noused_attirelist * p_noused_list, stru_home_used_attirelist * p_used_list);


	int home_update_used_list (userid_t userid, uint32_t homeid, stru_home_used_attirelist * p_used_list);
	int home_reduce_used_attire (userid_t userid, uint32_t homeid, uint32_t attireid);
	int home_get_used_list (userid_t userid, uint32_t homeid, stru_home_used_attirelist * p_usedlist);
	int home_edit_reset_all_update (userid_t userid,
									stru_noused_attirelist * p_noused_list, stru_home_used_attirelist * p_used_list);
	int home_get_attire_count (userid_t userid, uint32_t attireid, uint32_t count_flag, uint32_t * p_count);

	int home_edit_reset_all (userid_t userid,
							 stru_home_used_attirelist * p_new_usedlist,
							 stru_noused_attirelist * p_out_noused_attireidlist);

	int home_update_used_attirelist_with_check (userid_t userid, uint32_t homeid,
												stru_home_used_attirelist * p_new_usedlist);

	int home_swap_attirelist (userid_t userid,
							  uint32_t oldcount, attire_count * oldlist,
							  uint32_t newcount, attire_count_with_max * newlist);


	int home_get (userid_t userid, uint32_t homeid,
				  stru_home_used_attirelist * p_usedlist_1, stru_home_used_attirelist * p_usedlist);

	int home_reduce_used_attire (userid_t userid, uint32_t attireid);

	int check_lock_time (userid_t userid);

	int home_get_mode_index (userid_t userid, uint32_t * p_mode_index);
	int update_last_onlinetime (userid_t userid, uint32_t ol_today, uint32_t Ol_last);

	/**
	 * @brief 设置用户在新年照片里的服装列表
	 * @param const userid_t userid 哪个用户
	 * @param const DressingUnit& dress 照片里的服装
	 * @return 数据操作错误码，或者0
	 */
	int home_dress_in_photo_get(const userid_t userid, db_utility::DressingUnit& dress);

	/**
	 * @breif 取得新年照片里的服装
	 * @param const userid_t userid 用户米米号
	 * @param DressingUnit& dress 用户相片里的服装
	 * @return 数据操作错误码，或者0
	 */
	int home_dress_in_photo_set(const userid_t userid, const db_utility::DressingUnit& dress);

	/**
	 * @brief 把数据读取指定物品列表的物品个数信息
	 * @param buy_map 列表从其中读取
	 * @param have_map 最后生成的包含个数信息的列表
	 */
	int home_get_count_by_attire_list(const db_utility::UserAttireBuyMap& buy_map, db_utility::UserAttireCountMap& have_map)
	{
		stru_noused_attirelist noused_list;
		stru_home_used_attirelist used_list_1;
		stru_home_used_attirelist used_list_2;
		stru_home_used_attirelist used_list_3;
		int ret=home_get_all(_userid, &noused_list, &used_list_1, &used_list_2, &used_list_3);
		if(SUCC!=ret)
		{
			return ret;
		}

		db_utility::UserAttireBuyMap::const_iterator itr=buy_map.begin();
		for(; itr!=buy_map.end(); ++itr)
		{
			uint32_t aid=itr->first;
			size_t aid_num=noused_list.get_attire_count(aid)
					+ used_list_1.get_attire_count(aid)
					+ used_list_1.get_attire_count(aid)
					+ used_list_1.get_attire_count(aid);
			if(0!=aid_num)
			{
				have_map[aid].count = aid_num;
			}
		}

		return SUCC;
	}

	/**
	 * @brief 批量更新物品信息，或者插入，或者设置计数
	 * @param buy_map 需要更新的物品信息列表
	 * @param type_str 通过表中列名的前缀来确定的选出内容
	 * @see HOME_STR JY_STR
	 */
	int update_db_by_buy_map_with_type_str(const db_utility::UserAttireBuyMap& buy_map, const char* type_str,
			uint32_t is_vip_opt_type = 0)
	{
		noused_homeattirelist noused_list;
		int ret=get_noused_home_attirelist(type_str, _userid, &noused_list);
		if(SUCC!=ret)
		{
			return ret;
		}

		db_utility::UserAttireBuyMap::const_iterator itr=buy_map.begin();
		for(; itr!=buy_map.end(); ++itr)
		{
			if(noused_list.count < HOME_NOUSE_ATTIRE_ITEM_MAX){
				this->p_item_change_log->add(time(NULL), is_vip_opt_type, this->_userid, itr->first, itr->second.count);
			}
			noused_list.add_attire_count(itr->first, itr->second.count);
		}

		return update_noused_homeattirelist(type_str, _userid, &noused_list);
	}

	/**
	 * @brief 面向具体命令的函数，执行命令逻辑，批量更新物品信息
	 * @param buy_map 需要更新的物品信息列表
	 */
	int user_home_buy_many_thing_in_one_time(db_utility::UserAttireBuyMap& buy_map)
	{
		db_utility::UserAttireCountMap  have_map;
		int ret=home_get_count_by_attire_list(buy_map, have_map);
		if(SUCC!=ret)
		{
			return ret;
		}

		if(!completeness_ckeck(buy_map, have_map))
		{
			return BUY_MANY_THING_IN_ONE_TIME_NOT_COMPLETENESS_ERR;
		}

		return update_db_by_buy_map_with_type_str(buy_map, (const char*)HOME_STR);
	}

	/**
	 * @brief 把数据读取指定物品列表的物品个数信息
	 * @param buy_map 列表从其中读取
	 * @param have_map 最后生成的包含个数信息的列表
	 */
	int jiayuan_get_count_by_attire_list(const db_utility::UserAttireBuyMap& buy_map, db_utility::UserAttireCountMap& have_map)
	{
		home_attirelist used_list;
		noused_homeattirelist noused_list;
		int ret=get_homeattirelist (JY_STR, _userid, &used_list, &noused_list);
		if(SUCC!=ret)
		{
			return ret;
		}

		db_utility::UserAttireBuyMap::const_iterator itr=buy_map.begin();
		for(; itr!=buy_map.end(); ++itr)
		{
			uint32_t aid=itr->first;
			size_t aid_num=noused_list.get_attire_count(aid) + used_list.get_attire_count(aid);
			if(0!=aid_num)
			{
				have_map[aid].count = aid_num;
			}
		}

		return SUCC;
	}

	/**
	 * @brief 面向具体命令的函数，执行命令逻辑，批量更新物品信息
	 * @param buy_map 需要更新的物品信息列表
	 */
	int user_jiayuan_buy_many_thing_in_one_time(db_utility::UserAttireBuyMap& buy_map)
	{
		db_utility::UserAttireCountMap  have_map;
		int ret=jiayuan_get_count_by_attire_list(buy_map, have_map);
		if(SUCC!=ret)
		{
			return ret;
		}

		if(!completeness_ckeck(buy_map, have_map))
		{
			return BUY_MANY_THING_IN_ONE_TIME_NOT_COMPLETENESS_ERR;
		}

		return update_db_by_buy_map_with_type_str(buy_map, (const char*)JY_STR);
	}

	int get_noah_info(userid_t userid, user_get_noah_info_out& out);

	int add_attire_msglog(userid_t userid, uint32_t attireid, uint32_t count);

	int update_xiaomee_inc(userid_t userid, int32_t xiaomee_inc);
	int update_noah_info_inc(userid_t userid, user_noah_inc_user_info_in* p_in);
	int get_field_uint32(userid_t userid, const char *field, uint32_t &value);
	int get_user_board_info(userid_t userid, user_get_user_board_info_out_header * p_out );
	int get_user_exp(userid_t userid, uint32_t& exp);
	int get_user_exp_color(userid_t userid, uint32_t& exp, uint32_t &color);
	int update_exp_inc(userid_t userid, uint32_t exp_inc, uint32_t strong_inc);

	int get_home_attire_count(userid_t userid, bool is_vip, user_get_home_attire_count_out* p_out);
	int get_teamid(userid_t userid,uint32_t* data);
	int get_one_col(userid_t userid, const char* col, uint32_t *value);
	int update_one_col(userid_t userid, const char* col, uint32_t value);
};


#endif /* ----- #ifndef CUSER_INC  ----- */
