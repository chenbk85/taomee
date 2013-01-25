
/*
 * =====================================================================================
 * 
 *       Filename:  Cpet.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:   CST
 *       Revision:  none
 *       Compiler:  g++
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CATTIRE_INC
#define  CATTIRE_INC
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"
#include "Citem_change_log.h"

class Cattire: public CtableRoute
{
private:
	enum MOLE2_ATTIRE_POS_TYPE{
		MOLE2_ATTIRE_ALL,
		MOLE2_ATTIRE_IN_BAG,
		MOLE2_ATTIRE_ON_BODY,
		MOLE2_ATTIRE_IN_STORAGE,
		MOLE2_ATTIRE_BODY_BAG,
	};
	int insert(userid_t userid, uint32_t time, stru_mole2_user_attire_info* p_in);
	int getlist(userid_t userid, MOLE2_ATTIRE_POS_TYPE type,uint32_t start,uint32_t count,mole2_user_attire_get_list_out_item** p_out_item, uint32_t* p_out_count);
	int set_body_null(userid_t userid);
private:
	const char* str_userid;
	const char* str_gettime;
	const char* str_attireid;
	const char* str_gridid;
	const char* str_level;
	const char* str_mduration;
	const char* str_duration;
	const char* str_hpmax;
	const char* str_mpmax;
	const char* str_atk;
	const char* str_matk;
	const char* str_def;
	const char* str_mdef;
	const char* str_speed;
	const char* str_spirit;
	const char* str_resume;
	const char* str_hit;
	const char* str_dodge;
	const char* str_crit;
	const char* str_fightback;
	const char* str_rpoison;
	const char* str_rlithification;
	const char* str_rlethargy;
	const char* str_rinebriation;
	const char* str_rconfusion;
	const char* str_roblivion;
	const char* str_quality;
	const char* str_validday;
	const char* str_crystal_attr;
	const char* str_bless_type;
	Citem_change_log *p_item_change_log;
	int del(userid_t userid, uint32_t gettime);
public:
	Cattire(mysql_interface * db ,Citem_change_log * p_log );

	int del_attire(userid_t userid, uint32_t gettime, uint32_t *p_attireid, bool is_vip_opt=false );
	int get_attire_list(userid_t userid, stru_mole2_user_attire_info_1** p_out_item, uint32_t* p_out_count);

	int get_attire_list_1(userid_t userid, stru_mole2_user_attire_info_1** p_out_item, uint32_t* p_out_count);

	int set_field_value(uint32_t userid, su_mole2_set_field_value_in* p_in);
	int storage_list_get(userid_t userid, uint32_t start,uint32_t count,
			stru_mole2_user_attire_info_1** p_out_item, user_attire_in_storage_list_get_out_header* out_hdr);

	int get_attire_list_ex(userid_t userid, 
			stru_mole2_user_attire_info_1** p_out_item, uint32_t* p_out_count);
	int get_attire(userid_t userid,uint32_t key, stru_mole2_user_attire_info_1* p_out);
	int set_attr(userid_t userid, uint32_t key, mole2_user_attire_attr_add_list_out_item* p_item);
	int update_gridid(userid_t userid, uint32_t gettime, uint32_t gridid);

	int update_gridid_list(userid_t userid, mole2_user_attire_update_gridid_list_in_item* p_in_item, uint32_t in_count);

	int get_attire_simple(uint32_t userid, uint32_t gettime, uint32_t* p_attire_id, uint32_t* p_level);

	int attire_add(userid_t userid, stru_mole2_user_attire_info* p_in, uint32_t* p_gettime = NULL,bool is_vip_opt=false);

	int duration_set(uint32_t userid, uint32_t attireid, uint16_t duration);

	int mending_set(uint32_t userid, uint32_t attireid, uint16_t mduration, uint16_t duration);

	int get_cloths(userid_t userid, std::vector<stru_cloth_info> &cloths,uint32_t grid_min,uint32_t grid_max);
	int get_attire_id(userid_t userid,uint32_t key, uint32_t *p_attire_id );
	int get_count_by_type(userid_t userid,uint32_t clothid,uint32_t &count);
	int get_list_count(userid_t userid,MOLE2_ATTIRE_POS_TYPE type,uint32_t &count);
};

#endif /* ----- #ifndef CPET_INC  ----- */
