/*
 * =====================================================================================
 * 
 *       Filename:  Ccup.h
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

#ifndef  CROOMINFO_INCL
#define  CROOMINFO_INCL
#include <algorithm>
#include <cstring>
#include "CtableRoute10x10.h"
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"
#include "serlib/user.hpp"
#include "serlib/db_utility.hpp"

#define  STR_ROOM_PUGLIST "room_puglist"

typedef roominfo_roominfo_dressing_mark_get_out MarkUnit;

struct DressingMarkUnit
{
	db_utility::DressingUnit dress;
	MarkUnit mark;
}__attribute__((packed));

struct DressingMarkRecord
{
	uint32_t next_index;
	DressingMarkUnit record[8];
}__attribute__((packed));

inline const DressingMarkUnit* find(const DressingMarkUnit* begin, const DressingMarkUnit* end, const db_utility::DressingUnit& v)
{
	for(; begin!=end; ++begin)
	{
		if(begin->dress==v)
		{
			return begin;
		}
	}

	return end;
}

class Croominfo : public CtableRoute10x10
{
public:

	void userid(const userid_t uid)
	{
		_userid=uid;
	}

	DB_MACRO_define_value_type(fire_halt_height_t,  fire_halt_height,  uint32_t)
	DB_MACRO_define_value_type(water_halt_height_t, water_halt_height, uint32_t)
	DB_MACRO_define_value_type(wood_halt_height_t,  wood_halt_height,  uint32_t)

	DB_MACRO_define_template_update_1_by_userid(room_id, _userid, ID_NOT_EXIST_ERR)
	DB_MACRO_define_template_update_2_by_userid(room_id, _userid, ID_NOT_EXIST_ERR)

	DB_MACRO_define_template_select_1_by_userid(room_id, _userid, ID_NOT_EXIST_ERR)
	DB_MACRO_define_template_select_2_by_userid(room_id, _userid, ID_NOT_EXIST_ERR)
	DB_MACRO_define_template_select_3_by_userid(room_id, _userid, ID_NOT_EXIST_ERR)

private:
	userid_t _userid;

public:
	Croominfo(mysql_interface * db );
	int insert(userid_t userid);
	int get_hot(userid_t userid, roominfo_get_hot_out * p_out);
	int get_hot_db(userid_t userid, roominfo_get_hot_out * p_out);
	int vote_room(userid_t userid, roominfo_vote_room_in *p_in);
	int get_value(userid_t userid, char * filed_name ,uint32_t  * p_value);
	int add_value(userid_t userid, char * filed_name );
	int get_vote_list(userid_t userid,const char * field_str, roominfo_pug_list_out *p_out );
	int add_vote(userid_t userid,const char * field_str , rooninfo_pug_item *p_item );
	int update_votelist(userid_t userid , const char * field_str,  roominfo_pug_list_out * p_list );
	int init_mud(userid_t userid);
	int del_mud(userid_t userid);
	int set_value(userid_t userid,roominfo_set_info_in *p_in );
	int add_hot(userid_t userid );
	int get_work_time(userid_t userid, uint32_t *p_out);
	int update_work_time( userid_t userid);   
	int get_total_time(userid_t userid, uint32_t *p_out);
	int add_field(userid_t userid, const char *field, uint32_t add, uint32_t *p_out);
	int get_score_candy(userid_t userid, roominfo_get_score_candy_out * p_out);
	int add_score(userid_t userid, uint32_t add_score);
	int update_score( userid_t userid, uint32_t add_score);
	int get_merge(userid_t userid, roominfo_get_merge_buf *p_out);
	int insert_merge(userid_t userid, roominfo_get_merge_buf *p_list);
	int update_merge(userid_t userid, roominfo_get_merge_buf *p_list); 
	int check_candy(userid_t userid, uint32_t *p_flag);
	int change_candy_prop(userid_t userid, uint32_t flag, uint32_t candy, uint32_t pro, uint32_t *p_flag, uint32_t *p_prop);
	int update_user_info(userid_t userid, roominfo_set_user_all_info_web_in *p_in);
	int update_candy_prop( userid_t userid, uint32_t candy_count, uint32_t prop_count);
	int get_candy_score(userid_t userid, roominfo_get_user_all_info_web_out *p_out);
	int get_candy_list(userid_t userid, roominfo_candy_list *p_out);
	int check_candy_list(userid_t userid, userid_t get_userid);
	int get_candy_prop(userid_t userid, uint32_t *p_candy, uint32_t *p_prop);
	int update_candy_list(userid_t userid, roominfo_candy_list *p_list);
	int insert_candy_list(userid_t userid, roominfo_candy_list *p_list);
	int dressing_mark_get(const userid_t user_id, const db_utility::DressingUnit& dress, roominfo_roominfo_dressing_mark_get_out& out);
	int dressing_mark_set(const userid_t user_id, const DressingMarkUnit& unit);
	int duck_statu_get(const userid_t user_id, uint32_t& statu);
	int duck_statu_set(const userid_t user_id, uint32_t statu);
	int get_socks(userid_t useid, userid_t other_userid);
	int update_wish_sql(userid_t userid, stru_roominfo_wish *p_list);
	int select_wish_sql(userid_t user_id, stru_roominfo_wish *p_out);
	int select_gift_sql(userid_t user_id, roominfo_give_gift_userid *p_out);
	int update_gift_sql(userid_t userid, roominfo_give_gift_userid *p_list); 
	int add_gift_userid(userid_t userid, roominfo_record_give_gift_userid_in *p_list);
	int del_gift_userid(userid_t userid, roominfo_get_give_gift_userid_out *p_out);
	int update_sql(userid_t userid, const char *p_field, void *p_list, size_t len); 
	int select_sql(userid_t user_id, const char *p_field, void *p_out, size_t len);
	int update_two_col_sql(userid_t userid, const char *p_col1, const char *p_p_col2, uint32_t col1_value, uint32_t col2_value);
	int select_two_col_sql(userid_t userid, const char *p_col1, const char *p_col2, uint32_t *p_col1_value, uint32_t *p_col2_value);
	int add_energy_star(userid_t userid, uint32_t add_value, uint32_t vip_flag, roominfo_add_energy_star_out *p_out);
	int get_scores(userid_t userid, uint32_t& score, uint32_t& today_score);
//	int reward_num_select(userid_t userid, uint32_t *p_reward_num);
//	int reward_num_update(userid_t userid, int reward_num);

	int get_hotlist_date(userid_t userid, uint32_t &date, roominfo_hot_list *visits);
	int update_hotlist(userid_t userid, roominfo_hot_list *visits);
private:
	int dressing_mark_get_all(const userid_t user_id, DressingMarkRecord& out);
	int dressing_mark_set_all(const userid_t user_id, const DressingMarkRecord& indata);
	int select_socks_sql(userid_t user_id, stru_roominfo_socks *p_out);
	int update_socks_sql(userid_t userid, stru_roominfo_socks *p_list);

};


#endif   /* ----- #ifndef CROOMINFO_INCL  ----- */

