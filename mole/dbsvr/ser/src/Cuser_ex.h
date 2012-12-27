/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_ex.h
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

#ifndef  CUSER_EX_INC
#define  CUSER_EX_INC
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#define  WATER_OPT_FLAG 	0x01 
#define  KILL_BUG_OPT_FLAG 	0x02 
#define  FEED_PET_OPT_FLAG 	0x04 
/// @brief 标志位，表示植物施肥
#define  PLANT_MUCK_OPT_FLAG (0x1<<5)

#define  USER_VIPFLAG_VIP    	  		0x00000001
#define  USER_VIPFLAG_IS_VIP_OLD    	0x00000002

#define  FRIEND_LIST_STR 	  		"friendlist" 
#define  BLACKUSER_LIST_STR 		"blacklist" 
#define  CLASS_LIST_STR 		"classlist" 
#define  CLASS_FIRSTID_STR 		"class_firstid" 

#define  USER_EX_FLAG_STR 		"flag" 



class Cuser_ex :public CtableRoute100x10
{
private:
	int update_jy_accesslist( userid_t userid, access_jylist *p_list ) ;
	userid_t _userid;

public:
	void userid(const userid_t uid)
	{
		_userid=uid;
	}

	Cuser_ex(mysql_interface * db ); 
	int init(userid_t userid, char *  nick );
	int get_jyaccess_list(userid_t userid,access_jylist *p_out );
	int access_jy (userid_t userid, user_access_jy_in *p_in );
	int jy_accesslist_set_opt (userid_t userid, uint32_t opt_userid ,uint32_t opt_type );


	int get_friend_black_list(userid_t userid ,uint32_t *p_flag2,
			id_list *p_friendlist, id_list *  p_blacklist );

	int add_friend(userid_t userid , userid_t id );

	int update_idlist(userid_t userid , const char * id_flag_str ,  id_list * idlist );

	int del_id(userid_t userid ,const char * id_flag_str , userid_t id );

	int get_idlist(userid_t userid ,const  char * id_flag_str , id_list * idlist );

	int set_flag ( userid_t userid  ,const  char * flag_type  , 
			uint32_t flag_bit ,  bool is_true );

	int get_flag(userid_t userid ,const char * flag_type   ,  uint32_t * flag);

	int update_flag(userid_t userid ,const char * flag_type  , uint32_t  flag);
	int check_friend(userid_t userid , userid_t friendid, uint32_t *p_is_existed  );
	int check_black(userid_t userid , userid_t friendid, uint32_t *p_is_existed  );

	int add_id(userid_t userid ,const  char * id_flag_str , userid_t id,
		uint32_t allow_max_count );
	int get_nick_fiendlist(userid_t userid , char * nick , id_list * idlist );
	int login(userid_t userid , user_login_ex_out  *p_out  );

	int get_info(userid_t userid ,stru_user_other *p_out );

	int update_nick(userid_t userid  ,  char * nick );
	int get_tmpbuf(userid_t userid ,user_get_tempbuf_out *p_out );
	int update_tmpbuf(userid_t userid  ,  user_set_tempbuf_in *p_in );

	int get_info(userid_t userid , user_get_user_all_out *p_out );

	int get_vip_nick(userid_t userid ,user_get_vip_nick_out  *p_out );

	int update_professionlist(userid_t userid  ,  stru_user_profession_list *p_in );
	int get_professionlist(userid_t userid ,stru_user_profession_list *p_out );

	int set_professionlist(userid_t userid  ,  user_profession_set_in*p_in );
	int get_nick(userid_t userid , char  *p_nick);

	int get_depot(userid_t userid, farm_depot *p_list, uint32_t type);
	int update_depot(userid_t userid, farm_depot *p_list, uint32_t type);
	int get_farm_goods(userid_t userid, all_goods_farm *p_farm);

	int get_classlist(userid_t userid ,stru_user_class_list *p_out ) ;
	int set_water_time(userid_t userid);
	int sub_depot(userid_t userid, food_info *p_list, uint32_t type);
	int update_lock(userid_t userid  ,  uint32_t lock);
	int set_lock(userid_t userid  ,  uint32_t lock);
	int get_lock(userid_t userid, uint32_t *p_out);
	int get_vip(userid_t userid ,bool * is_vip );
	int update_driver(userid_t userid  ,  uint32_t onlinetime);
	int get_pic_clothe(userid_t userid, user_ex_pic_clothe  *p_out);
	int update_pic_clothe(userid_t userid, user_ex_pic_clothe *p_in);
	int get_vip_pet_level(userid_t userid, uint32_t& vip_pet_level);
	int get_nick_clothe(userid_t userid, char *nick,  user_ex_pic_clothe  *p_out);

	/**
	 * @brief 设置VIP用户的开始时间与结束时间
	 * @param uint32_t start_time 开始时间
	 * @param uint32_t end_time 结束时间
	 * @return 数据错误或成功
	 */
	int vip_start_end_time_set(const user_set_vip_flag_in& in);

	/**
	 * @brief 设置VIP用户的上次的等级值，它作为此次的开始值
	 * @param uint32_t vip_mark_last 降要设置的等级值
	 * @return 数据错误或成功
	 */
	int vip_mark_last_set(const uint32_t vip_mark_last);

	int user_vip_set_flag_no_check(const user_set_vip_flag_in& in);

	int update_build_level_inc(userid_t userid, uint32_t build_level);
	
	int set_event_flag(userid_t userid, uint32_t index);
	int reset_event_flag(userid_t userid, uint32_t index);
	int set_sendmail_flag(userid_t userid, uint32_t index);
	int set_sendmail_flag(userid_t userid, uint32_t index, uint32_t email_flag);
	int get_months(userid_t userid, user_ex_get_month_out *p_out);
	int update_months(userid_t userid, user_ex_add_month_in *p_in);
	int inc_months(userid_t userid, uint32_t months);
	int add_bonus_used(userid_t userid, uint32_t value);
	int get_board_info(userid_t userid ,user_get_user_board_info_out_header *p_out );
	int update_nourish_level_inc(userid_t userid, uint32_t nourish_level);
	int clear_bonus_used(userid_t userid);
	int get_user_color_nick(userid_t userid, char* p_nick, uint32_t *color);
	int get_two_cols(userid_t userid, const char* col_1, const char* col_2, uint32_t &value_1, uint32_t &value_2);
	int update_two_cols(userid_t userid, const char* col_1, const char* col_2, uint32_t value_1, uint32_t value_2);
	int batch_del_friends(userid_t userid, const char* id_flag_str, uint32_t in_count, user_batch_del_friends_in_item* p_in_item,
			uint32_t *real_del_count);
	int inc_login_vip_mark(userid_t userid, uint32_t add_vip_mark);
	int update_one_col(userid_t userid, const char* col, uint32_t value);
};


#endif   /* ----- #ifndef cuser_ex_inc  ----- */

