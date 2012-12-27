#ifndef  GF_CGF_SKILL_H
#define    GF_CGF_SKILL_H
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

const uint32_t max_skill_lv = 11;

struct add_skill_t{
	uint32_t skill_id;
	uint32_t skill_lv;
	uint32_t bind_key;
}__attribute__((packed));

struct skill_info_t{
	uint32_t skill_id;
	uint32_t skill_lv;
	uint32_t skill_point;
}__attribute__((packed));

class Cgf_skill:public CtableRoute{
	public:
		Cgf_skill(mysql_interface * db ); 
		
		//int add_skill(userid_t userid, uint32_t role_regtime,gf_add_skill_in* p_in);
		int add_skill(userid_t userid, uint32_t role_regtime,add_skill_t* p_in);
		int add_skill(userid_t userid, uint32_t role_regtime, uint32_t skill_id, uint32_t skill_lv = 1);
		
		int del_skill(userid_t userid, uint32_t role_regtime,gf_del_skill_in* p_in);
		
		int update_skill(userid_t userid, uint32_t role_regtime,gf_upgrade_skill_in* p_in);

		int upgrade_skill(userid_t userid, uint32_t role_regtime,gf_upgrade_skill_in* p_in);
		
		int get_skill_list(userid_t userid, uint32_t role_regtime, uint32_t* p_count, 
			gf_get_skill_list_out_item** pp_list);

		int get_skill_points(userid_t userid, uint32_t role_regtime, uint32_t* p_count);

		int reset_skills(userid_t userid, uint32_t role_regtime);

		int clear_skills(userid_t userid, uint32_t role_regtime);
		
		int get_skill_lv(userid_t userid, uint32_t role_regtime, uint32_t skill_id,
			uint32_t* p_lv);

		int clear_role_skill(userid_t userid,uint32_t role_regtime);

#if 0
		int set_bind_key(userid_t userid, uint32_t role_regtime,uint32_t skill_id, 
			uint32_t bind_id);

		int get_skill_bind_key(userid_t userid, uint32_t role_regtime,uint32_t *p_count,
			gf_get_skill_bind_key_out_item** pp_list);

		int clear_skill_bind_key(userid_t userid, uint32_t role_regtime);
#endif
		int get_skill_info(userid_t userid, uint32_t role_regtime,uint32_t skill_id,
			skill_info_t* p_out);

		int update_skill_info(userid_t userid, uint32_t role_regtime, uint32_t skill_id,
			skill_info_t* p_out);
		
		int skill_level_up(userid_t userid, uint32_t role_regtime, uint32_t skill_id);
		
		int update_skill_point(userid_t userid, uint32_t role_regtime, uint32_t skill_id,
			uint32_t skill_point);

		int get_login_skills_info(userid_t userid, uint32_t role_regtime, 
			uint32_t* p_count, gf_get_skill_bind_key_out_item** pp_list);
		
		int clear_role_info(userid_t userid,uint32_t role_regtime);
};

#endif  




