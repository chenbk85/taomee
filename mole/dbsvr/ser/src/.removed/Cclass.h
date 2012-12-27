/*
 * =====================================================================================
 * 
 *       Filename:  Cgroup.h
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

#ifndef  CCLASS_INCL
#define  CCLASS_INCL
#include <map>
#include "CtableRoute10x10.h"
#include "proto.h"
#include "benchapi.h"

using namespace std;


class Cclass:public CtableRoute10x10 {
private:
	const uint32_t name_len;
	const uint32_t slogan_len;
	const uint32_t member_len;

	int set_memberlist(uint32_t classid, class_member *p_item);
public:
	Cclass(mysql_interface * db ); 
	int	get_member(uint32_t classid, class_member *p_out);
	int insert(uint32_t classid, class_add_in *p_item );
	int get(uint32_t groupid, class_get_out *p_out);
	int set_msg(uint32_t groupid, group_msg_item*p_item );
	int add_member( uint32_t classid, userid_t addid, class_member *p_memberlist);
	int del_member( uint32_t classid, uint32_t memberid, class_member *p_memberlist);
	int del( uint32_t classid);
	int get_noused_attirelist(uint32_t classid, class_noused_homeattirelist *p_list);
	int get_used_attirelist(uint32_t classid, class_used_homeattirelist *p_list);
	int update_noused_attirelist(uint32_t classid, class_noused_homeattirelist *p_list);
	int update_used_attirelist(uint32_t classid, class_used_homeattirelist *p_list);
	int add_home_attire(uint32_t classid, attire_count_with_max *p_item);
	int get_homeattirelist(uint32_t classid, class_used_homeattirelist *p_usedlist,
			                              class_noused_homeattirelist *p_nousedlist);
    int add_homeattire_noused(class_noused_homeattirelist *p_list, attire_count_with_max * p_item);	
	int update_homeattirelist(uint32_t userid, home_attirelist * p_usedlist,
			                                noused_homeattirelist * p_nousedlist );
	int update_homeattire_all(uint32_t classid, class_used_homeattirelist *p_usedlist,
			                                class_noused_homeattirelist * p_nousedlist );
	void get_home_attire_total(ATTIRE_MAPITEM * p_itemmap,
			                class_used_homeattirelist * p_usedlist,
							                class_noused_homeattirelist * p_nousedlist);
	int class_change_info(int classid, class_change_info_in *p_item);
	int class_get_benefactor(uint32_t classid, class_benefactor_info *p_list);
	int class_benefactor_attire(uint32_t classid, userid_t userid, attire_count_with_max *item);
	int class_get_state(uint32_t classid, uint32_t *state);
	int class_set_state(uint32_t classid, uint32_t state);
};

#endif   /* ----- #ifndef CGROUP_INCL  ----- */

