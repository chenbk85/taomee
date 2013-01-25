
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
 *       Compiler:  g++
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

#define MOLE2_MAX_EXPERIENCE	75468400	//经验最大值
#define	MOLE2_MAX_PHYSIQUE		300			//体力最大值
#define	MOLE2_MAX_STRENGTH		300			//力量最大值
#define	MOLE2_MAX_ENDURANCE		300			//耐力最大值
#define	MOLE2_MAX_QUICK			300			//敏捷最大值
#define	MOLE2_MAX_INTELLIGENCE	300			//智力最大值
#define MOLE2_MAX_ATTR_ADDTION	426			//剩余属性点最大值

#define MOLE2_FLAG_VIP			0x00000001	//VIP用户标志位

class Cuser:public CtableRoute100x10
{
	enum {
		MOLE2_MAX_XIAOMEE		= 999999999,		//小米最大值
		MOLE2_MAX_XIAOMEE_DAY	= 20000,			//每天获得的小米最大值
		MOLE2_MAX_HP 			= 0xFFFFFFFF,		//生命最大值
		MOLE2_MAX_MP			= 0xFFFFFFFF,		//魔法最大值
		MOLE2_USER_MAX_LEVEL	= 100,				//人物等级最大值
	};
private:
	int	insert(userid_t userid, userid_t parent,char* nick, char* sign, uint32_t color, uint8_t professtion, uint32_t regtime);
	int get_attr_addition(userid_t userid, mole2_user_user_add_attr_addition_out* p_out);
	int set_attr_addition(userid_t userid, mole2_user_user_add_attr_addition_out* p_out);	
	int get_level_exp_attr(userid_t userid, uint32_t* p_level, uint32_t* p_exp, uint16_t* p_attr);
	int set_level_exp_attr(userid_t userid, uint32_t level, uint32_t exp, uint16_t attr);
	int get_friend_black_list(userid_t userid, uint32_t* flag,
			stru_id_list* p_friendlist, stru_id_list* p_blacklist);
	int del_id(userid_t userid, const char * id_flag_str , userid_t id );
	int get_idlist(userid_t userid ,const  char * id_flag_str , stru_id_list * p_idlist );
	int update_idlist(userid_t userid , const char * id_flag_str ,  stru_id_list * p_idlist );
public:
	const char* str_uid;
	const char* str_nick;
	const char* str_sign;
	const char* str_flag;
	const char* str_color;
	const char* str_regtime;
	const char* str_race;
	const char* str_profess;
	const char* str_honor;
	const char* str_xiaomee;
	const char* str_level;
	const char* str_exp;
	const char* str_physique;
	const char* str_strength;
	const char* str_endurance;
	const char* str_quick;
	const char* str_iq;
	const char* str_attradd;
	const char* str_hp;
	const char* str_mp;
	const char* str_msglist;
	const char* str_friendlist;
	const char* str_blacklist;
	const char* str_injury_state;
	const char* str_front;
	const char* str_joblevel;
	const char* str_max_attire;
	const char* str_max_medicine;
	const char* str_max_stuff;
	const char* str_logout;
	const char* str_axis_x;
	const char* str_axis_y;
	const char* str_mapid;
	const char* str_maptype;
	const char* str_login;
	const char* str_daytime;
	const char* str_fly_mapid;
	const char* str_expbox;
	const char* str_energy;
	const char* str_skill_expbox;
	const char* str_flag_ex;
	const char* str_ban_flag;
	const char* str_winbossid;
	const char* str_parent;
	const char* str_lastonline;
	const char* str_total_time;
	const char* str_levelup_time;
	const char* str_sign_count;
	const char* str_shop_name;
public:
	Cuser (mysql_interface * db);
	int get_all_info(userid_t userid , stru_mole2_user_user_info* p_out );
	int get_online_login(userid_t userid, mole2_online_login_out* p_out);

	int get_all_info(userid_t userid  , stru_mole2_user_user_info_ex* p_out);
	
	int get_login_info(userid_t userid, mole2_user_user_login_out_header* p_out_header, 
			mole2_user_user_login_out_item** pp_idlist);
	
	int get_offline_msg(userid_t userid, mole2_user_user_get_offline_msg_out* p_out);
	
	int set_offline_msg(userid_t userid, mole2_user_user_get_offline_msg_out* p_out);
	
	int	create_role(userid_t userid, mole2_user_user_create_role_in* p_in);
	
	int update_nick(userid_t userid, mole2_user_user_update_nick_in* p_in); 
	
	int update_signature(userid_t userid, mole2_user_user_update_signature_in* p_in); 
	
	int update_color(userid_t userid, mole2_user_user_update_color_in* p_in); 
	
	int update_professtion(userid_t userid, mole2_user_user_update_professtion_in* p_in);
	
	int add_xiaomee(userid_t userid, int32_t add_value, mole2_user_user_add_xiaomee_out* p_out, int add_limit = 0);
	
