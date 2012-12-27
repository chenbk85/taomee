/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_card.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2008年12月22日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  zheng, zheng@taomee.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CUSER_AUTO_INCL
#define  CUSER_AUTO_INCL
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include "Citem_change_log.h"


class Cuser_auto: public CtableRoute100x10 {
private:

	char *msglog_file;
	Citem_change_log *p_item_change_log;

	int get_count(userid_t userid, uint32_t *p_out);

	int get_attire(uint32_t id, userid_t userid, char *p_type, uint32_t *p_out);

	int get_propery(uint32_t id, userid_t userid, user_auto_propery *p_old);

	int update_propery(uint32_t id, userid_t userid, user_auto_propery *p_in);
public:
	Cuser_auto(mysql_interface *db, Citem_change_log *p_log);
	
	int insert(userid_t userid, user_auto_insert_in *p_in, uint32_t *p_id);

	int update_attire(uint32_t id, userid_t userid, uint32_t type, uint32_t new_type, uint32_t *p_out);

	int add(userid_t userid, user_auto_insert_in *p_in, uint32_t *p_id, uint32_t is_vip_opt_type = 0);

	int change_propery(uint32_t id, userid_t userid, uint32_t type,	uint32_t propery_id,
			uint32_t new_propery, uint32_t *p_old);

	int get_field(uint32_t id, userid_t userid, char *p_field, uint32_t *p_out);

	int update_field(uint32_t id, userid_t userid, char *p_field, uint32_t value);

	int get_oil_time(uint32_t id, userid_t userid, uint32_t *p_oil, uint32_t *p_time,
			         uint32_t *p_hour, uint32_t *p_auto);

	int update_oil_time(uint32_t id, userid_t userid, uint32_t oil, uint32_t time);

	int add_oil(uint32_t id, userid_t userid, uint32_t *p_xiaomee);

	int cal_oil(uint32_t id, userid_t userid, uint32_t auto_id, uint32_t *old_time, uint32_t *oil, uint32_t engine);

	int get_all_auto(userid_t userid, user_auto_get_all_out_item **pp_list, uint32_t *p_count);

	int get_one_auto_info(userid_t userid, uint32_t id, user_auto_get_one_info_out *p_list);

	int get_adron(userid_t userid, uint32_t id, stru_adron *p_list);

	int update_adron(uint32_t id, userid_t userid, stru_adron *p_in);

	int change_adron(uint32_t id, userid_t userid, uint32_t type,
			        uint32_t adron_id, uint32_t new_adron, uint32_t *p_old);

	int get_auto_class(userid_t userid, uint32_t auto_id, uint32_t *p_count);

	int check_id_exist(userid_t userid, uint32_t show_id, bool *exist);


	int get_auto_num_by_class(userid_t userid, user_get_attire_list_out_item **pp_list, uint32_t *p_count,
			        uint32_t startid, uint32_t endid);

	int del_auto(userid_t userid, uint32_t id);

	int update_web(userid_t userid, uint32_t id, uint32_t oil, uint32_t engine,
		           uint32_t color, uint32_t oil_time);

	int get_init_oil(uint32_t auto_id, uint32_t *p_total_oil);

	int add_attire_msglog(userid_t userid, uint32_t attireid, uint32_t count);

};

#endif   /* ----- #ifndef CUSER_CARD_INCL  ----- */

