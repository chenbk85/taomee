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

#ifndef  CUSER_FARM_EX_INC
#define  CUSER_FARM_EX_INC
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include <map> 
#include "Citem_change_log.h"

using namespace std;
typedef map<uint32_t , uint32_t>  ATTIRE_MAPITEM;  





#define  FARM_STR 		"farm" 
#define  FEEDSTUFF_STR 		"feedstuff" 

class Cuser_farm :public CtableRoute100x10
{
private:

	char *msglog_file;
	Citem_change_log *p_item_change_log;


	//返回实际删除的个数
	inline int del_homeattire_noused(noused_homeattirelist *p_list, attire_noused_item *p_item ); 

	//返回实际删除的个数
	inline int del_homeattire_used(home_attirelist *p_list , home_attire_item * p_item );
	inline int add_homeattire_noused(noused_homeattirelist *p_list, attire_count_with_max * p_item);

	int update_used_homeattirelist(const char * type_str, userid_t userid , home_attirelist * p_usedlist);
public:
	Cuser_farm(mysql_interface * db, Citem_change_log * p_log); 

	int insert(userid_t userid);
	int del(userid_t userid );
	int	check_existed( userid_t userid);

	int user_check(userid_t userid);

	//attire list
	int get_attirelist( const char * type_str,userid_t userid,home_attirelist * p_home_attirelist);
	int swap_homeattire(const char * type_str, userid_t userid , 
	uint32_t oldcount,attire_count* p_oldlist,
	uint32_t newcount,attire_count_with_max * p_newlist );
	//
	//msg lits
	int add_home_attire(const char * type_str, userid_t userid,attire_count_with_max * p_item, uint32_t is_vip_opt_type = 0);


	int update_homeattire_all(const char * type_str, userid_t userid, 
	home_attirelist * p_usedlist,noused_homeattirelist * p_nousedlist );

	int update_noused_homeattirelist( const char * type_str,userid_t userid, noused_homeattirelist * p_list );
	int get_noused_home_attirelist(const char * type_str, userid_t userid, noused_homeattirelist * p_list );

	int  get_homeattirelist(const char * type_str, userid_t userid, home_attirelist * p_usedlist, 
	noused_homeattirelist* p_nousedlist);

	int update_homeattirelist(const char * type_str, userid_t userid,  
	home_attirelist * p_usedlist,noused_homeattirelist * p_nousedlist );

	void get_home_attire_total(ATTIRE_MAPITEM  * p_itemmap,
	home_attirelist * p_usedlist,
	noused_homeattirelist * p_nousedlist );


	int get(userid_t userid, user_get_farm_out_header *p_out,
	farm_noused_attirelist *p_noused, farm_used_attirelist *p_uused);

	int change_user_value(userid_t userid,  user_change_user_value_in *in , 
	user_change_user_value_out *out );
	int del_home_attire_noused(const char * type_str, userid_t userid , attire_count_with_max  * p_item,
			uint32_t is_vip_opt_type = 0);

	int get_attire_count(const char * type_str, userid_t userid, uint32_t  attireid,
	uint32_t count_flag,  uint32_t * p_count );

	int home_set_attire_noused(const char * type_str, userid_t userid , uint32_t attireid);



	int home_attire_change(const char * type_str, userid_t userid , user_home_attire_change_in *p_in );

	int del_home_attire_used(const char * type_str, userid_t userid , uint32_t attireid );

	int update_used_homeattirelist_with_check(const char * type_str, userid_t userid ,   
	home_attirelist * p_new_usedlist );
	int get_hometypeid(userid_t userid ,uint32_t *p_hometypeid);
	int add_feedstuff(attire_count_with_max *p_list, userid_t userid, noused_homeattirelist *p_noused);
	int set_water_time(userid_t userid);
	int set_water_time(userid_t userid, uint32_t end_time);
	int get_jyaccess_list(userid_t userid, access_jylist *p_out);
	int access_jy (userid_t userid, user_access_jy_in *p_in);
	int jy_accesslist_set_opt (userid_t userid, uint32_t opt_userid ,uint32_t opt_type );
	int update_jy_accesslist( userid_t userid, access_jylist *p_list );
	int get_user_farm_table_info(userid_t userid, user_get_farm_out_header *p_out_header,
	farm_noused_attirelist *p_noused, farm_used_attirelist *p_used, uint32_t &water_time);
	int update_state(userid_t userid, uint32_t state);
	int get_thiever_info(userid_t userid, stru_thiever *p_out);
	int update_thiever(userid_t userid, stru_thiever *p_list);
	int thiever_animal(userid_t userid, userid_t fish_userid, uint32_t id, uint32_t type);
	int get_state(userid_t userid, uint32_t *p_state);
	int day_between(const time_t pre_time);
	int time_limit(userid_t userid, uint32_t *p_time, uint32_t *p_plant_limit, 
	uint32_t *p_breed_limit, uint32_t *p_plant, uint32_t *p_breed);
	int update_time_limit(userid_t userid, uint32_t plant, uint32_t breed, uint32_t time,
				uint32_t plant_limit, uint32_t breed_limit);
	int update_plant_breed(userid_t userid, uint32_t plant, uint32_t breed);
	int add_other(userid_t userid, uint32_t add, uint32_t type, int32_t *p_new);
	int get_plant_breed(userid_t userid, uint32_t *p_plant, uint32_t *p_breed);
	int get_farm_web(userid_t userid, user_farm_get_web_out *p_out);
	int set_farm_web(userid_t userid, user_farm_set_web_in *p_in); 
	int get_rabbit_list(userid_t userid, rabbit_foster_list *p_out);
	int update_rabbit_list(userid_t userid, rabbit_foster_list *p_list);
	int foster_rabbit(userid_t userid, uint32_t id);
	int del_foster_rabbit(userid_t userid, uint32_t *p_array, uint32_t *p_count);
	int catch_get_exp(userid_t userid, uint32_t type, uint32_t add, uint32_t *p_ex);
	int update_col(userid_t userid, uint32_t type, uint32_t value);
	int update_count(userid_t userid, uint32_t value);
	int get_count(userid_t userid, uint32_t &count);
	int add_attire_msglog(userid_t userid, uint32_t attireid, uint32_t count);

};


#endif   /* ----- #ifndef CUSER_INC  ----- */