	int add_exp(userid_t userid, int32_t exp, mole2_user_user_add_exp_out* p_out);
	
	int add_attr_addition(userid_t userid, mole2_user_user_add_attr_addition_in* p_in,
			mole2_user_user_add_attr_addition_out* p_out);

	int get_hp_mp(userid_t userid, mole2_user_user_add_hp_mp_out* p_out);

	int add_hp_mp(userid_t userid, int hp, int mp, uint32_t injury_state);

	int set_hp_mp(userid_t userid, uint32_t hp, uint32_t mp);
	int set_hp_mp_injury(userid_t userid, uint32_t hp, uint32_t mp, uint32_t injury);
	
	int get_base_attr(userid_t userid, mole2_user_user_add_base_attr_out* p_attr);
	
	int set_base_attr(userid_t userid, mole2_user_user_add_base_attr_in* p_in);
	
	//int update_hp_mp(userid_t userid, mole2_user_user_set_hp_mp_in* p_in,
	//		mole2_user_user_set_hp_mp_out* p_out);
	//int add_four_attr(userid_t userid, mole2_user_user_add_four_attr_in* p_in,
	//		mole2_user_user_add_four_attr_out* p_out);
	
	int add_friend(userid_t userid, userid_t uid);
	
	int del_friend(userid_t userid, userid_t uid);
	
	int get_friend_list(userid_t userid, mole2_user_user_get_friend_list_out* p_out);
	
	int check_is_friend(userid_t userid, mole2_user_user_check_is_friend_in* p_in, 
			mole2_user_user_check_is_friend_out* p_out);
	
	int add_blackuser(userid_t userid, userid_t uid);
	
	int del_blackuser(userid_t userid, userid_t uid);
	
	int injury_state_update(userid_t userid, uint32_t injury_state);
	
	int get_black_list(userid_t userid, mole2_user_user_get_black_list_out* p_out);

	int injury_state_select(uint32_t userid, uint32_t* injury_state);
	
	int add_xiaomee(uint32_t userid, int32_t value);
	
	int add_exp(uint32_t userid, uint32_t value);
	
	int set_pos(uint32_t userid, uint8_t pos);

	int	update_role(userid_t userid, char* nick, uint32_t prof,uint32_t color, uint16_t* p_attr, uint32_t hp, uint32_t mp);

	int levelup_update(uint32_t userid, stru_mole2_user_levelup_info*p_in);

	int flag_set(uint32_t userid, uint32_t flag, uint32_t flag_ex);

	int reward_set(uint32_t userid, uint32_t xiaomee, uint32_t experience, uint32_t level, uint32_t attr_addition, uint32_t hp, uint32_t mp, uint32_t expbox);

	int reward_set_ex(uint32_t userid, uint32_t xiaomee, uint32_t experience, uint32_t level,
				uint32_t attr_addition, uint32_t hp, uint32_t mp, uint32_t expbox, uint32_t skill_expbox,
				uint32_t flag_ex,	uint32_t joblevel);

	int xiaomee_get(uint32_t userid, uint32_t* p_xiaomee);

	int profession_set(userid_t userid, uint8_t profession);

	int update_total_time(uint32_t userid,uint32_t timeadd);
	int logout_position_set(uint32_t userid, uint32_t time, uint32_t axis_x, uint32_t axis_y, uint32_t mapid,  uint32_t maptype,uint32_t daytime, uint32_t fly_mapid,uint32_t last_online);

	int logout_position_get(uint32_t userid, uint32_t* p_time, uint32_t* p_axis_x, uint32_t* p_axis_y, uint32_t* p_mapid);

	int honor_set(uint32_t userid, uint32_t titleid);

	int login_set(uint32_t userid, uint32_t login);

	int login_set(uint32_t userid, uint32_t login, uint32_t daytime);

	int login_get(uint32_t userid, uint32_t* p_login, uint32_t* p_daytime);

	int nick_get(uint32_t userid, char* sz_nick);

	int exp_box_set(userid_t userid, uint32_t expbox);
	int exp_box_get(userid_t userid, uint32_t* expbox);

	int exp_info_set(uint32_t userid, uint32_t exp, uint32_t level, uint32_t attr, uint32_t hp, uint32_t mp, uint32_t expbox);

	int for_home_get(uint32_t userid, char* sz_nick, uint32_t* p_expbox,uint32_t* p_joblvl);

	int energy_set(uint32_t userid, uint32_t energy);
	int set_ban_flag(userid_t,uint32_t ban_flag);
	int check_ban_flag(userid_t userid);
	int set_win_bossid(userid_t userid, uint32_t bossid);
	int set_joblevel(userid_t userid,uint32_t joblevel);
	int set_field_value(userid_t userid, su_mole2_set_field_value_in *p_in);
	int set_shop_name(userid_t userid, char* nick);
	int get_shop_name(userid_t userid, char* nick);
};

#endif /* ----- #ifndef CUSER_INC  ----- */
