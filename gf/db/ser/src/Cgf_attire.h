#ifndef  GF_CGF_ATTIRE_INCL
#define    GF_CGF_ATTIRE_INCL
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

#define ATTIRE_USED 1
#define ATTIRE_UNUSED 0
#define MAX_WEAR_CLOTHES_NUM 10
#define MAX_DURATION_VALUE  (100 * 50)
#define MAX_ATTIRE_CNT 50000


struct add_attire_t{
	uint32_t attireid;
	uint32_t gettime;
	uint32_t endtime;
	uint32_t attire_rank;
	uint32_t duration;
	uint32_t usedflag;
	uint32_t attire_lv;
};

class Cgf_attire:public CtableRoute{
	public:
		Cgf_attire(mysql_interface * db ); 

		int get_used_clothes_list(userid_t userid, uint32_t role_regtime, uint32_t* p_count, 
				gf_get_role_list_out_item_2** pp_list);
		int get_used_clothes_list_with_duration(userid_t userid, uint32_t role_regtime, uint32_t* p_count, gf_attire_id_duration_list** pp_list);
		int get_package_clothes_list(userid_t userid, uint32_t role_regtime, uint32_t* p_count, gf_attire_id_duration_list** pp_list);

		int get_clothes_list(userid_t userid, uint32_t role_regtime, uint32_t* p_count, 
				gf_get_clothes_list_out_item** pp_list);
		int get_clothes_list_ex(userid_t userid, uint32_t role_regtime, uint32_t* p_count, 
				gf_get_clothes_list_ex_out_item** pp_list);

		int get_clothes_by_timelist(userid_t userid, uint32_t role_regtime, gf_wear_clothes_in_item* p_in_item, 
				uint32_t in_count, gf_wear_clothes_out_item** pp_out_list, uint32_t* p_out_count);
		int set_all_clothes_noused(userid_t userid, uint32_t role_regtime);
        int set_clothes_noused(userid_t userid, uint32_t role_regtime, uint32_t id);
        int set_clothes_outdated(userid_t userid, uint32_t role_regtime, uint32_t outdate);
		
		int get_clothes_outdated(userid_t userid, uint32_t role_regtime, uint32_t outdate, 
				uint32_t** pp_out_list, uint32_t* p_out_count);
		
		int set_clothes_used_by_timelist(userid_t userid, uint32_t role_regtime, 
				gf_wear_clothes_in_item* p_in_item, uint32_t in_count);
		
		int set_clothes_flag_equip_part(userid_t userid, uint32_t role_regtime, 
			uint32_t unique_id, uint32_t equip_part);
		
		int get_clothes_info_for_web(userid_t userid, uint32_t role_regtime, uint32_t* p_count, 
			gf_get_clothes_info_for_web_out_item** pp_out_list);
		
		int get_user_attireid_by_usedflg(userid_t userid, uint32_t role_regtime, 
				uint32_t* p_count, gf_get_user_partial_info_out_item** pp_list, uint32_t usedflg);
        int get_outdated_attire_by_index(userid_t userid, uint32_t role_regtime, uint32_t key, uint32_t* id);
        int get_attire_by_usedflg(userid_t userid,uint32_t role_regtime,uint32_t* p_cnt,
            gf_get_outdated_product_out_item** pp_list, uint32_t usedflg);
		int del_attire(userid_t userid,uint32_t role_regtime,gf_del_attire_in* p_in);


		int del_attire_with_index(userid_t userid,uint32_t role_regtime,uint32_t index);
		int del_attire_by_attireid(userid_t userid,uint32_t role_regtime,uint32_t attireid);
        int del_timeout_attire_by_attireid(userid_t userid,uint32_t role_regtime,uint32_t attireid);

		int add_attire(userid_t userid,uint32_t role_regtime,add_attire_t* p_in,  uint32_t max_attire_bag_count);
		int add_attire(userid_t userid,uint32_t role_regtime,add_attire_t* p_in,  uint32_t max_attire_bag_count, uint32_t *last_insert_id);

		int get_attire_cnt(userid_t userid, uint32_t role_regtime, uint32_t* p_count, uint32_t usedflag);
        int get_attire_cnt_by_attireid(userid_t userid, uint32_t role_regtime, uint32_t* p_count, uint32_t id);
        int get_attire_cnt_by_unique_id(userid_t userid, uint32_t role_regtime, uint32_t* p_count, uint32_t id);

		int clear_role_attire(userid_t userid,uint32_t role_regtime);

		int get_max_id(userid_t userid,uint32_t role_regtime,uint32_t* p_iter_id);

		int get_max_id_list(userid_t userid, uint32_t role_regtime, uint32_t in_count,
			uint32_t* p_out_count, gf_buy_attire_out_item** pp_out_list);
		int get_max_id_list(userid_t userid, uint32_t role_regtime, uint32_t in_count,
			uint32_t* p_out_count, gf_task_swap_item_out_item** pp_out_list);

		int set_attire_list_duration(userid_t userid, uint32_t role_regtime, uint32_t in_count,
			gf_set_role_base_info_in_item* p_item);

		int update_duration (userid_t userid, uint32_t role_regtime, uint32_t attire_index, uint32_t duration);

		int get_duration_by_index (userid_t userid, uint32_t role_regtime, uint32_t attire_index, uint32_t* p_duration);
		int set_attire_int_value(userid_t userid,uint32_t role_regtime, uint32_t indexid ,const char * field_type , uint32_t  value);
		int get_attire_by_index(userid_t userid, uint32_t role_regtime, uint32_t id, uint32_t item_id, add_attire_t* p_in);
        int getback_attire_by_index(userid_t userid, uint32_t role_regtime, uint32_t id, uint32_t gettime,uint32_t endtime);

		int get_clothes_list_for_boss(userid_t userid, uint32_t role_regtime, uint32_t* p_count, gf_get_role_base_info_for_boss_out_item** pp_list);
	private:

};
#endif   /* ----- #ifndef GF_CGF_ATTIRE_INCL  ----- */
