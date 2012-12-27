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

#ifndef  CUSER_EGG_INCL
#define  CUSER_EGG_INCL
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"


class Cuser_egg : public CtableRoute100x10 {
public:
	Cuser_egg(mysql_interface * db); 

	int insert(userid_t userid, uint32_t pos, uint32_t attireid); 
	
	int get_count(userid_t userid, uint32_t pos, uint32_t *p_out);

	int insert_record(userid_t userid, uint32_t pos, uint32_t attireid);
	
	int get_all_record(uint32_t userid, stru_user_egg_ex **pp_list, uint32_t *p_count);

	int get_one_info(uint32_t userid, uint32_t pos, stru_user_egg_all *p_list);

	int add_num(userid_t userid, uint32_t userid_other, uint32_t pos, uint32_t *p_attireid);

	int update_egg(userid_t userid, stru_user_egg_all *p_list);

	int del(userid_t userid, uint32_t pos);

	int get_child(uint32_t attireid, uint32_t *p_out);

	int get_brood_num(uint32_t attireid, uint32_t *p_out);

	int get_all_record_web(uint32_t userid, user_egg_get_all_web_out_item **pp_list, uint32_t *p_count);

	int update_egg_web(userid_t userid, user_egg_update_web_in *p_list);

	int check_id(uint32_t attireid, uint32_t *p_out);

	int get_one_egg_count(userid_t userid, uint32_t egg_id, uint32_t *p_count);

};

#endif   /* ----- #ifndef CUSER_CARD_INCL  ----- */

